
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
SimulationPatch() {
	loadJSON(data);
}

void ArnoldPatch::loadJSON(const JSONNode data) {
	string patchName = data.name();

	// Load options for raytracer application. (window, ray tracer, filter)
	JSONNode::const_iterator i = data.begin();


	// As of 4/8/2016 this is only implemented for Arnold
	if (useDistributedRendering(data)) {
#ifdef USE_DUMIVERSE

		auto distributedNode = data.find("distributed");
		auto hostNode = distributedNode->find("host");
		auto portNode = distributedNode->find("port");
		auto outputPathNode = distributedNode->find("outputPath");

		string host = hostNode->as_string();
		int port = portNode->as_int();
		string outputPath = "";
		if (outputPathNode != distributedNode->end()) {
			string outputPath = outputPathNode->as_string();
		}

		stringstream ss;
		ss << "Using DistributedArnoldInterface with host " << host << " and port " << port;
		Logger::log(INFO, ss.str());

		// Should we use caching on the distributed side
		if (cacheRendering(data)) {
			m_interface = (DistributedCachingArnoldInterface *)new DistributedCachingArnoldInterface(host, port, outputPath);
			m_interface->setUsingCaching(true);
		} else {
			m_interface = (DistributedArnoldInterface *)new DistributedArnoldInterface(host, port, outputPath);
			m_interface->setUsingCaching(false);
		}
		m_using_distributed = true;
#endif
	}
	else if (cacheRendering(data)) {
#ifdef USE_ARNOLD_CACHING
		Logger::log(INFO, "Using ArnoldInterface with caching");
		m_interface = (CachingArnoldInterface *)new CachingArnoldInterface();
		m_interface->setUsingCaching(true);

    // Find caching options and set them in the caching interface
#endif
	} else {
		Logger::log(INFO, "Using ArnoldInterface");
		m_interface = (ArnoldInterface *)new ArnoldInterface();
		m_interface->setUsingCaching(false);
	}

	while (i != data.end()) {
		std::string nodeName = i->name();

		if (nodeName == "jsonPath") {
			JSONNode path = *i;

			// TODO: better separator
			std::string directory = path.as_string();
			int slash;
			if ((slash = directory.find_last_of("/")) != string::npos) {
				directory = directory.substr(0, slash + 1);
			}
			else if ((slash = directory.find_last_of("\\")) != string::npos) {
				directory = directory.substr(0, slash + 1);
			}
			else
				directory += "/";
			m_interface->setDefaultPath(directory);
		}

		if (nodeName == "sceneFile") {
			JSONNode fileName = *i;
			m_interface->setAssFile(fileName.as_string());
		}

		if (nodeName == "pluginDir") {
			JSONNode dir = *i;
			std::string plugin = dir.as_string();
#ifndef _WIN32
			convertPlugin(plugin);
#endif
			m_interface->setPluginDirectory(plugin);
	}

		if (nodeName == "gamma") {
			JSONNode gamma = *i;
			m_interface->setGamma(gamma.as_float());
		}

		if (nodeName == "predictive") {
			JSONNode predictive = *i;
			m_interface->setPredictive(predictive.as_bool());
		}

		// check if m_using_caching flag was passed
		// this is consumed by the distributed renderer when deciding
		// which buffer driver to use
		if (nodeName == "m_using_caching") {
			m_interface->setUsingCaching(i->as_bool());
		}
        
    if (nodeName == "samples") {
      JSONNode samples = *i;
      m_interface->setSamples(samples.as_int());
		}

    // Light loading is now done via automatic matching based
    // on the "Arnold Node Name" metadata field that should be
    // included by lights used in an arnold simulation
        
    if (nodeName == "arnoldParamMaps") {
			JSONNode params = *i;
			JSONNode::const_iterator param = params.begin();
			while (param != params.end()) {
				std::string param_name = param->name();

        m_interface->loadArnoldParam(*param);
        
        std::stringstream sstm;
        sstm << "Added param " << param_name;
                
				Logger::log(INFO, sstm.str());
                
				param++;
			}
		}

		i++;
	}

}

void ArnoldPatch::setOptionParameter(std::string paramName, int val) {
	m_interface->setOptionParameter(paramName, val);
}

void ArnoldPatch::setOptionParameter(std::string paramName, float val) {
	m_interface->setOptionParameter(paramName, val);
}

bool ArnoldPatch::cacheRendering(const JSONNode data) {
	auto cacheNode = data.find("cache_rendering");

	return (cacheNode != data.end()) && (cacheNode->as_bool());
}

bool ArnoldPatch::useDistributedRendering(const JSONNode data) {
	auto distributed = data.find("distributed");
	if (distributed == data.end()) {
		return false;
	}

	auto hostNode = distributed->find("host");
	if (hostNode == distributed->end()) {
		Logger::log(WARN, "Unable to determine hostname for distributing rendering -- make sure it is included in your rig. Defaulting to local rendering.");

		return false;
	}

	auto portNode = distributed->find("port");
	if (portNode == distributed->end()) {
		Logger::log(WARN, "Unable to determine port for distributing rendering -- make sure it is included in your rig. Defaulting to local rendering.");

		return false;
	}

	return true;
}


void ArnoldPatch::setOrientation(AtNode *light_ptr, Device *d_ptr, std::string pan_str, std::string tilt_str) {
	float pan_val;
	float tilt_val;

	std::istringstream iss_pan(pan_str);
	iss_pan >> pan_val;

	std::istringstream iss_tilt(tilt_str);
	iss_tilt >> tilt_val;

	LumiverseOrientation pan(pan_val, DEGREE, pan_val);
	LumiverseOrientation tilt(tilt_val, DEGREE, tilt_val);

	setOrientation(light_ptr, d_ptr, &pan, &tilt);
}

void ArnoldPatch::setOrientation(AtNode *light_ptr, Device *d_ptr, LumiverseOrientation *pan, LumiverseOrientation *tilt) {
	std::string lookat_str;
	std::string up_str;
	std::string pos;
	if (!d_ptr->getMetadata("lookat", lookat_str) ||
		!d_ptr->getMetadata("up", up_str) ||
		!d_ptr->getMetadata("position", pos)) {
		return ;
	}

	ArnoldParameterVector<3, float> lookat_vec;
	parseArnoldParameter<3, float>(lookat_str, lookat_vec);
	ArnoldParameterVector<3, float> up_vec;
	parseArnoldParameter<3, float>(up_str, up_vec);
	ArnoldParameterVector<3, float> pos_vec;
	parseArnoldParameter<3, float>(pos, pos_vec);

	Eigen::Vector3f lookat(lookat_vec[0] - pos_vec[0], lookat_vec[1] - pos_vec[1], lookat_vec[2] - pos_vec[2]);
	Eigen::Vector3f up(up_vec[0], up_vec[1], up_vec[2]);

	Eigen::Matrix3f rotation = LumiverseTypeUtils::getRotationMatrix(lookat, up, pan, tilt);

	std::stringstream ss;
	ss << rotation(0, 0) << "," << rotation(0, 1) << "," << rotation(0, 2) << ",0,"
		<< rotation(1, 0) << "," << rotation(1, 1) << "," << rotation(1, 2) << ",0,"
		<< rotation(2, 0) << "," << rotation(2, 1) << "," << rotation(2, 2) << ",0,"
		<< pos << ",1";

	m_interface->setParameter(light_ptr, "matrix", ss.str());
}

void ArnoldPatch::loadLight(Device *d_ptr) {    
	if (!d_ptr->metadataExists("Arnold Node Name"))
    return;

  string light_name = d_ptr->getMetadata("Arnold Node Name");
    
  // Sets arnold params with device params
  // This process is after parsing metadata, so parameters here can overwrite values from metadata

  // Instead of iterating through everything, we look for specific parameters that we
  // then map directly to Arnold node parameters

  // Internsity
  if (d_ptr->paramExists("intensity")) {
    LumiverseFloat* scaledVal = (LumiverseFloat*)LumiverseTypeUtils::copy(d_ptr->getIntensity());

    if (d_ptr->metadataExists("gel")) {
      *scaledVal *= (float)(ColorUtils::getTotalTrans(d_ptr->getMetadata("gel")));
    }

    m_interface->setParameter(light_name, "intensity", scaledVal->getVal());
    delete scaledVal;
  }

  // Position (the entire 3D transform)
  // method 1: look at point + position in spherical coordinates around that point
  if (d_ptr->paramExists("polar") && d_ptr->paramExists("azimuth") && d_ptr->paramExists("distance") &&
      d_ptr->paramExists("lookAtX") && d_ptr->paramExists("lookAtY") && d_ptr->paramExists("lookAtZ")) {
    // while a vec3 would be the most appropriate for the lookAt params, Lumiverse does not include
    // it as a built in type, as no lights really use a vec3 for any actual parameters

    float distance = ((LumiverseFloat*)d_ptr->getParam("distance"))->getVal();
    auto lookAtX = (LumiverseFloat*) d_ptr->getParam("lookAtX");
    auto lookAtY = (LumiverseFloat*) d_ptr->getParam("lookAtY");
    auto lookAtZ = (LumiverseFloat*) d_ptr->getParam("lookAtZ");
    Eigen::Vector3f lookAt(lookAtX->getVal(), lookAtY->getVal(), lookAtZ->getVal());

    float polar = ((LumiverseOrientation*)d_ptr->getParam("polar"))->valAsUnit(RADIAN);
    float azimuth = ((LumiverseOrientation*)d_ptr->getParam("azimuth"))->valAsUnit(RADIAN);

    // Calculate xyz position. The look at point is assumed to be the origin of the coordinate system.
    //Eigen::Vector3f pos(distance * sinf(polar) * cosf(azimuth), distance * sinf(polar) * sinf(azimuth), distance * cosf(polar));
    Eigen::Vector3f pos(distance * sinf(polar) * sinf(azimuth), distance * cosf(polar), distance * cosf(azimuth) * sinf(polar));

    // Translate point to world coordinates from local spherical coords.
    pos += lookAt;
    Eigen::Vector3f dir = lookAt - pos;
    dir.normalize();

    // Assumes y-up
    Eigen::Matrix3f rot = LumiverseTypeUtils::getRotationMatrix(dir, Eigen::Vector3f(0, 0, -1));

    m_interface->setParameter(light_name, "matrix", rot, pos);
  }
  // method 2: xyz position + roll + pitch + yaw
  // method 3: xyz position + lookAt

  // Color
  if (d_ptr->paramExists("color")) {
    LumiverseColor* color = (LumiverseColor*)d_ptr->getParam("color");
    m_interface->setParameter(light_name, "color", color->getColorChannel("Red"), color->getColorChannel("Green"), color->getColorChannel("Blue"));
  }

  // Softness
  if (d_ptr->paramExists("penumbraAngle")) {
    LumiverseFloat* angle = d_ptr->getParam<LumiverseFloat>("penumbraAngle");
    m_interface->setParameter(light_name, "penumbra_angle", angle->getVal());
  }

	//ArnoldLightRecord *record = (ArnoldLightRecord *)m_lights[light_name];
	//record->light = light_ptr;
}

/*!
* \brief Destroys the object.
*/
ArnoldPatch::~ArnoldPatch() {
	m_interface->close();
	delete m_interface;
}

void ArnoldPatch::modifyLightColor(Device *d, Eigen::Vector3d white) {
	//AtNode *light_ptr = getLightNode(d);

	//if (!light_ptr)
  //  return;

	Eigen::Vector3d rgb;
	if (d->getColor() != NULL) 
		rgb = d->getColor()->getRGB(sharpRGB);
	else {
		map<string, Eigen::Vector3d> basis;
    basis["White"] = d->getGelColor(); // Returns D65 if no gel present.
		unordered_map<string, double> channels;
		channels["White"] = 1;
		LumiverseColor white(channels, basis, ColorMode::ADDITIVE, 1);
		white.setColorChannel("White", 1);
		rgb = white.getRGB(sharpRGB);
	}

	rgb = Eigen::Vector3d(rgb[0] / white[0], rgb[1] / white[1], rgb[2] / white[2]);

	std::stringstream ss;
	ss << rgb[0] << ", " << rgb[1] << ", " << rgb[2];
	//m_interface->setParameter(light_ptr, "color", ss.str());
}
    
void ArnoldPatch::updateLight(set<Device *> devices) {
	if (m_interface->getPredictive()) {
		updateLightPredictive(devices);
		return;
	}

	for (Device* d : devices) {
		std::string name = d->getMetadata("Arnold Node Name");
		if (m_lights.count(name) == 0)
			continue;
		loadLight(d);
	}
}

void ArnoldPatch::updateLightPredictive(set<Device *> devices) {
	Device *dominant = NULL;
	float max_luminant = -1;
	LumiverseColor white(BASIC_RGB);
	white.setColorChannel("Red", 1);
	white.setColorChannel("Green", 1);
	white.setColorChannel("Blue", 1);

	for (Device* d : devices) {
		std::string name = d->getId();
		if (m_lights.count(name) == 0)
			continue;
		float intensity = ((LumiverseFloat*)d->getParam("intensity"))->getVal();
		std::string exp_str = "";
		float exposure = 0.f;

		if (d->getMetadata("exposure", exp_str)) {
			std::istringstream iss(exp_str);
			iss >> exposure;
		}

		float luminant;
		float color_intensity;
		if (d->getColor() != NULL)
			color_intensity = d->getColor()->getY();
		else {
			color_intensity = 100;// white.getY();
		}

		luminant = color_intensity * intensity * powf(2, exposure);

		if (luminant > max_luminant) {
			max_luminant = luminant;
			dominant = d;
		}

		loadLight(d);
	}

	if (!dominant)
		return;

	Eigen::Vector3d rgb_w (0.9220, 1.0446, 1.0878);
	/*
	if (dominant->getColor() != NULL)
		rgb_w = dominant->getColor()->getRGB(sharpRGB);
	else {
		rgb_w = white.getRGB(sharpRGB);
	}
	*/

	for (Device* d : devices) {
		std::string name = d->getId();
		if (m_lights.count(name) == 0)
			continue;
		modifyLightColor(d, rgb_w);
	}

	m_interface->updateSurfaceColor(rgb_w);
}
    
bool ArnoldPatch::renderLoop(const std::set<Device *> &devices) {
  if (!m_rendering) {
    m_rendering = true;
    int code = m_interface->render(devices);
    m_rendering = false;
    return (code == AI_SUCCESS);
  }
  else
    return false;
}

bool ArnoldPatch::renderLoop() {
  if (!m_rendering) {
    m_rendering = true;
    int code = m_interface->render();
    m_rendering = false;

    return (code == AI_SUCCESS);
  }
}

void ArnoldPatch::interruptRender() {
    m_interface->interrupt();
    
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
    
void ArnoldPatch::setSamples(int samples) {
    m_interface->setSamples(samples);
}
    
void ArnoldPatch::update(set<Device *> devices) {
	bool render_req = isUpdateRequired(devices);

    if (!render_req) {
        return ;
    }
    updateLight(devices);
    clearUpdateFlags();
    
    interruptRender();
    
	// Use arnold loop with devices
	int (ArnoldPatch::*renderLoop)() const = renderLoop;
    m_renderloop = new std::thread(renderLoop, this);
}

void ArnoldPatch::init() {
  m_rendering = false;

	if (!m_using_distributed) {

		// Init patch and interface
		m_interface->init();

		// Find lights and create the light records in the patch
		for (auto light : m_interface->getLights()) {
			ArnoldLightRecord* r = new ArnoldLightRecord();
			r->light = light.second;
			r->init();
			m_lights[light.first] = r;
		}
	}
}

void ArnoldPatch::close() {
	SimulationPatch::close();
    m_interface->close();
}

JSONNode ArnoldPatch::toJSON() {
	JSONNode root;

	root.push_back(JSONNode("type", getType()));
	root.push_back(JSONNode("sceneFile", m_interface->getAssFile()));
	root.push_back(JSONNode("pluginDir", m_interface->getPluginDirectory()));
	root.push_back(JSONNode("predictive", (m_interface->getPredictive()) ? 1 : 0));
	root.push_back(JSONNode("gamma", m_interface->getGamma()));
	root.push_back(JSONNode("m_using_caching", m_interface->isUsingCaching()));

	JSONNode lights;
	lights.set_name("lights");

	for (auto light : m_lights) {
		JSONNode lightNode;
		lightNode.set_name(light.first);
		lightNode.push_back(JSONNode("type", light.second->metadata));
		lights.push_back(lightNode);
	}
	root.push_back(lights);

	root.push_back(m_interface->arnoldParameterToJSON());

	return root;
}

}
