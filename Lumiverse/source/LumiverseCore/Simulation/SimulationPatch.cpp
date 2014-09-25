
#include "SimulationPatch.h"
#include "types/LumiverseFloat.h"
#include "imageio.h"
#include <sstream>

namespace Lumiverse {

SimulationPatch::SimulationPatch(const JSONNode data) :
m_renderloop(NULL), m_blend(NULL) {
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

}

/*!
* \brief Destroys the object.
*/
SimulationPatch::~SimulationPatch() {
	delete[] m_blend;
	m_blend = NULL;
	for (auto light : m_lights) {
		delete light.second;
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
    
void SimulationPatch::blendUint8(float* blended, unsigned char* light, 
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
			}
		}
	}
}

void SimulationPatch::renderLoop() {
	// Set to zeros.
	unsigned char *buffer;
	bool toclear = true;

	// TODO: Potential optimization here. Data casting.
	// Read and blend one by one
	for (auto record : m_lights) {
		PhotoLightRecord *photo = (PhotoLightRecord *)record.second;
		buffer = imageio_load_image(photo->metadata.c_str(), &m_width, &m_height);

		// Allocate or clear once we know the size.
		size_t img_size = (size_t)(m_height * m_width * 4);
		if (!m_blend) {
			m_blend = new float[img_size];
			std::memset(m_blend, 0, img_size * sizeof(float));
		}
		else if (toclear) {
			toclear = false;
			std::memset(m_blend, 0, img_size * sizeof(float));
		}
			
		if (buffer)
			blendUint8(m_blend, buffer, photo->intensity, photo->color);
		else {
			std::stringstream sstm;
			sstm << "Unable to read file: " << photo->metadata.c_str();

			Logger::log(WARN, sstm.str());
		}
	}
}

void SimulationPatch::interruptRender() {
	
}

void SimulationPatch::onDeviceChanged(Device *d) {
    // TODO : LOCK
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
		m_lights[light.first]->init();
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
