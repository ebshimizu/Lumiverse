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
		int render();

		/*!
		* \brief Set the HDR output buffer
		*/
		void setHDROutputBuffer(Pixel4 *buffer);

		/*!
		* \brief Dump to the HDR buffer
		*/
		void dumpHDRToBuffer();

	private:

		Compositor compositor;

		ToneMapper tone_mapper;

		/**
		* Compositor output. (internal)
		*/
		Pixel4 *compositor_output;

		/**
		* Buffer to write illuminance pixel output of the compositor.
		*/
		Pixel4 *hdr_output_buffer;

		int load_exr(const char *file_path);
	};
}

#endif

#endif // _Arnold_CACHE_INTERFACE_H_