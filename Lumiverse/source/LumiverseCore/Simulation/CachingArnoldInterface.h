/*! \file CachingArnoldInterface.h
* \brief An interface for serving cached Arnold render responses.
*
* This interface renders a separate frame buffer for each device in the scene,
* and caches the responding frame buffer so that it can be returned quickly
* on future requests. The cached request can be served quickly even if the
* request changes a lighting intensity, turns off devices, etc because the 
* rendering equation is linear in the devices.
*/
#ifndef _Arnold_CACHE_INTERFACE_H_
#define _Arnold_CACHE_INTERFACE_H_

#pragma once

#include "LumiverseCoreConfig.h"

#ifdef USE_ARNOLD

#ifdef USE_ARNOLD_CACHING

#include "ArnoldInterface.h"
#include <ai.h>
#include "ArnoldParameterVector.h"
#include "Compositor.h"
#include "ToneMapper.h"
#include "EXRLayer.h"
#include <thread>
#include <algorithm>
#include <unordered_map>

namespace Lumiverse {
	struct CachedDevice {
		Device device;
		float *buffer;
	};

	class CachingArnoldInterface : public ArnoldInterface
	{
	public:
		CachingArnoldInterface() : ArnoldInterface() {}

		/*!
		* \brief Initialize the interface by building the EXR layer basis.
		* This can take quite a long time depending on how many light sources are
		* in a scene and the sampling rate used when rendering.
		*/
		void init();

		/*!
		* \brief Now that all of the EXR layers have been rendered (i.e. the cache has been filled),
		* render an image per the light node parameters.
		*/
		int render(const std::set<Device *> &devices);

		/*!
		* \brief Set the HDR output buffer
		*/
		void setHDROutputBuffer();

		/*!
		* \brief Dump to the HDR buffer
		*/
		void dumpHDRToBuffer();

	private:

		Compositor compositor;

		ToneMapper tone_mapper;

		/*!
		* Compositor output. (internal)
		*/
		Pixel4 *compositor_output;

		/*!
		* \brief Load layers from an EXR file
		*/
		int load_exr(const char *file_path);

		void updateDevicesLayers(const std::set<Device *> &devices);

		/*!
		* \brief A map from a device name to the device. This is used
		* to determine when a device has changed and needs to be updated
		* in the cache on a render call.
		*/
		std::unordered_map<std::string, Device*> cached_devices;
	};
}

#endif // USE_ARNOLD_CACHING

#endif // USE_ARNOLD

#endif // _Arnold_CACHE_INTERFACE_H_