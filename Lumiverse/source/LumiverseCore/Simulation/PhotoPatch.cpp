
#include "PhotoPatch.h"

#ifdef USE_ARNOLD
#include "types/LumiverseFloat.h"
#include "imageio.h"
#include <sstream>

namespace Lumiverse {

PhotoPatch::PhotoPatch(const JSONNode data) :
m_blend(NULL), m_blend_buffer(NULL) {
	m_interrupt_flag.test_and_set();
	loadJSON(data);
}

void PhotoPatch::loadJSON(const JSONNode data) {
	string patchName = data.name();

	// Load options for raytracer application. (window, ray tracer, filter)
	JSONNode::const_iterator i = data.begin();

	while (i != data.end()) {
		std::string nodeName = i->name();

		if (nodeName == "jsonPath") {
			JSONNode path = *i;

			// TODO: better separator
			std::string directory = path.as_string();
			size_t slash;
			if ((slash = directory.find_last_of("/")) != string::npos) {
				directory = directory.substr(0, slash + 1);
			}
			else if ((slash = directory.find_last_of("\\")) != string::npos) {
				directory = directory.substr(0, slash + 1);
			}
			else
				directory += "/";
			
		}

		if (nodeName == "width") {
			JSONNode path = *i;

			m_width = i->as_int();
		}

		if (nodeName == "height") {
			JSONNode path = *i;

			m_height = i->as_int();
		}

		if (nodeName == "lights") {
			JSONNode lights = *i;
			JSONNode::const_iterator light = lights.begin();
			while (light != lights.end()) {
				std::string light_name = light->name();

				m_lights[light_name] = new PhotoLightRecord();
				m_lights[light_name]->metadata = light->find("filename")->as_string();

				std::stringstream sstm;
				sstm << "Added light " << light_name << ": " << m_lights[light_name]->metadata;

				Logger::log(INFO, sstm.str());

				light++;
			}
		}

		i++;
	}
}

void PhotoPatch::loadLight(Device *d_ptr) {
	std::string light_name = d_ptr->getId();

	if (m_lights.count(light_name) > 0) {
		unsigned char *buffer = NULL;
		PhotoLightRecord *record = (PhotoLightRecord *)m_lights[light_name];
		int width, height;

		if (record->photo) {
			return;
		}

		buffer = imageio_load_image(record->metadata.c_str(), &width, &height);

		if (!buffer) {
			std::stringstream sstm;
			sstm << "Unable to read file: " << record->metadata.c_str();

			Logger::log(WARN, sstm.str());

			return;
		}

		record->photo = new float[m_width * m_height * 4];
		bytes_to_floats(record->photo, buffer, m_width, m_height);

		delete[] buffer;
		buffer = NULL;

	}

}

/*!
* \brief Destroys the object.
*/
PhotoPatch::~PhotoPatch() {
	delete[] m_blend;
	m_blend = NULL;
	delete[] m_blend_buffer;
	m_blend_buffer = NULL;
}
    
void PhotoPatch::updateLight(set<Device *> devices) {
	for (Device* d : devices) {
		std::string name = d->getId();
		if (m_lights.count(name) == 0)
			continue;

		PhotoLightRecord *light = (PhotoLightRecord *)m_lights[d->getId()];
		if (light->photo == NULL)
			loadLight(d);

		LumiverseColor *color = d->getColor();
		if (color != NULL) {
			light->color[0] = (float)color->getRGB()[0];
			light->color[1] = (float)color->getRGB()[1];
			light->color[2] = (float)color->getRGB()[2];
		}
		if (!d->getParam("intensity", light->intensity))
			light->intensity = 0;
	}
}

bool PhotoPatch::blendFloat(float* blended, float* light, 
	float intensity, Eigen::Vector3f color) {
	Eigen::Vector4f rgba(color[0], color[1], color[2], 1.f);

	// Assume they are of size (m_width, m_height)
	for (int i = 0; i < m_height; i++) {
		for (int j = 0; j < m_width; j++) {
			for (size_t ch = 0; ch < 4; ch++) {
				size_t offset = (m_width * i + j) * 4 + ch;
				size_t offset_des = (m_width * (m_height - 1 - i) + j) * 4 + ch;
				if (ch < 3)
					blended[offset] += light[offset] * rgba[ch] * intensity;
				else
					blended[offset] += light[offset] * rgba[ch];
			}
		}
		if (!m_interrupt_flag.test_and_set())
			return false;
	}

	return true;
}

bool PhotoPatch::blendUint8(float* blended, unsigned char* light, 
	float intensity, Eigen::Vector3f color) {
	Eigen::Vector4f rgba(color[0], color[1], color[2], 1.f);

	// Assume they are of size (m_width, m_height)
	for (int i = 0; i < m_height; i++) {
		for (int j = 0; j < m_width; j++) {
			for (size_t ch = 0; ch < 4; ch++) {
				size_t offset = (m_width * i + j) * 4 + ch;
				size_t offset_des = (m_width * (m_height - 1 - i) + j) * 4 + ch;
				float cvt_channel = static_cast<float>(light[offset]) / 0xff;
				if (ch < 3)
					blended[offset_des] += cvt_channel * rgba[ch] * intensity;
				else
					blended[offset_des] += cvt_channel * rgba[ch];

				blended[offset_des] = (blended[offset_des] > 1) ? 1.f : blended[offset_des];
			}
		}
		if (!m_interrupt_flag.test_and_set())
			return false;
	}

	return true;
}

void PhotoPatch::bindRenderLoop() {
	m_renderloop = new std::thread(&PhotoPatch::renderLoop, this);
}

bool PhotoPatch::renderLoop() {
	// Set to zeros.
	bool toclear = true;
	size_t img_size = (size_t)(m_height * m_width * 4);
	bool success = true;

	m_interrupt_flag.test_and_set();

	// Blend one by one
	for (auto record : m_lights) {
		PhotoLightRecord *light = (PhotoLightRecord *)record.second;

		if (toclear) {
			toclear = false;
			std::memset(m_blend_buffer, 0, img_size * sizeof(float));
		}
		
		if (light->photo && light->intensity > 0 &&
			!light->color.isZero())
			success = blendFloat(m_blend_buffer, light->photo, light->intensity, light->color);
		else if (!light->photo) {
			std::stringstream sstm;
			sstm << "Empty file: " << light->metadata.c_str();

			Logger::log(WARN, sstm.str());
		}

		if (!success)
			break;
	}

	if (success)
		std::memcpy(m_blend, m_blend_buffer, img_size * sizeof(float));

	return success;
}

void PhotoPatch::init() {
	// Init patch and interface
	SimulationPatch::init();

	size_t img_size = (size_t)(m_height * m_width * 4);
	if (!m_blend) {
		m_blend = new float[img_size];
		std::memset(m_blend, 0, img_size * sizeof(float));
		m_blend_buffer = new float[img_size];
		std::memset(m_blend_buffer, 0, img_size * sizeof(float));
	}
}

JSONNode PhotoPatch::toJSON() {
	JSONNode root;

	root.push_back(JSONNode("type", getType()));

	JSONNode lights;
	lights.set_name("lights");

	for (auto light : m_lights) {
		JSONNode lightNode;
		lightNode.set_name(light.first);
		lightNode.push_back(JSONNode("filename", light.second->metadata));
		lights.push_back(lightNode);
	}
	root.push_back(lights);

	return root;
}
    

}
#endif