
#include "DistributedArnoldInterface.h"
#include "types/LumiverseFloat.h"
#include <sstream>

namespace Lumiverse {

	static size_t write_buffer_callback(void *ptr, size_t size, size_t nmemb, void *userdata) {
		size_t num_processed = size * nmemb;

		struct RequestBuffer *bitmap_buffer = (struct RequestBuffer *)userdata;
		bitmap_buffer->buffer = (char *)realloc(bitmap_buffer->buffer, bitmap_buffer->num_written + num_processed + 1);

		std::memcpy(bitmap_buffer->buffer + bitmap_buffer->num_written, ptr, num_processed);
		bitmap_buffer->num_written += num_processed;

		return num_processed;
	}

	void DistributedArnoldInterface::init() {
		std::cout << "Initializing connection in DistributedArnoldInterface" << std::endl;

		if (m_open) {
			std::cout << "Tried to open a connection when one was already open. You must close this connection before  opening a new one" << std::endl;

			return;
		}

		if (!openConnection()) {
			std::cout << "Unable to open connection" << std::endl;

			return;
		}

		if (!sendAssFileRequest()) {
			std::cout << "Unable to send ass file to opened server" << std::endl;

			return;
		}

		m_open = true;
	}

	void DistributedArnoldInterface::init(const std::set<Device *> &devices) {
		// Only set callbacks if connection was opened
		if (m_open) {

			// Set all of the devices to use the deviceUpdateCallback
			for (auto i = devices.begin(); i != devices.end(); i++) {
				Device::DeviceCallbackFunction callback = std::bind(
					&DistributedArnoldInterface::deviceUpdateCallback,
					(DistributedArnoldInterface *)this,
					std::placeholders::_1);
				(*i)->addParameterChangedCallback(callback);
			}
		} else {
			std::cerr << "Connection was not opened -- not registering callbacks" << std::endl;
		}
	}

	bool DistributedArnoldInterface::openConnection() {

		// send request to open endpoint
		m_curl_connection.reset();
		m_curl_connection.add<CURLOPT_URL>((m_host_name + "/open").c_str());
		m_curl_connection.add<CURLOPT_PORT>(m_host_port);
		struct RequestBuffer request_buffer;
		bindRequestBuffer(&request_buffer);

		try {
			// Fire off request
			m_curl_connection.perform();
		}
		catch (curl::curl_easy_exception error) {
			auto errors = error.what();

			error.print_traceback();

			if (request_buffer.buffer != NULL) {
				delete request_buffer.buffer;
			}

			return false;
		}

		bool wasSuccessful = remoteRequestSuccessful(libjson::parse(request_buffer.buffer));

		if (request_buffer.buffer != NULL) {
			delete request_buffer.buffer;
		}

		return wasSuccessful;
	}

	bool DistributedArnoldInterface::remoteRequestSuccessful(JSONNode response) {
		auto successNode = response.find("success");
		auto messageNode = response.find("msg");

		bool success = successNode->as_bool();
		std::string message = messageNode->as_string();

		// Print message out on every response
		std::cout << "Successful: " << success << ", response message: " << message << std::endl;

		return success;
	}

	float DistributedArnoldInterface::getPercentage() {

		if (!m_open) {
			std::cout <<
				"Can't check progress without opening a connection. Send a request to the /open endpoint first by calling init()."
				<< std::endl;

			return 0.f;
		}

		m_curl_connection.reset();
		m_curl_connection.add<CURLOPT_URL>((m_host_name + "/percent").c_str());
		m_curl_connection.add<CURLOPT_PORT>(m_host_port);
		struct RequestBuffer request_buffer;
		bindRequestBuffer(&request_buffer);

		try {

			m_curl_connection.perform();
		} catch (curl::curl_easy_exception error) {
			curl::curlcpp_traceback errors = error.get_traceback();
			error.print_traceback();

			return 0.f;
		}

		JSONNode response = libjson::parse(request_buffer.buffer);
		bool wasSuccessful = remoteRequestSuccessful(response);

		float percentage = 0.0f;
		if (wasSuccessful) {
 			percentage = response.find("percent")->as_float();
		}

		if (request_buffer.buffer != NULL) {
			delete request_buffer.buffer;
		}

		return percentage;
	}

	bool DistributedArnoldInterface::deflateAss() {

		// Gzip the file with maximum compression. Sending the ass file over the
		// wire is one of the bigger bottlenecks in the distributed setting
		// so we want to maximize compression
		gzFile to_compress = gzopen((getAssFile() + ".gz").c_str(), "wb9");

		// set the buffer to pretty big -- user is expected to have a lot of RAM anyways
		gzbuffer(to_compress, CHUNK);

		// Read ass file into memory
		FILE *input = fopen(getAssFile().c_str(), "rb");
		size_t total_read = 0;
		unsigned char *in = new unsigned char[CHUNK + 1];
		while (!feof(input)) {
			size_t num_read = fread((void *)(in + total_read), 1, CHUNK, input);
			total_read += num_read;
			in = (unsigned char *)realloc(in, total_read + CHUNK + 1);
		}
		fclose(input);

		// Gzip assfile
		int num_written = gzwrite(to_compress, in, total_read);
		gzclose(to_compress);
		delete in;

		return true;
	}

	bool DistributedArnoldInterface::sendAssFileRequest() {
	
		// gzip the ass file before we send it over the wire
		deflateAss();

		// Send request to init 
		m_curl_connection.reset();
		m_curl_connection.add<CURLOPT_URL>((m_host_name + "/init").c_str());
		m_curl_connection.add<CURLOPT_PORT>(m_host_port);

		curl::curl_form post_form;

		// m_gamma
		std::string m_gamma_option = "m_gamma";
		std::string m_gamma_value = std::to_string(getGamma());
		curl::curl_pair<CURLformoption, std::string> gamma_form(CURLFORM_COPYNAME, m_gamma_option);
		curl::curl_pair<CURLformoption, std::string> gamma_cont(CURLFORM_COPYCONTENTS, m_gamma_value);

		// m_predictive
		std::string m_predictive_option = "m_predictive";
		std::string m_predictive_value = std::to_string(getPredictive());
		curl::curl_pair<CURLformoption, std::string> predictive_form(CURLFORM_COPYNAME, m_predictive_option);
		curl::curl_pair<CURLformoption, std::string> predictive_cont(CURLFORM_COPYCONTENTS, m_predictive_value);

		// Buffer output
		std::string m_buffer_option = "m_buffer_output";
		std::string m_buffer_value = getFileOutputPath();
		curl::curl_pair<CURLformoption, std::string> output_form(CURLFORM_COPYNAME, m_buffer_option);
		curl::curl_pair<CURLformoption, std::string> output_cont(CURLFORM_COPYCONTENTS, m_buffer_value);

		// Ass file
		std::string m_ass_option = "ass_file";
		std::string m_ass_value = getAssFile() + ".gz";
		curl::curl_pair<CURLformoption, std::string> ass_form(CURLFORM_COPYNAME, m_ass_option);
		curl::curl_pair<CURLformoption, std::string> ass_cont(CURLFORM_FILE, m_ass_value);

		struct RequestBuffer json_buffer;
		bindRequestBuffer(&json_buffer);

		try {
			// build form and fire off request
			post_form.add(gamma_form, gamma_cont);
			post_form.add(predictive_form, predictive_cont);
			post_form.add(output_form, output_cont);
			post_form.add(ass_form, ass_cont);

			m_curl_connection.add<CURLOPT_HTTPPOST>(post_form.get());

			m_curl_connection.perform();
		} catch (curl::curl_easy_exception error) {
			curl::curlcpp_traceback errors = error.get_traceback();
			error.print_traceback();

			return false;
		}

		JSONNode data = libjson::parse(json_buffer.buffer);
		bool wasSuccessful = remoteRequestSuccessful(data);

		if (json_buffer.buffer != NULL) {
			delete json_buffer.buffer;
		}

		if (wasSuccessful) {
			m_width = data.find("m_width")->as_int();
			m_height = data.find("m_height")->as_int();
		}
		
		return wasSuccessful;
	}

	void DistributedArnoldInterface::bindRequestBuffer(struct RequestBuffer *buffer) {
		buffer->buffer = new char[1];
		buffer->num_written = 0;

		m_curl_connection.add<CURLOPT_WRITEFUNCTION>(write_buffer_callback);
		m_curl_connection.add<CURLOPT_WRITEDATA>(buffer);
	}

	int DistributedArnoldInterface::render() {
		if (!m_open) {
			init();

			// If we still weren't able to open a connection, return with a generic arnold error
			if (!m_open) {
				return AI_ERROR;
			}
		}


		m_curl_connection.reset();
		m_curl_connection.add<CURLOPT_URL>((m_host_name + "/render").c_str());
		m_curl_connection.add<CURLOPT_PORT>(m_host_port);
		struct RequestBuffer bitmap_buffer;
		bindRequestBuffer(&bitmap_buffer);

		try {
			std::string m_properties_option = "m_properties";
			std::string m_properties_value = "";

			// If the properties node is not null, set the value here
			if (properties_node != NULL) {
				json_string json_params = properties_node->write();
				std::string m_properties_value = json_params;
				delete properties_node;
				properties_node = NULL;
			}
			curl::curl_form post_form;
			curl::curl_pair<CURLformoption, std::string> properties_form(CURLFORM_COPYNAME, m_properties_option);
			curl::curl_pair<CURLformoption, std::string> properties_cont(CURLFORM_COPYCONTENTS, m_properties_value);
			post_form.add(properties_form, properties_cont);
			m_curl_connection.add<CURLOPT_HTTPPOST>(post_form.get());
			m_curl_connection.perform();
		} catch (curl::curl_easy_exception error) {
			curl::curlcpp_traceback errors = error.get_traceback();
			error.print_traceback();

			if (bitmap_buffer.buffer != NULL) {
				delete bitmap_buffer.buffer;
			}

			return AI_ERROR;
		}

		m_buffer = (float *)bitmap_buffer.buffer;

		bool wasSuccessful = bitmap_buffer.num_written >= 1000;

		return wasSuccessful ? AI_SUCCESS : AI_ERROR;
	}

	void DistributedArnoldInterface::close() {
		if (!m_open) {
			std::cout << "Can't close connection -- connection not opened" << std::endl;

			return;
		}

		if (!closeConnection()) {
			std::cout << "Unable to close connection -- error ocurred" << std::endl;
		}

		m_open = false;
	}

	void DistributedArnoldInterface::interrupt() {
		if (!m_open) {
			std::cout << "Can't interrupt rendering -- connection was never opened. You must first make a call to init" << std::endl;

			return;
		}

		m_curl_connection.reset();
		m_curl_connection.add<CURLOPT_URL>((m_host_name + "/interrupt").c_str());
		m_curl_connection.add<CURLOPT_PORT>(m_host_port);

		try {
			m_curl_connection.perform();
		}
		catch (curl::curl_easy_exception error) {
			curl::curlcpp_traceback errors = error.get_traceback();
			error.print_traceback();

			return;
		}
	}

	bool DistributedArnoldInterface::closeConnection() {

		// send request to open endpoint
		m_curl_connection.reset();
		m_curl_connection.add<CURLOPT_URL>((m_host_name + "/close").c_str());
		m_curl_connection.add<CURLOPT_PORT>(m_host_port);
		struct RequestBuffer request_buffer;
		bindRequestBuffer(&request_buffer);

		try {
			// Fire off request
			m_curl_connection.perform();
		}
		catch (curl::curl_easy_exception error) {
			auto errors = error.what();

			error.print_traceback();

			if (request_buffer.buffer != NULL) {
				delete request_buffer.buffer;
			}

			return false;
		}

		bool wasSuccessful = remoteRequestSuccessful(libjson::parse(request_buffer.buffer));

		if (request_buffer.buffer != NULL) {
			delete request_buffer.buffer;
		}

		return wasSuccessful;
	}

	void DistributedArnoldInterface::deviceUpdateCallback(Device *device) {
		if (properties_node == NULL) {
			properties_node = new JSONNode();
		}

		properties_node->push_back(device->toJSON());
	}
}
