/*! \file SimulationAnimationPatch.h
* \brief Subclass of ArnoldPatch to render frames of an animation.
*/
#ifndef _SimulationAnimationPATCH_H_
#define _SimulationAnimationPATCH_H_

#pragma once

#include "LumiverseCoreConfig.h"
#ifdef USE_ARNOLD

#include "../lib/libjson/libjson.h"
#include "SimulationPatch.h"
#include "ArnoldMemoryFrameManager.h"
#include "ArnoldFileFrameManager.h"

#include <thread>
#include <chrono>
#include <iostream>

namespace Lumiverse {
	/*! \brief Four working modes of SimulationAnimationPatch. 
	* INTERACTIVE: Patch renders the latest sent frame with preview sampling rate (camera).
	* RECORDING: Patch renders the preview scene with similar behavior as INTERACTIVE,
	* and it also creates a duplicate for each frame, which is used later for rendering.
	* RENDERING: Renders the frames with rendering sampling rate.
	* STOPPED: Patch doesn't respond to any input. This state is usually used for playing video.
	*/
  enum SimulationAnimationMode {
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
      SimulationAnimationMode mode;

      /*! \brief Constructor. */
      FrameDeviceInfo() : time(-1), mode(SimulationAnimationMode::RENDERING) { }

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
  class SimulationAnimationPatch
  {
  public:
    /*!
    * \brief Constructs a SimulationAnimationPatch object.
    */
    SimulationAnimationPatch() : m_worker(NULL), 
	  m_startPoint(std::chrono::system_clock::from_time_t(0)),
	  m_mem_frameManager(NULL), m_file_frameManager(NULL), 
	  m_mode(SimulationAnimationMode::STOPPED) { }

    /*!
    * \brief Constructs ArnoldPatch from JSON data.
    *
    * \param data JSONNode containing the SimulationAnimationPatch object data.
    */
    SimulationAnimationPatch(const JSONNode data);

    /*!
    * \brief Destroys the object.
    */
    virtual ~SimulationAnimationPatch();

	// Callbacks
	typedef function<void()> FinishedCallbackFunction;
	typedef function<bool(set<Device *>)> IsUpdateRequiredFunction;
	typedef function<void(FrameDeviceInfo&)> CreateFrameInfoBodyFunction;
	typedef function<void()> InterruptFunction;
	typedef function<void()> ClearUpdateFlagsFunction;

    /*!
    * \brief Initializes Arnold with function of its parent class and
    * starts a worker thread.
    */
    void init();

    /*!
    * \brief Starts recording.
    * Main thread starts to send frame labeled as RECORDING info to worker.
    */
    virtual void startRecording() { 
		m_mem_frameManager->clear();

		// Overwrite existing frames instead of clearing all
		if (m_file_frameManager)
			m_file_frameManager->reset();

		m_mode = SimulationAnimationMode::RECORDING; 
	}
    
	/*!
	* \brief Ends recording.
	* Main thread stops to send frame labeled as RECORDING info to worker.
	* It starts to send INTERACTIVE frame instead.
	*/
    virtual void endRecording();
      
    /*!
    * \brief Starts interactive mode.
    * Worker thread can get interrupted. It always takes the most fresh info.
    */
	virtual void startInteractive() { m_mode = SimulationAnimationMode::INTERACTIVE; }
      
	/*!
	* \brief Returns the mode/state in which the patch is.
	*
	* \return The mode/state.
	*/
    virtual SimulationAnimationMode getMode() { return m_mode; }
      
    /*!
    * \brief Gets the type of this object.
    *
    * \return String containing "SimulationAnimationPatch"
    */
    virtual string getType() { return "SimulationAnimationPatch"; }

    /*!
    * \brief Updates the rendering queue given the list of devices
    * in the rig.
    *
    * Before enqueuing the rendering info, main thread checks if
    * there is any parameter or metadata changed during last update
    * interval. It only adds a new request when it's truly necessary.
    */
	void update(set<Device *> devices, IsUpdateRequiredFunction isUpdateRequired,
		InterruptFunction interruptRender,
		ClearUpdateFlagsFunction clearUpdateFlags);

    /*!
    * \brief Waits for the worker thread and closes the Arnold session.
    *
    * The main thread sends a special frame info at the beginning of
    * this function. (a info with devices list only containning a NULL)
    * Then the thread would wait to join the worker thread. After all
    * there are done, closes the arnold session as the parent class.
    */
	void close();

    /*!
    * \brief Returns the ArnoldFrameManager to reconstruction the 
    * animation.
    *
    * \return The ArnoldFrameManager object containning all the frame
    * and their corresponding time point.
    */
    virtual ArnoldFrameManager *getFrameManager() const;
    
    /*!
    * \brief Resets the object to its initial state.
    *
    * Including resetting start point, clearing frame manager, interrupting worker and clearing worker's queue.
    */
	virtual void reset(InterruptFunction interruptRender);
      
	virtual void reset() = 0;

	/*!
	* \brief Stops the patch.
	*
	* The main thread would stop responding to new requests and the worker thread would be joined.
	*/
	virtual void stop();
      
    /*!
    * \brief Registers a callback function for parameter changed event.
    *
    * All registered functinos would be called when a parameter is changed
    * by Device::setParam and Device::reset function.
    * \param func The callback function.
    * \return The int id for the registered function.
    * \sa addMetadataChangedCallback(DeviceCallbackFunction func)
    */
    virtual int addFinishedCallback(FinishedCallbackFunction func);
      
    /*!
    * \brief Deletes a registered callback for parameter change
    *
    * \param id The id returned when the callback is registered
    * \sa addParameterChangedCallback(DeviceCallbackFunction func)
    */
	virtual void deleteFinishedCallback(int id);

	virtual float *getBufferPointer() = 0;

	virtual void rerender() = 0;
	
	virtual void interruptRender() = 0;

	virtual int getWidth() = 0;
	virtual int getHeight() = 0;

	virtual void setPreviewSamples(int preview) = 0;

	virtual void setRenderSamples(int render) = 0;

	virtual int getPreviewSamples() = 0;

	virtual int getRenderSamples() = 0;

  protected:

	/*!
	* \brief Loads data from a parsed JSON object
	* \param data JSON data to load
	*/
	void loadJSON(const JSONNode data);

	/*!
	* \brief Worker loop.
	*
	* Dequeues a new task. Sets the light parameters and renders. Dumps
	* the frame buffer. The loop ends when an end info is received.
	*/
	virtual void workerLoop();

	/*!
	* \brief Helper to call all the registered callbacks for rendering finished event.
	*/
	virtual  void onWorkerFinished();

	virtual void onRecording() { }

	virtual void onRendering() { }

	virtual void workerRender(FrameDeviceInfo frame) = 0;

	virtual void createFrameInfoHeader(FrameDeviceInfo &frame);

	virtual void createFrameInfoBody(set<Device *> devices, FrameDeviceInfo &frame) = 0;

	virtual void enqueueFrameInfo(const FrameDeviceInfo &frame);


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
    ArnoldMemoryFrameManager *m_mem_frameManager;
	ArnoldFileFrameManager *m_file_frameManager;
      
    /*! \brief Indicates the mode of SimulationAnimationPatch.
    */
    SimulationAnimationMode m_mode;
      
	/*! \brief The list for callback functions.
	*/
    map<int, FinishedCallbackFunction> m_onFinishedFunctions;
  };
    
}

#endif

#endif
