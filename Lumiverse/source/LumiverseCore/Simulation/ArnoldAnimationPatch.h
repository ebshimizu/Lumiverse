/*! \file ArnoldAnimationPatch.h
* \brief Subclass of ArnoldPatch to render frames of an animation.
*/
#ifndef _ArnoldAnimationPATCH_H_
#define _ArnoldAnimationPATCH_H_

#pragma once

#include "LumiverseCoreConfig.h"
#ifdef USE_ARNOLD

#include "../lib/libjson/libjson.h"
#include "ArnoldPatch.h"
#include "ArnoldMemoryFrameManager.h"

#include <thread>
#include <chrono>
#include <iostream>

namespace Lumiverse {
	/*! \brief Four working modes of ArnoldAnimationPatch. 
	* INTERACTIVE: Patch renders the latest sent frame with preview sampling rate (camera).
	* RECORDING: Patch renders the preview scene with similar behavior as INTERACTIVE,
	* and it also creates a duplicate for each frame, which is used later for rendering.
	* RENDERING: Renders the frames with rendering sampling rate.
	* STOPPED: Patch doesn't respond to any input. This state is usually used for playing video.
	*/
  enum ArnoldAnimationMode {
      INTERACTIVE, RECORDING, RENDERING, STOPPED
  };
    
  /*! \brief The state info for worker thread. */
  struct FrameDeviceInfo {
      // The time point of this frame.
      // It's actually the duration counted from the update is first time
      // called.
      time_t time;
      // Copies for devices connected to this patch.
      std::set<Device *> devices;
      ArnoldAnimationMode mode;

      /*! \brief Constructor. */
      FrameDeviceInfo() : time(-1), mode(ArnoldAnimationMode::INTERACTIVE) { }

      /*! \brief Releases the copies for devices. */
      void clear() {
		  time = -1;

		  for (Device *d : devices) {
			  if (d != NULL)
			  delete d;
			  d = NULL;
		  }
		  devices.clear();
      }
      
	  /*! \brief Deep copy. */
      void copyByValue(const FrameDeviceInfo &other) {
          time = other.time;
          mode = other.mode;
          for (Device *d : other.devices) {
              devices.insert(new Device(d));
          }
      }
  };
    
  /*!
  * \brief A subclass of ArnoldPatch. 
  * Instead of interrupting the worker thread every time a new rendering 
  * task is received, this class keeps all requests in a queue. A worker
  * thread grasps tasks and dumps frame buffer to a ArnoldFrameManager.
  *  
  * \sa ArnoldPatch, ArnoldFrameManager
  */
  class ArnoldAnimationPatch : public ArnoldPatch
  {
  public:
    /*!
    * \brief Constructs a ArnoldAnimationPatch object.
    */
    ArnoldAnimationPatch() : m_worker(NULL), 
	  m_startPoint(std::chrono::system_clock::from_time_t(0)),
	  m_frameManager(NULL), m_mode(ArnoldAnimationMode::STOPPED),
      m_preview_samples(m_interface.getSamples()),
      m_render_samples(m_interface.getSamples()) { }

    /*!
    * \brief Constructs ArnoldPatch from JSON data.
    *
    * \param data JSONNode containing the ArnoldAnimationPatch object data.
    */
    ArnoldAnimationPatch(const JSONNode data);

    /*!
    * \brief Destroys the object.
    */
    virtual ~ArnoldAnimationPatch();

    /*!
    * \brief Initializes Arnold with function of its parent class and
    * starts a worker thread.
    */
    virtual void init() override;

    /*!
    * \brief Starts recording.
    * Main thread starts to send frame labeled as RECORDING info to worker.
    */
    void startRecording() { m_frameManager->clear(); m_mode = ArnoldAnimationMode::RECORDING; }
    
	/*!
	* \brief Ends recording.
	* Main thread stops to send frame labeled as RECORDING info to worker.
	* It starts to send INTERACTIVE frame instead.
	*/
    void endRecording();
      
    /*!
    * \brief Starts interactive mode.
    * Worker thread can get interrupted. It always takes the most fresh info.
    */
    void startInteractive() { m_mode = ArnoldAnimationMode::INTERACTIVE; }
      
	/*!
	* \brief Returns the mode/state in which the patch is.
	*
	* \return The mode/state.
	*/
    ArnoldAnimationMode getMode() { return m_mode; }
      
    /*!
    * \brief Gets the type of this object.
    *
    * \return String containing "ArnoldAnimationPatch"
    */
    virtual string getType() { return "ArnoldAnimationPatch"; }

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

    /*!
    * \brief Returns the ArnoldFrameManager to reconstruction the 
    * animation.
    *
    * \return The ArnoldFrameManager object containning all the frame
    * and their corresponding time point.
    */
    ArnoldFrameManager *getFrameManager() const;
    
    /*!
    * \brief Resets the object to its initial state.
    *
    * Including resetting start point, clearing frame manager, interrupting worker and clearing worker's queue.
    */
    void reset();
      
	/*!
	* \brief Stops the patch.
	*
	* The main thread would stop responding to new requests and the worker thread would be joined.
	*/
    void stop();

	/*!
	* \brief Sets the camera sampling rate for preview.
	*
	* Although it's possible to set the rate to a large number. It's not recommended.
	* \param preview The camera sampling rate for preview.
	*/
    void setPreviewSamples(int preview);

	/*!
	* \brief Sets the camera sampling rate for rendering.
	*
	* Although it's possible to set the rate to a small number. It's not recommended.
	* \param render The camera sampling rate for rendering.
	*/
    void setRenderSamples(int render);

	/*!
	* \brief Returns the camera sampling rate for preview.
	*/
    int getPreviewSamples() { return m_preview_samples; }

	/*!
	* \brief Returns the camera sampling rate for render.
	*/
    int getRenderSamples() { return m_render_samples; }
      
    // Callbacks
    typedef function<void()> FinishedCallbackFunction;
      
    /*!
    * \brief Registers a callback function for parameter changed event.
    *
    * All registered functinos would be called when a parameter is changed
    * by Device::setParam and Device::reset function.
    * \param func The callback function.
    * \return The int id for the registered function.
    * \sa addMetadataChangedCallback(DeviceCallbackFunction func)
    */
    int addFinishedCallback(FinishedCallbackFunction func);
      
    /*!
    * \brief Deletes a registered callback for parameter change
    *
    * \param id The id returned when the callback is registered
    * \sa addParameterChangedCallback(DeviceCallbackFunction func)
    */
    void deleteFinishedCallback(int id);
      
	/*!
	* \brief Gets the current rendering progress as percentage.
	*
	* The value returned may not be the accurate number due to concurrency.
	* Also the accuracy is limited to number of frame.
	* \return The current rendering progress as percentage
	*/
    virtual float getPercentage() const override;

  private:
    /*!
    * \brief Worker loop.
    *
    * Dequeues a new task. Sets the light parameters and renders. Dumps
    * the frame buffer. The loop ends when an end info is received.
    */
    void workerLoop();
      
	/*!
	* \brief Helper to call all the registered callbacks for rendering finished event.
	*/
    void onWorkerFinished();

    // The worker thread.
    std::thread *m_worker;

    // The lock for the task queue.
    std::mutex m_queue;

    // The task queue.
    std::vector<FrameDeviceInfo> m_queuedFrameDeviceInfo;

    // The start point for time points in FrameDeviceInfo.
    // It's the moment when update function is called for the first
    // time.
    std::chrono::time_point<std::chrono::system_clock> m_startPoint;

    // The ArnoldFrameManager object. Used to store frame buffers.
    ArnoldFrameManager *m_frameManager;
      
    /*! \brief Indicates the mode of ArnoldAnimationPatch.
    */
    ArnoldAnimationMode m_mode;
      
	/*! \brief The camera sampling rate for preview.
	*/
    int m_preview_samples;

	/*! \brief The camera sampling rate for rendering.
	*/
    int m_render_samples;
      
	/*! \brief The list for callback functions.
	*/
    map<int, FinishedCallbackFunction> m_onFinishedFunctions;
  };
    
}

#endif

#endif
