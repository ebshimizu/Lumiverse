
#include "ArnoldPatch.h"
#include "types/LumiverseFloat.h"
#include <sstream>

namespace Lumiverse {

/*!
* \brief Construct DMXPatch from JSON data.
*
* \param data JSONNode containing the DMXPatch object data.
*/
ArnoldPatch::ArnoldPatch(const JSONNode data) {
	loadJSON(data);
}

void ArnoldPatch::loadJSON(const JSONNode data) {
	string patchName = data.name();

	// Load options for raytracer application. (window, ray tracer, filter)
	JSONNode::const_iterator i = data.begin();

	// Two rounds. In the first round, initialize the size of output (window), so it's
	// possible to allocate memory for each light.
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

	m_lights[light_name].light = light_ptr;
}

/*!
* \brief Destroys the object.
*/
ArnoldPatch::~ArnoldPatch() {

}

bool ArnoldPatch::updateLight(set<Device *> devices) {
    bool rerender_rep = false;
    
	for (Device* d : devices) {
		std::string name = d->getId();
		if (m_lights.count(name) == 0)
			continue;
		
        if (m_lights[d->getId()].rerender_req) {
            rerender_rep = true;
            loadLight(d);
        }
	}
    
    return rerender_rep;
}

void ArnoldPatch::renderLoop() {
    Logger::log(INFO, "Rendering...");
    AiRender(AI_RENDER_MODE_CAMERA);
    Logger::log(INFO, "Done.");
}

void ArnoldPatch::interruptRender() {
    if (m_renderloop != NULL) {
        if (AiRendering()) {
            AiRenderInterrupt();
            Logger::log(INFO, "Aborted rendering to restart.");
        }
        m_renderloop->join();
        m_renderloop = NULL;
    }
}

void ArnoldPatch::onDeviceChanged(std::string deviceName) {
    // TODO : LOCK
    if (m_lights.count(deviceName) > 0) {
        m_lights[deviceName].rerender_req = true;
    }
}
    
/*!
* \brief Updates the values sent to the DMX network given the list of devices
* in the rig.
*
* The list of devices should be maintained outside of this class.
*/
// TODO: Relationship between Device and Light??
// (Simulation light)
void ArnoldPatch::update(set<Device *> devices) {
    //AiBegin();
    
	bool render_req = updateLight(devices);

    if (!render_req) {
        return ;
    }
    
    interruptRender();
    
    m_renderloop = new std::thread(&ArnoldPatch::renderLoop, this);
    
    // TODO : LOCK
    for (Device* d : devices) {
		std::string name = d->getId();
		if (m_lights.count(name) == 0)
			continue;
		m_lights[name].rerender_req = false;
	}

}

/*!
* \brief Initializes connections and other network settings for the patch.
*
* Call this AFTER all interfaces have been assigned. May need to call again
* if interfaces change.
*/
void ArnoldPatch::init() {
    m_interface.init();
}

/*!
* \brief Closes connections to the interfaces.
*/
void ArnoldPatch::close() {
    interruptRender();
    m_interface.close();
}

/*!
* \brief Exports a JSONNode with the data in this patch
*
* \return JSONNode containing the DMXPatch object
*/
JSONNode ArnoldPatch::toJSON() {
	return JSONNode("test", 0);
}

}
