
#include "ArnoldPatch.h"
#include "types/LumiverseFloat.h"
#include <sstream>

namespace Lumiverse {

#ifndef _WIN32
	void convertPlugin(std::string &dir) {
		for (size_t i = 0; i < dir.size(); i++) {
			if (dir.at(i) == ';')
				dir[i] = ':';
		}
	}
#endif

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
			std::string plugin = dir.as_string();
#ifndef _WIN32
			convertPlugin(plugin);
#endif
			m_interface.setPluginDirectory(plugin);
		}
        
        if (nodeName == "gamma") {
            JSONNode gamma = *i;
            m_interface.setGamma(gamma.as_float());
		}
        
        if (nodeName == "samples") {
            JSONNode samples = *i;
            m_interface.setSamples(samples.as_int());
		}

		if (nodeName == "lights") {
			JSONNode lights = *i;
			JSONNode::const_iterator light = lights.begin();
			while (light != lights.end()) {
				std::string light_name = light->name();

				m_lights[light_name] = ArnoldLightRecord();
				m_lights[light_name].arnold_type = light->find("type")->as_string();

				std::stringstream sstm;
				sstm << "Added light " << light_name << ": " << m_lights[light_name].arnold_type;

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
    AtNode *light_ptr;
    
    if (m_lights.count(light_name) == 0)
        return ;
	std::string type = m_lights[light_name].arnold_type;
    
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
            ss << rgb[0] << ", " << rgb[1] << ", " << rgb[2];
            m_interface.setParameter(light_ptr, param, ss.str());
        }
		// Assume pan and tilt are named as "pan" and "tilt"
		else if (raw->getTypeName() == "orientation" &&
				param == "tilt") {
			LumiverseOrientation *tilt = (LumiverseOrientation*)raw;
			LumiverseOrientation *pan = (LumiverseOrientation*)d_ptr->getParam("pan");

			if (pan == NULL)
				continue;

			std::string lookat_str;
			std::string up_str;
			std::string pos;
			if (!d_ptr->getMetadata("lookat", lookat_str) ||
				!d_ptr->getMetadata("up", up_str) ||
				!d_ptr->getMetadata("position", pos)) {
				continue;
			}

			ArnoldParameterVector<3, float> lookat_vec;
			parseArnoldParameter<3, float>(lookat_str, lookat_vec);
			ArnoldParameterVector<3, float> up_vec;
			parseArnoldParameter<3, float>(up_str, up_vec);

			Eigen::Vector3f lookat(lookat_vec[0], lookat_vec[1], lookat_vec[2]);
			Eigen::Vector3f up(up_vec[0], up_vec[1], up_vec[2]);

			Eigen::Matrix3f rotation = LumiverseTypeUtils::getRotationMatrix(lookat, up, pan, tilt);

			std::stringstream ss;
			ss << rotation(0, 0) << "," << rotation(0, 1) << "," << rotation(0, 2) << ",0,"
				<< rotation(1, 0) << "," << rotation(1, 1) << "," << rotation(1, 2) << ",0,"
				<< rotation(2, 0) << "," << rotation(2, 1) << "," << rotation(2, 2) << ",0,"
				<< pos << ",1";

			Logger::log(LDEBUG, light_name);
			Logger::log(LDEBUG, ss.str());

			m_interface.setParameter(light_ptr, "matrix", ss.str());
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
		loadLight(d);
	}
}

void ArnoldPatch::clearUpdateFlags() {
    for (auto& record : m_lights) {
        record.second.rerender_req = false;
    }
}
    
void ArnoldPatch::renderLoop() {
    m_interface.render();
}

void ArnoldPatch::interruptRender() {
    m_interface.interrupt();
    
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
    }
}
    
void ArnoldPatch::setSamples(int samples) {
    m_interface.setSamples(samples);
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
	JSONNode root;

	root.push_back(JSONNode("type", getType()));
	root.push_back(JSONNode("sceneFile", m_interface.getAssFile()));
	root.push_back(JSONNode("pluginDir", m_interface.getPluginDirectory()));
	root.push_back(JSONNode("gamma", m_interface.getGamma()));
	
	JSONNode lights;
	lights.set_name("lights");

	for (auto light : m_lights) {
		lights.push_back(JSONNode("light", light.first));
	}
	root.push_back(lights.as_array());

	root.push_back(m_interface.arnoldParameterToJSON());

	return root;
}
    
void ArnoldPatch::rerender() {
    // Given at least one light exists
    // This will triggle a rerender later.
    if (m_lights.size() > 0)
        m_lights.begin()->second.rerender_req = true;
}

}
