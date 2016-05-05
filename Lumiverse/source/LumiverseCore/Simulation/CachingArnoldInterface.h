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
#include <set>

namespace Lumiverse {

	class CachingArnoldInterface : public virtual ArnoldInterface
	{
	public:
    CachingArnoldInterface();

		/*!
		* \brief Initialize the interface by building the EXR layer basis.
		* This can take quite a long time depending on how many light sources are
		* in a scene and the sampling rate used when rendering.
		*/
		void init();

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
		* \brief Dump to the HDR buffer
		*/
		virtual void dumpHDRToBuffer(const std::set<Device *> &devices);

		/*!
		\brief Override set dims so that we know if we should force a re-loading of the cache
		*/
		bool setDims(int w, int h) override;

    /*!
    \brief Changes the size of the cache and forces a reload.
    */
    void setCacheDims(int w, int h);

		/*!
		* \brief Set a new sample rate and force a reload of the cache if needed.
    *
    * A reload is required if the new AA sample rate is higher than the current cached
    * sample rate.
		*/
		void setSamples(int samples) override;

    /*!
    \brief Sets the location the cache saves and loads exr files from
    */
    void setPath(string path) { _cache_file_path = path; }

		/*!
		\brief Sets a parameter found in the global options node in arnold
		*/
		void setOptionParameter(const std::string &paramName, int val) override;
		void setOptionParameter(const std::string &paramName, float val) override;

    /*!
    \brief Saves cache layers to exr files for reuse.
    */
    void dumpCache();

    /*!
    \brief If the interface is caching it should use this function to perform any
    preload operations it may be doing.
    */
    virtual void loadIfUsingCaching(const set<Device*>& devices);

	protected:

		const static int DEFAULT_WIDTH = 1920;
		const static int DEFAULT_HEIGHT = 980;

		Compositor compositor;

		ToneMapper tone_mapper;

		/*!
		* Compositor output. (internal)
		*/
		Pixel4 *compositor_output;

		/*!
		* \brief Load layers from an EXR file
		*/
		int load_exr(string& filename);

		/*!
		\brief Update each lighting device's basis layer if necessary
		*/
		virtual void updateDevicesLayers(const std::set<Device *> &devices);

		/*!
		\brief Get a list of devices that need to be updated / re-rendered
		*/
		const std::unordered_map<std::string, Device*> getDevicesToUpdate(const std::set<Device *> &devices);

		/*!
		* \brief A map from a device name to the device. This is used
		* to determine when a device has changed and needs to be updated
		* in the cache on a render call.
		*/
		std::unordered_map<std::string, Device*> cached_devices;
		
		/*!
		\brief Should we force an update on the next render call?
		*/
		bool force_cache_reload = false;
	
		/*!
		* \brief Buffer that holds temporary rendered results from each
		* lighting node when the cache is being filled.
		*/
		float *m_render_buffer = NULL;

    /*!
    \brief Cache frame width
    */
    int _cache_width;

    /*! \brief Cache frame height */
    int _cache_height;

    /*! \brief Cache sample rate. */
    int _cache_aa_samples;

    /*! \brief Sets the path where cache files will be written to. */
    string _cache_file_path;

		/*!
		* \brief Check if an option change requires a complete reloading of the cache
		*
		* When an option is changed with setParam, check if the entire cache needs to
		* be reloaded. This should likely only happen if an image is upsampled
		*/
		bool optionRequiresCacheReload(const std::string &paramName);

		/*
		* \brief Test whether a device parameter has changed such that its cache
		* basis image should be re-rendered. This can happen on updates to params
		* such as position, rotation, penumbra angle, and distance.
		*/
		bool isValidCacheCopy(Device *cached_device, Device *other_device);

		/*!
		* \brief Set the HDR output buffer
		*/
		virtual void setHDROutputBuffer();

    /*!
    \brief Saves a layer to a file
    */
    void saveLayer(EXRLayer* l);

    /*!
    \brief Loads cache layers from exr files. Assumes cache is up to date after load.
    */
    void loadCache(const set<Device*>& devices);
	};
}

#endif // USE_ARNOLD_CACHING

#endif // USE_ARNOLD

#endif // _Arnold_CACHE_INTERFACE_H_