
#include "ArnoldPatch.h"
#include "types/LumiverseFloat.h"
#include <sstream>

namespace Lumiverse {

ArnoldPatch::ArnoldPatch(const JSONNode data) :
m_renderloop(NULL) {
	loadJSON(data);
}

void ArnoldPatch::loadJSON(const JSONNode data) {
	string patchName = data.name();

	// Load options for raytracer application. (window, ray tracer, filter)
	JSONNode::const_iterator i = data.begin();

	while (i != data.end()) {
		std::string nodeName = i->name();

		if (nodeName == "sceneFile") {
          JSONNode fileName = *i;
          m_interface.setAssFile(fileName.as_string());
		}
        
        if (nodeName == "pluginDir") {
            JSONNode dir = *i;
            m_interface.setPluginDirectory(dir.as_string());
		}
        
        if (nodeName == "gamma") {
            JSONNode gamma = *i;
            m_interface.setGamma(gamma.as_float());
		}
        
        
        if (nodeName == "lights") {
			JSONNode lights = *i;
			JSONNode::const_iterator light = lights.begin();
			while (light != lights.end()) {
				std::string light_name = light->name();
                
				m_lights[light_name] = ArnoldLightRecord();
                
                std::stringstream sstm;
                sstm << "Added light " << light_name;

				Logger::log(INFO, sstm.str());
                
				light++;
			}
		}
        
        if (nodeName == "arnoldParamMaps") {
			JSONNode params = *i;
			JSONNode::const_iterator param = params.begin();
			while (param != params.end()) {
				std::string param_name = param->name();

                m_interface.loadArnoldParam(*param);
                
                std::stringstream sstm;
                sstm << "Added param " << param_name;
                
				Logger::log(INFO, sstm.str());
                
				param++;
			}
		}

		i++;
	}

}
    
void ArnoldPatch::loadLight(Device *d_ptr) {
    std::string light_name = d_ptr->getId();
    std::string type = d_ptr->getType();
    AtNode *light_ptr;
    
    if (m_lights.count(light_name) == 0)
        return ;
    
    if (m_lights[light_name].light == NULL) {
        light_ptr = AiNode(type.c_str());
        AiNodeSetStr(light_ptr, "name", light_name.c_str());
    }
    else {
        light_ptr = m_lights[light_name].light;
    }

    // TODO: mesh_light
    for (std::string meta : d_ptr->getMetadataKeyNames()) {
        std::string value;
        d_ptr->getMetadata(meta, value);
        m_interface.setParameter(light_ptr, meta, value);
    }
    
    // Sets arnold params with device params
    // This process is after parsing metadata, so parameters here can overwrite values from metadata
    for (std::string param : d_ptr->getParamNames()) {
        LumiverseType *raw = d_ptr->getParam(param);
        
        // First parse lumiverse type into string. So we can reuse the function for metadata.
        // It's obviously inefficient.
        if (raw->getTypeName() == "float") {
            m_interface.setParameter(light_ptr, param, ((LumiverseFloat*)raw)->asString());
        }
        else if (raw->getTypeName() == "color") {
            Eigen::Vector3d rgb = ((LumiverseColor*)raw)->getRGB();
            std::stringstream ss;
            ss << rgb[0] << ", " << rgb[1] << ", ", rgb[2];
            m_interface.setParameter(light_ptr, param, ss.str());
        }
    }

    m_lights[light_name].light = light_ptr;
}

/*!
* \brief Destroys the object.
*/
ArnoldPatch::~ArnoldPatch() {

}

bool ArnoldPatch::isUpdateRequired(set<Device *> devices) {
    bool req = false;
    
    for (Device* d : devices) {
		std::string name = d->getId();
		if (m_lights.count(name) == 0)
			continue;
		
        if (m_lights[d->getId()].rerender_req) {
            req = true;
            break;
        }
	}
    
    return req;
}
    
void ArnoldPatch::updateLight(set<Device *> devices) {
	for (Device* d : devices) {
		std::string name = d->getId();
		if (m_lights.count(name) == 0)
			continue;
		
        if (m_lights[d->getId()].rerender_req) {
            loadLight(d);
        }
	}
}

void ArnoldPatch::clearUpdateFlags() {
    for (auto& record : m_lights) {
        record.second.rerender_req = false;
    }
}
    
void ArnoldPatch::renderLoop() {
    Logger::log(INFO, "Rendering...");
    AiRender(AI_RENDER_MODE_CAMERA);
    Logger::log(INFO, "Done.");
}

void ArnoldPatch::interruptRender() {
    if (AiRendering()) {
        AiRenderInterrupt();
        Logger::log(INFO, "Aborted rendering to restart.");
    }
    if (m_renderloop != NULL) {
        try {
            m_renderloop->join();
        }
        catch (const std::system_error& e) {
            Logger::log(ERR, "Thread doesn't exist.");
        }
        m_renderloop = NULL;
    }
}

void ArnoldPatch::onDeviceChanged(Device *d) {
    // TODO : LOCK
    if (m_lights.count(d->getId()) > 0) {
        m_lights[d->getId()].rerender_req = true;
        Logger::log(LDEBUG, "Intensity changed...");
    }
}
    
void ArnoldPatch::update(set<Device *> devices) {
	bool render_req = isUpdateRequired(devices);

    if (!render_req) {
        return ;
    }
    updateLight(devices);
    clearUpdateFlags();
    
    interruptRender();
    
    m_renderloop = new std::thread(&ArnoldPatch::renderLoop, this);
}

void ArnoldPatch::init() {
    m_interface.init();
}

void ArnoldPatch::close() {
    interruptRender();
    m_interface.close();
}

JSONNode ArnoldPatch::toJSON() {
	return JSONNode("test", 0);
}

}
