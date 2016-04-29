/*! \file DistributedCachingArnoldInterface.h
* \brief An interface for serving distributed cached Arnold render responses.
*
* This interface functions exactly like the CachingArnoldInterface, except that it
* distributes out render requests to 
*/
#ifndef _Arnold_DISTRIBUTED_CACHE_INTERFACE_H_
#define _Arnold_DISTRIBUTED_CACHE_INTERFACE_H_

#pragma once

#include "LumiverseCoreConfig.h"

#ifdef USE_ARNOLD

#ifdef USE_DUMIVERSE

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
#include <set>
#include "CachingArnoldInterface.h"
#include "DistributedArnoldInterface.h"

namespace Lumiverse {

	class DistributedCachingArnoldInterface : public DistributedArnoldInterface, public CachingArnoldInterface
	{
	public:
		DistributedCachingArnoldInterface(string host, int port, string outputPath) : DistributedArnoldInterface(host, port, outputPath) {}

		/*!
		* \brief Initialize the interface by building the EXR layer basis.
		* This can take quite a long time depending on how many light sources are
		* in a scene and the sampling rate used when rendering.
		*/
		void init(const JSONNode jsonPatch) override;

		/*!
		\brief Close this caching interface
		*/
		void close() override;

		/*!
		* \brief Now that all of the EXR layers have been rendered (i.e. the cache has been filled),
		* render an image per the light node parameters.
		*/
		int render(const std::set<Device *> &devices);

		/*!
		\brief Override set dims so that we know if we should force a re-loading of the cache
		*/
		bool setDims(int w, int h) override;

		/*!
		\brief Set a new sample rate and force a reload of the cache
		*
		* Set a new sample rate and force a reload of the cache. We force a reload because
		* the values of the cache are going to be useless because we are likely only ever
		* going to upsample.
		*/
		void setSamples(int samples) override;

		/*!
		\brief Sets a parameter found in the global options node in arnold
		*/
		void setOptionParameter(const std::string &paramName, int val) override;
		void setOptionParameter(const std::string &paramName, float val) override;

		float *getBufferPointer() override;

	protected:
		void dumpHDRToBuffer(const std::set<Device *> &devices) override;
		void updateDevicesLayers(const std::set<Device *> &devices) override;
		void setHDROutputBuffer() override;
	};
}

#endif // USE_ARNOLD_CACHING

#endif // USE_DUMIVERSE

#endif // USE_ARNOLD

#endif // _Arnold_CACHE_INTERFACE_H_