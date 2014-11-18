
#include "SimulationPatch.h"
#include "types/LumiverseFloat.h"
#include "imageio.h"
#include <sstream>

namespace Lumiverse {

SimulationPatch::SimulationPatch(const JSONNode data) :
m_renderloop(NULL) {
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

		i++;
	}
}

/*!
* \brief Destroys the object.
*/
SimulationPatch::~SimulationPatch() {
	for (auto light : m_lights) {
		light.second->clear();
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
    
	if (!req) {
		std::set<std::string> existingDevs;
		for (Device *dev : devices) {
			existingDevs.insert(dev->getId());
		}

		for (auto it = m_lights.cbegin(); !m_lights.empty() && it != m_lights.cend(); ) {
			if (existingDevs.count(it->first) == 0) {
				it->second->clear();
				delete it->second;
				auto del = it++;
				m_lights.erase(del);
			}
			else
				it++;
		}
	}

    return req;
}

void SimulationPatch::clearUpdateFlags() {
    for (auto& record : m_lights) {
        record.second->rerender_req = false;
    }
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
    
	bindRenderLoop();
}

void SimulationPatch::bindRenderLoop() {
	m_renderloop = new std::thread(&SimulationPatch::renderLoop, this);
}

void SimulationPatch::init() {
	// Init patch and interface
	for (auto light : m_lights) {
		light.second->init();
	}
}

void SimulationPatch::close() {
    interruptRender();
    // close files
	for (auto & record : m_lights) {
		record.second->clear();
	}
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
	root.push_back(lights);

	return root;
}
    
void SimulationPatch::rerender() {
    // Given at least one light exists
    // This will triggle a rerender later.
    if (m_lights.size() > 0)
        m_lights.begin()->second->rerender_req = true;
}

}
