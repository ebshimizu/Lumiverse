
#include "DistributedArnoldInterface.h"
#include "types/LumiverseFloat.h"
#include <sstream>
#include <unordered_map>

#ifdef USE_ARNOLD

#ifdef USE_DUMIVERSE

namespace Lumiverse {

	static size_t write_buffer_callback(void *ptr, size_t size, size_t nmemb, void *userdata) {
		size_t num_processed = size * nmemb;

		struct RequestBuffer *bitmap_buffer = (struct RequestBuffer *)userdata;
		bitmap_buffer->buffer = (char *)realloc(bitmap_buffer->buffer, bitmap_buffer->num_written + num_processed + 1);

		std::memcpy(bitmap_buffer->buffer + bitmap_buffer->num_written, ptr, num_processed);
		bitmap_buffer->num_written += num_processed;

		return num_processed;
	}

	void DistributedArnoldInterface::init(const JSONNode jsonPatch) {
		std::cout << "Initializing connection in DistributedArnoldInterface" << std::endl;

		if (m_remote_open) {
			std::cout << "Tried to open a connection when one was already open. You must close this connection before  opening a new one" << std::endl;

			return;
		}

		if (!openConnection()) {
			std::cout << "Unable to open connection" << std::endl;

			return;
		}

		if (!sendDistributedInitRequest(jsonPatch)) {
			std::cout << "Unable to send ass file to opened server" << std::endl;

			return;
		}

		m_remote_open = true;
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

		if (!m_remote_open) {
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

	bool DistributedArnoldInterface::sendDistributedInitRequest(const JSONNode jsonPatch) {
	
		// gzip the ass file before we send it over the wire
		deflateAss();

		// Send request to init 
		m_curl_connection.reset();
		m_curl_connection.add<CURLOPT_URL>((m_host_name + "/init").c_str());
		m_curl_connection.add<CURLOPT_PORT>(m_host_port);

		curl::curl_form post_form;

		// JSON patch
		std::string m_patch_option = "m_patch";
		std::string m_patch_value = jsonPatch.write();
		curl::curl_pair<CURLformoption, std::string> settings_form(CURLFORM_COPYNAME, m_patch_option);
		curl::curl_pair<CURLformoption, std::string> settings_cont(CURLFORM_COPYCONTENTS, m_patch_value);

		// Ass file
		std::string m_ass_option = "ass_file";
		std::string m_ass_value = getAssFile() + ".gz";
		curl::curl_pair<CURLformoption, std::string> ass_form(CURLFORM_COPYNAME, m_ass_option);
		curl::curl_pair<CURLformoption, std::string> ass_cont(CURLFORM_FILE, m_ass_value);

		struct RequestBuffer json_buffer;
		bindRequestBuffer(&json_buffer);

		try {
			// build form and fire off request
			
			post_form.add(ass_form, ass_cont);
			post_form.add(settings_form, settings_cont);

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

	int DistributedArnoldInterface::render(const std::set<Device *> &devices) {
		if (!m_remote_open) {
			std::cerr << "Cannot send render request -- " << 
				"you must first initialize the connection " << 
				"to the remote renderer by calling init" << std::endl;
			return AI_ERROR;
		}

		curl::curl_form post_form;
		m_curl_connection.reset();
		m_curl_connection.add<CURLOPT_URL>((m_host_name + "/render").c_str());
		m_curl_connection.add<CURLOPT_PORT>(m_host_port);
		struct RequestBuffer bitmap_buffer;
		bindRequestBuffer(&bitmap_buffer);

		// Devices / parameters node
		std::string m_parameters_option = "m_parameters";
		std::string m_parameters_value = this->getDevicesJSON(devices).write_formatted();
		curl::curl_pair<CURLformoption, std::string> m_parameters_form(CURLFORM_COPYNAME, m_parameters_option);
		curl::curl_pair<CURLformoption, std::string> m_parameters_cont(CURLFORM_COPYCONTENTS, m_parameters_value);
		
		// Settings node
		std::string m_settings_option = "m_settings";
		std::string m_settings_value = this->getSettingsJSON().write();
		curl::curl_pair<CURLformoption, std::string> m_settings_form(CURLFORM_COPYNAME, m_settings_option);
		curl::curl_pair<CURLformoption, std::string> m_settings_cont(CURLFORM_COPYCONTENTS, m_settings_value);

		try {

			post_form.add(m_parameters_form, m_parameters_cont);
			post_form.add(m_settings_form, m_settings_cont);
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
		if (!m_remote_open) {
			std::cout << "Can't close connection -- connection not opened" << std::endl;

			return;
		}

		if (!closeConnection()) {
			std::cout << "Unable to close connection -- error ocurred" << std::endl;
		}

		m_remote_open = false;
	}

	void DistributedArnoldInterface::interrupt() {
		if (!m_remote_open) {
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

	const JSONNode DistributedArnoldInterface::getDevicesJSON(const std::set<Device *> &devices) {
		JSONNode parameters_node;

		for (auto i = devices.begin(); i != devices.end(); i++) {
			parameters_node.push_back((*i)->toJSON());
		}

		return parameters_node;
	}

	const JSONNode DistributedArnoldInterface::getSettingsJSON() {
		JSONNode root;

		// First get all int options
		for (auto i = int_options.begin(); i != int_options.end(); i++) {
			JSONNode setting_node;
			setting_node.push_back(JSONNode("type", "int"));
			setting_node.push_back(JSONNode("name", i->first));
			setting_node.push_back(JSONNode("value", i->second));
			root.push_back(setting_node);
		}

		// Then get all float options
		for (auto i = float_options.begin(); i != float_options.end(); i++) {
			JSONNode setting_node;
			setting_node.push_back(JSONNode("type", "int"));
			setting_node.push_back(JSONNode("name", i->first));
			setting_node.push_back(JSONNode("value", i->second));
			root.push_back(setting_node);
		}

		return root;
	}

	void DistributedArnoldInterface::setOptionParameter(const std::string &paramName, int val) {
		if (strcmp(paramName.c_str(), "width") == 0) {
			m_width = val;
		} else if (strcmp(paramName.c_str(), "height") == 0) {
			m_height = val;
		}
		int_options[paramName] = val;
	}

	void DistributedArnoldInterface::setOptionParameter(const std::string &paramName, float val) {
		float_options[paramName] = val;
	}
  bool DistributedArnoldInterface::isDistributedOpen()
  {
    return m_remote_open;
  }
}

#endif // USE_DUMIVERSE

#endif // USE_ARNOLD