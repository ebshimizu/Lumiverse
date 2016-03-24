
#include "ArnoldAnimationPatch.h"

#include <sstream>

namespace Lumiverse {

// Uses chrono::system_clock::from_time_t(0) as an invalid value.
ArnoldAnimationPatch::ArnoldAnimationPatch(const JSONNode data)
	: SimulationAnimationPatch(), m_preview_samples(m_interface.getSamples()),
    m_render_samples(m_interface.getSamples()) {
    // TODO: type for frame manager
	m_mem_frameManager = new ArnoldMemoryFrameManager();
    loadJSON(data);
}
    
ArnoldAnimationPatch::~ArnoldAnimationPatch() {
    // If close() hasn't been called, closes here.
    if (m_worker != NULL)
        close();
}

void ArnoldAnimationPatch::loadJSON(const JSONNode data) {
	ArnoldPatch::loadJSON(data);
	SimulationAnimationPatch::loadJSON(data);
}

void ArnoldAnimationPatch::init() {
    ArnoldPatch::init();

	// Cleans hooks to old callbacks
	SimulationAnimationPatch::init();
}
    
void ArnoldAnimationPatch::update(set<Device *> devices) {
	SimulationAnimationPatch::IsUpdateRequiredFunction updateRequired =
		std::bind(&SimulationPatch::isUpdateRequired, (SimulationPatch*)this, std::placeholders::_1);
	SimulationAnimationPatch::InterruptFunction interrupt =
		std::bind(&SimulationPatch::interruptRender, (SimulationPatch*)this);
	SimulationAnimationPatch::ClearUpdateFlagsFunction clearUpdateFlags =
		std::bind(&SimulationPatch::clearUpdateFlags, (SimulationPatch*)this);
	SimulationAnimationPatch::update(devices, updateRequired, interrupt, clearUpdateFlags);
}

void ArnoldAnimationPatch::rerender() {
	// Sets all rerendering flags to true to preserve the first frame
	if (m_lights.size() > 0)
		for (auto light : m_lights)
			m_lights[light.first]->rerender_req = true;

	// Use this threshold to avoid deadlock caused by worry device settings
	chrono::time_point<chrono::system_clock> start = chrono::system_clock::now();

	// When patch is working, makes sure the request be processed
	bool flag = true;
	while (flag && m_mode != SimulationAnimationMode::STOPPED) {
		if (m_lights.size() > 0)
			for (auto light : m_lights) {
				flag &= m_lights[light.first]->rerender_req;
				if (!flag)
					return;
			}
		else
			return;

		chrono::time_point<chrono::system_clock> current = chrono::system_clock::now();
		if (chrono::duration_cast<chrono::seconds>(current - start).count() > 1)
			return;
	}
}

void ArnoldAnimationPatch::interruptRender() {
	if (m_mode == SimulationAnimationMode::RECORDING ||
		m_mode == SimulationAnimationMode::INTERACTIVE)
		ArnoldPatch::interruptRender();
	else if (m_mode == SimulationAnimationMode::RENDERING) {
		ArnoldPatch::interruptRender();

		// Clear queue (turn rendering tasks into interactive tasks)
		m_queue.lock();
		for (auto i = m_queuedFrameDeviceInfo.begin();
			i != m_queuedFrameDeviceInfo.end(); i++) {
			i->mode = SimulationAnimationMode::INTERACTIVE;
		}
		m_queue.unlock();
	}
}

void ArnoldAnimationPatch::close() {
	SimulationAnimationPatch::close();

    // Close arnold interface
    ArnoldPatch::close();
}

void ArnoldAnimationPatch::setPreviewSamples(int preview) {
    m_preview_samples = preview;
    if (m_mode == SimulationAnimationMode::INTERACTIVE)
        setSamples(m_preview_samples);
}
    
void ArnoldAnimationPatch::setRenderSamples(int render) {
    m_render_samples = render;
}
  
float ArnoldAnimationPatch::getPercentage() const {
  // Rough number.
	if (m_mode == SimulationAnimationMode::RENDERING) {
		// Don't forget the frame being processed
		size_t finished = m_mem_frameManager->getFrameNum();
		size_t sum = finished + m_queuedFrameDeviceInfo.size() + 1;
		float renderingPer = m_interface.getPercentage();
		sum = (sum == 0) ? 1 : sum;

		return ((float)finished * 100.f + renderingPer) / sum;
	}

  return ArnoldPatch::getPercentage();
}
    
void ArnoldAnimationPatch::reset() {
	SimulationAnimationPatch::InterruptFunction interrupt =
		std::bind(&SimulationPatch::interruptRender, (SimulationPatch*)this);
	SimulationAnimationPatch::reset(interrupt);
}

void ArnoldAnimationPatch::disableContinuousRenderMode() {
  m_mode = SimulationAnimationMode::STOPPED;
}

void ArnoldAnimationPatch::enableContinuousRenderMode() {
  m_mode = SimulationAnimationMode::INTERACTIVE;
}

void ArnoldAnimationPatch::renderSingleFrame(const set<Device*>& devices, string basepath, string filename) {
  if (m_mode != SimulationAnimationMode::STOPPED)
    disableContinuousRenderMode();

  FrameDeviceInfo frame;

  // Fulls time and mode for frame info.
  createFrameInfoHeader(frame);
  createFrameInfoBody(devices, frame);

  std::stringstream ss;
  ss << "Rendering single frame: " << frame.time;
  Logger::log(LDEBUG, ss.str());

  // Interrupts the current rendering if in the interactive mode.
  if (m_mode == SimulationAnimationMode::INTERACTIVE ||
    m_mode == SimulationAnimationMode::RECORDING) {
    interruptRender();
  }

  // Render immediately.
  if (!m_interface.isOpen())
    m_interface.init();

  m_interface.setDriverFileName(basepath, filename);

  updateLight(frame.devices);
  bool success = ArnoldPatch::renderLoop();

  if (success) {
    unsigned char *bytes = new unsigned char[getWidth() * getHeight() * 4];
    floats_to_bytes(bytes, getBufferPointer(), getWidth(), getHeight());

    string file = basepath + "/" + filename + ".png";

    if (!imageio_save_image(file.c_str(), bytes, getWidth(), getHeight())) {
      std::stringstream err_ss;
      err_ss << "Error to write png: " << filename;
      Logger::log(ERR, err_ss.str());
    }
  }
}

void ArnoldAnimationPatch::renderSingleFrameToBuffer(const set<Device*>& devices, unsigned char* buff) {
  if (m_mode != SimulationAnimationMode::STOPPED)
    disableContinuousRenderMode();

  FrameDeviceInfo frame;

  // Fulls time and mode for frame info.
  createFrameInfoHeader(frame);
  createFrameInfoBody(devices, frame, true);

  std::stringstream ss;
  ss << "Rendering single frame: " << frame.time;
  Logger::log(LDEBUG, ss.str());

  // Interrupts the current rendering if in the interactive mode.
  if (m_mode == SimulationAnimationMode::INTERACTIVE ||
    m_mode == SimulationAnimationMode::RECORDING) {
    interruptRender();
  }

  // Render immediately.
  if (!m_interface.isOpen())
    m_interface.init();

  updateLight(frame.devices);
  bool success = ArnoldPatch::renderLoop();
  frame.clear();

  if (success) {
    auto bp = getBufferPointer();
    for (int j = 0; j < getHeight(); j++) {
      for (int i = 0; i < getWidth(); i++) {
        int offset = (j * getWidth() + i) * 4;

        // convert to bytes
        buff[offset] = static_cast<unsigned char>(bp[offset + 2] * 0xff);
        buff[offset + 1] = static_cast<unsigned char>(bp[offset + 1] * 0xff);
        buff[offset + 2] = static_cast<unsigned char>(bp[offset + 0] * 0xff);
        buff[offset + 3] = static_cast<unsigned char>(bp[offset + 3] * 0xff);
      }
    }
  }
}

void ArnoldAnimationPatch::getPositionFromAss(const set<Device*>& devices)
{
  // by default in y-up coordinate systems, the light faces -z to start with.
  // The procedure here will be to grab the translation component of the arnold matrix
  // (which is actually just the position of the light in 3-d space) and set the
  // look at point to origin - (lookAtDir.normalized()) and the distance to 1.

  for (auto d : devices) {
    // bit of a hack, but don't touch quad light positions, they seem a bit odd
    if (d->getType() == "quad_light")
      continue;

    string nodeName = d->getMetadata("Arnold Node Name");
    AtNode* light = AiNodeLookUpByName(nodeName.c_str());
    
    if (light != nullptr) {
      // Get the matrix
      AtMatrix m;
      AiNodeGetMatrix(light, "matrix", m);

      // Get the light location
      Eigen::Vector3d origin(m[3][0], m[3][1], m[3][2]);

      // Get the rotation matrix
      Eigen::Matrix3d rot;
      rot(0, 0) = m[0][0];
      rot(0, 1) = m[0][1];
      rot(0, 2) = m[0][2];
      rot(1, 0) = m[1][0];
      rot(1, 1) = m[1][1];
      rot(1, 2) = m[1][2];
      rot(2, 0) = m[2][0];
      rot(2, 1) = m[2][1];
      rot(2, 2) = m[2][2];

      // rotate -z to get direction
      Eigen::Vector3d z(0, 0, -1);

      Eigen::Vector3d dir = z.transpose() * rot;
      Eigen::Vector3d look = origin + dir.normalized();

      // calculate spherical coords
      Eigen::Vector3d relPos = origin - look;
      double polar = acos(relPos(1) / relPos.norm()) * (180.0 / M_PI);
      double azimuth = atan2(relPos(2), relPos(0)) * (180 / M_PI);

      // Set params, and lock them to the values found in the file.
      // If the params don't exist, just create them
      if (!d->paramExists("distance"))
        d->setParam("distance", new LumiverseFloat());
      if (!d->paramExists("polar"))
        d->setParam("polar", new LumiverseOrientation());
      if (!d->paramExists("azimuth"))
        d->setParam("azimuth", new LumiverseOrientation());
      if (!d->paramExists("lookAtX"))
        d->setParam("lookAtX", new LumiverseFloat());
      if (!d->paramExists("lookAtY"))
        d->setParam("lookAtY", new LumiverseFloat());
      if (!d->paramExists("lookAtZ"))
        d->setParam("lookAtZ", new LumiverseFloat());

      d->getParam<LumiverseFloat>("distance")->setVals(1, 1, 1, 1);
      d->getParam<LumiverseOrientation>("polar")->setVals(polar, polar, polar, polar);
      d->getParam<LumiverseOrientation>("azimuth")->setVals(azimuth, azimuth, azimuth, azimuth);
      d->getParam<LumiverseFloat>("lookAtX")->setVals(look(0), look(0), look(0), look(0));
      d->getParam<LumiverseFloat>("lookAtY")->setVals(look(1), look(1), look(1), look(1));
      d->getParam<LumiverseFloat>("lookAtZ")->setVals(look(2), look(2), look(2), look(2));
    }
  }

}

void ArnoldAnimationPatch::getBeamPropsFromAss(const set<Device*>& devices)
{
  for (auto d : devices) {
    string nodeName = d->getMetadata("Arnold Node Name");
    AtNode* light = AiNodeLookUpByName(nodeName.c_str());

    if (light != nullptr) {
      AtRGB color = AiNodeGetRGB(light, "color");
      float intens = AiNodeGetFlt(light, "intensity");
      float pangle = AiNodeGetFlt(light, "penumbra_angle");

      if (!d->paramExists("intensity"))
        d->setParam("intensity", new LumiverseFloat(0, 0, 100000, 0));
      if (!d->paramExists("penumbraAngle"))
        d->setParam("penumbraAngle", new LumiverseFloat(0, 0, 20, -20));
      if (!d->paramExists("color"))
        d->setParam("color", new LumiverseColor(Lumiverse::BASIC_RGB));

      d->getColor()->setRGB(color.r, color.g, color.b);
      d->setIntensity(intens);
      d->getParam<LumiverseFloat>("penumbraAngle")->setVal(pangle);
    }
  }
}

void ArnoldAnimationPatch::createFrameInfoBody(set<Device *> devices, FrameDeviceInfo &frame, bool forceUpdate) {
	for (Device *d : devices) {
		// Checks if the device is connect to this patch
		if (forceUpdate || (m_lights.count(d->getMetadata("Arnold Node Name")) > 0 &&
			m_lights[d->getMetadata("Arnold Node Name")]->rerender_req)) {
			// Makes copy of this device
			Device *d_copy = new Device(*d);
			frame.devices.insert(d_copy);
		}
	}
}

//============================ Worker Code =========================

void ArnoldAnimationPatch::workerRender(FrameDeviceInfo frame) {
	std::stringstream ss;
	ss << "Received new frame: " << frame.time << "(" << frame.mode << ")";
	Logger::log(LDEBUG, ss.str());

	updateLight(frame.devices);
	bool success = ArnoldPatch::renderLoop();

	// Dumps only when the image was rendered successfully for rendering.
	// If the worker was reset while rendering, doesn't dump.
	if (success && frame.mode == SimulationAnimationMode::RENDERING) {
		m_mem_frameManager->dump(frame.time, getBufferPointer(),
			getWidth(), getHeight());
		if (m_file_frameManager)
			m_file_frameManager->dump(frame.time, getBufferPointer(),
			getWidth(), getHeight());
	}
}

}
