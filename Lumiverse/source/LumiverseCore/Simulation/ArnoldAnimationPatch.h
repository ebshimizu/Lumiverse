/*! \file ArnoldAnimationPatch.h
* \brief Subclass of ArnoldPatch to render frames of an animation.
*/
#ifndef _ArnoldAnimationPATCH_H_
#define _ArnoldAnimationPATCH_H_

#pragma once

#include "LumiverseCoreConfig.h"
#ifdef USE_ARNOLD

#include "../lib/libjson/libjson.h"
#include "SimulationAnimationPatch.h"
#include "ArnoldPatch.h"
#include "ArnoldMemoryFrameManager.h"
#include "ArnoldFileFrameManager.h"

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
  enum SimulationAnimationMode;
    
  /*! \brief The state info for worker thread. */
  struct FrameDeviceInfo;
    
  /*!
  * \brief A subclass of ArnoldPatch. 
  * Instead of interrupting the worker thread every time a new rendering 
  * task is received, this class keeps all requests in a queue. A worker
  * thread grasps tasks and dumps frame buffer to a ArnoldFrameManager.
  *  
  * \sa ArnoldPatch, ArnoldFrameManager
  */
  class ArnoldAnimationPatch : public SimulationAnimationPatch,
								public ArnoldPatch
  {
  public:
    /*!
    * \brief Constructs a ArnoldAnimationPatch object.
    */
	  ArnoldAnimationPatch() : SimulationAnimationPatch(),
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
    * \brief Gets the type of this object.
    *
    * \return String containing "ArnoldAnimationPatch"
    */
    virtual string getType() override { return "ArnoldAnimationPatch"; }

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
    * \brief Manually schedule a re-rendering and make sure the task be inserted into queue.
    *
    * This new rendering task may not be done immediately, but it will get inserted.
    */
    virtual void rerender();

    /*!
    * \brief Stops the working rendering procedure if Arnold is running. Potentially
    * interrupt rendering process if the patch is in rendering mode.
    */
    virtual void interruptRender();

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
    * \brief Gets the current rendering progress as percentage.
    *
    * The value returned may not be the accurate number due to concurrency.
    * Also the accuracy is limited to number of frame.
    * \return The current rendering progress as percentage
    */
    virtual float getPercentage() const override;

    virtual void reset();

    /*!
    \brief Returns the Arnold interface
    */
    ArnoldInterface* getArnoldInterface() { return &m_interface; }

    /*!
    \brief Disables continuous rendering of frames in the ArnoldAnimationPatch.
    */
    void disableContinuousRenderMode();

    /*!
    \brief Enables the continuous rendering of frames in the ArnoldAnimationPatch

    This is the default mode of operation for an ArnoldAnimationPatch.
    */
    void enableContinuousRenderMode();

    /*!
    \brief Renders a single frame from the current state of Devices and saves it to the specified filename.

    This function will block while the frame is rendering.
    */
    void renderSingleFrame(const set<Device *>& devices, string basepath, string filename);

    /*!
    \brief Renders a single frame from the current state of Devices to a buffer.

    Buffer format is RGBA 32-bit.
    */
    void renderSingleFrameToBuffer(const set<Device*>& devices, unsigned char* buff);

  protected:
    virtual void onRecording() override { setSamples(m_preview_samples); }

    virtual void onRendering() override { setSamples(m_render_samples); }
    /*!
    * \brief Loads data from a parsed JSON object
    * \param data JSON data to load
    */
    virtual void loadJSON(const JSONNode data) override;

    virtual void workerRender(FrameDeviceInfo frame);

    virtual void createFrameInfoBody(set<Device *> devices, FrameDeviceInfo &frame, bool forceUpdate = false);

  private:
    /*! \brief The camera sampling rate for preview.
    */
    int m_preview_samples;

    /*! \brief The camera sampling rate for rendering.
    */
    int m_render_samples;

  };
    
}

#endif

#endif
