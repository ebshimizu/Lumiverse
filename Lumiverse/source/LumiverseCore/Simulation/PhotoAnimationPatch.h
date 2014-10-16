/*! \file PhotoAnimationPatch.h
* \brief Subclass of ArnoldPatch to render frames of an animation.
*/
#ifndef _PhotoAnimationPatch_H_
#define _PhotoAnimationPatch_H_

#pragma once

#include "LumiverseCoreConfig.h"
#ifdef USE_ARNOLD

#include "../lib/libjson/libjson.h"
#include "PhotoPatch.h"
#include "SimulationAnimationPatch.h"
#include "ArnoldMemoryFrameManager.h"
#include "ArnoldFileFrameManager.h"

#include <thread>
#include <chrono>
#include <iostream>

namespace Lumiverse {
    
  /*!
  * \brief A subclass of ArnoldPatch. 
  * Instead of interrupting the worker thread every time a new rendering 
  * task is received, this class keeps all requests in a queue. A worker
  * thread grasps tasks and dumps frame buffer to a ArnoldFrameManager.
  *  
  * \sa ArnoldPatch, ArnoldFrameManager
  */
  class PhotoAnimationPatch : public SimulationAnimationPatch,
							  public PhotoPatch
  {
  public:
    /*!
    * \brief Constructs a PhotoAnimationPatch object.
    */
    PhotoAnimationPatch() { }

    /*!
    * \brief Constructs ArnoldPatch from JSON data.
    *
    * \param data JSONNode containing the PhotoAnimationPatch object data.
    */
    PhotoAnimationPatch(const JSONNode data);

    /*!
    * \brief Destroys the object.
    */
    virtual ~PhotoAnimationPatch();

    /*!
    * \brief Gets the type of this object.
    *
    * \return String containing "PhotoAnimationPatch"
    */
	virtual string getType() override { return "PhotoAnimationPatch"; }

	/*!
	* \brief Initializes Arnold with function of its parent class and
	* starts a worker thread.
	*/
	virtual void init() override;

	/*!
	* \brief Updates the rendering queue given the list of devices
	* in the rig.
	*
	* Before enqueuing the rendering info, main thread checks if
	* there is any parameter or metadata changed during last update
	* interval. It only adds a new request when it's truly necessary.
	*/
	virtual void update(set<Device *> devices) override;

	/*!
	* \brief Waits for the worker thread and closes the Arnold session.
	*
	* The main thread sends a special frame info at the beginning of
	* this function. (a info with devices list only containning a NULL)
	* Then the thread would wait to join the worker thread. After all
	* there are done, closes the arnold session as the parent class.
	*/
	virtual void close() override;

	virtual void reset();

  protected:

	virtual void workerRender(FrameDeviceInfo frame) override;
	virtual void createFrameInfoBody(set<Device *> devices, FrameDeviceInfo &frame);

	/*!
	* \brief Loads data from a parsed JSON object
	* \param data JSON data to load
	*/
	virtual void loadJSON(const JSONNode data) override;

	virtual void onRecording() override { }

	virtual void onRendering() override { }

  };
    
}

#endif

#endif
