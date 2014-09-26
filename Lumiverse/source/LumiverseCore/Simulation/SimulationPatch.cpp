
#include "SimulationPatch.h"
#include "types/LumiverseFloat.h"
#include "imageio.h"
#include <sstream>

namespace Lumiverse {

SimulationPatch::SimulationPatch(const JSONNode data) :
m_renderloop(NULL), m_blend(NULL), m_blend_buffer(NULL) {
	m_interrupt_flag.test_and_set();
	loadJSON(data);
}

void SimulationPatch::loadJSON(const JSONNode data) {
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

void SimulationPatch::loadLight(Device *d_ptr) {
    std::string light_name = d_ptr->getId();

	loadLight(light_name);
}

void SimulationPatch::loadLight(std::string light) {
	std::string light_name = light;

	if (m_lights.count(light_name) > 0) {
		unsigned char *buffer;
		PhotoLightRecord *record = (PhotoLightRecord *)m_lights[light_name];
		buffer = imageio_load_image(record->metadata.c_str(), &m_width, &m_height);

		if (!buffer) {
			std::stringstream sstm;
			sstm << "Unable to read file: " << record->metadata.c_str();

			Logger::log(WARN, sstm.str());

			return;
		}

		if (record->photo) {
			delete[] record->photo;
			record->photo = NULL;
		}

		record->photo = new float[m_width * m_height * 4];
		bytes_to_floats(record->photo, buffer, m_width, m_height);
	}

}

/*!
* \brief Destroys the object.
*/
SimulationPatch::~SimulationPatch() {
	delete[] m_blend;
	m_blend = NULL;
	delete[] m_blend_buffer;
	m_blend_buffer = NULL;

	for (auto light : m_lights) {
		delete light.second;
		light.second = NULL;
	}
}

bool SimulationPatch::isUpdateRequired(set<Device *> devices) {
    bool req = false;
    
    for (Device* d : devices) {
		std::string name = d->getId();
		if (m_lights.count(name) == 0)
			continue;
		
        if (m_lights[d->getId()]->rerender_req) {
            req = true;
            break;
        }
	}
    
    return req;
}
    
void SimulationPatch::updateLight(set<Device *> devices) {
	for (Device* d : devices) {
		std::string name = d->getId();
		if (m_lights.count(name) == 0)
			continue;
		PhotoLightRecord *light = (PhotoLightRecord *)m_lights[d->getId()];
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

void SimulationPatch::clearUpdateFlags() {
    for (auto& record : m_lights) {
        record.second->rerender_req = false;
    }
}
    
bool SimulationPatch::blendFloat(float* blended, float* light, 
	float intensity, Eigen::Vector3f color) {
	Eigen::Vector4f rgba(color[0], color[1], color[2], 1.f);

	// Assume they are of size (m_width, m_height)
	for (size_t i = 0; i < m_height; i++) {
		for (size_t j = 0; j < m_width; j++) {
			for (size_t ch = 0; ch < 4; ch++) {
				size_t offset = (m_width * i + j) * 4 + ch;
				size_t offset_des = (m_width * (m_height - 1 - i) + j) * 4 + ch;
				if (ch < 3)
					blended[offset] += light[offset] * rgba[ch] * intensity;
				else
					blended[offset] += light[offset] * rgba[ch];

				if (!m_interrupt_flag.test_and_set())
					goto exit_blend;
			}
		}
	}

	return true;

exit_blend:
	return false;
}

bool SimulationPatch::blendUint8(float* blended, unsigned char* light, 
	float intensity, Eigen::Vector3f color) {
	Eigen::Vector4f rgba(color[0], color[1], color[2], 1.f);

	// Assume they are of size (m_width, m_height)
	for (size_t i = 0; i < m_height; i++) {
		for (size_t j = 0; j < m_width; j++) {
			for (size_t ch = 0; ch < 4; ch++) {
				size_t offset = (m_width * i + j) * 4 + ch;
				size_t offset_des = (m_width * (m_height - 1 - i) + j) * 4 + ch;
				float cvt_channel = static_cast<float>(light[offset]) / 0xff;
				if (ch < 3)
					blended[offset_des] += cvt_channel * rgba[ch] * intensity;
				else
					blended[offset_des] += cvt_channel * rgba[ch];

				blended[offset_des] = (blended[offset_des] > 1) ? 1.f : blended[offset_des];

				if (!m_interrupt_flag.test_and_set())
					goto exit_blend_int;
			}
		}
	}

	return true;

exit_blend_int:
	return false;
}

bool SimulationPatch::renderLoop() {
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

void SimulationPatch::interruptRender() {
	m_interrupt_flag.clear();
	if (m_renderloop &&
		m_renderloop->joinable())
		m_renderloop->join();
}

void SimulationPatch::onDeviceChanged(Device *d) {
    if (m_lights.count(d->getId()) > 0) {
        m_lights[d->getId()]->rerender_req = true;
    }
}
    
void SimulationPatch::update(set<Device *> devices) {
	bool render_req = isUpdateRequired(devices);

    if (!render_req) {
        return ;
    }
    updateLight(devices);
    clearUpdateFlags();
    
    interruptRender();
    
    m_renderloop = new std::thread(&SimulationPatch::renderLoop, this);
}

void SimulationPatch::init() {
	// Init patch and interface
	for (auto light : m_lights) {
		light.second->init();
		loadLight(light.first);
	}

	size_t img_size = (size_t)(m_height * m_width * 4);
	if (!m_blend) {
		m_blend = new float[img_size];
		std::memset(m_blend, 0, img_size * sizeof(float));
		m_blend_buffer = new float[img_size];
		std::memset(m_blend_buffer, 0, img_size * sizeof(float));
	}
}

void SimulationPatch::close() {
    interruptRender();
    // close files
}

JSONNode SimulationPatch::toJSON() {
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
	root.push_back(lights.as_array());

	return root;
}
    
void SimulationPatch::rerender() {
    // Given at least one light exists
    // This will triggle a rerender later.
    if (m_lights.size() > 0)
        m_lights.begin()->second->rerender_req = true;
}

}
