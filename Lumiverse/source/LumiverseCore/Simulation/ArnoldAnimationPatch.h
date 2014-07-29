/*! \file ArnoldAnimationPatch.h
* \brief Subclass of ArnoldPatch to render frames of an animation.
*/
#ifndef _ArnoldAnimationPATCH_H_
#define _ArnoldAnimationPATCH_H_

#pragma once

#include "LumiverseCoreConfig.h"

#include "../lib/libjson/libjson.h"
#include "ArnoldPatch.h"
#include "ArnoldMemoryFrameManager.h"

#include <thread>
#include <chrono>
#include <iostream>

namespace Lumiverse {
  /*! \brief The state info for worker thread. */
  struct FrameDeviceInfo {
      // The time point of this frame.
      // It's actually the duration counted from the update is first time
      // called.
      time_t time;
      // Copies for devices connected to this patch.
      std::set<Device *> devices;

      /*! \brief Constructor. */
      FrameDeviceInfo() : time(-1) { }

      /*! \brief Releases the copies for devices. */
      void clear() {
	  time = -1;

	  for (Device *d : devices) {
	      if (d != NULL)
		  delete d;
	  }
	  devices.clear();
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
	  m_frameManager(NULL), m_recording(false) { }

    /*!
    * \brief Construct ArnoldPatch from JSON data.
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
    virtual void init();

    /*!
    * \brief Starts recording.
    * Main thread starts to send frame info to worker.
    */
    void startRecording() { m_recording = true; }
      
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
    virtual void update(set<Device *> devices);

    /*!
    * \brief Waits for the worker thread and closes the Arnold session.
    *
    * The main thread sends a special frame info at the beginning of
    * this function. (a info with devices list only containning a NULL)
    * Then the thread would wait to join the worker thread. After all
    * there are done, closes the arnold session as the parent class.
    */
    virtual void close();

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

  private:
    /*!
    * \brief Worker loop.
    *
    * Dequeues a new task. Sets the light parameters and renders. Dumps
    * the frame buffer. The loop ends when an end info is received.
    */
    void workerLoop();

    /*!
    * \brief Checks if the passed in info is an end info.
    *
    * An end info is the FrameDeviceInfo with time = -1 and 
    * devices = { NULL }.
    */
    bool isEndInfo(const FrameDeviceInfo &data) const;

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
      
    /*! \brief Indicates the status of the update loop.
    *
    * True if recording.
    */
    bool m_recording;
  };
    
}

#endif
