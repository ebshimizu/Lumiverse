/*! \file ArnoldPatch.h
* \brief Implementation of a patch for Arnold.
*/
#ifndef _ArnoldPATCH_H_
#define _ArnoldPATCH_H_

#pragma once

#include "LumiverseCoreConfig.h"
#ifdef USE_ARNOLD

#include <iostream>
#include <thread>
#include <algorithm>
#include <cstdio>

#include "../Patch.h"
#include "../lib/libjson/libjson.h"
#include "ArnoldParameterVector.h"
#include "ArnoldInterface.h"

namespace Lumiverse {

  struct ArnoldParam;

  /*! \brief Record that denotes if a arnold light node requires
   * update.
   */    
  struct ArnoldLightRecord {
      ArnoldLightRecord()
		  : arnold_type(""), rerender_req(true), light(NULL) { }
      ArnoldLightRecord(AtNode *node)
		  : arnold_type(AiNodeGetName(node)), rerender_req(true), light(node) { }
      
	  std::string arnold_type;
      bool rerender_req;
      AtNode *light;
  };
    
  /*!
  * \brief The Arnold Patch object is responsible for the communication
  * between the Arnold renderer and the Lumiverse devices. The major part
  * of communication is done with help of an ArnoldInterface object.
  * ArnoldPatch handles parsing Json and passing info to ArnoldInterface.
  *  
  * \sa ArnoldInterface, ArnoldAnimationPatch
  */
  class ArnoldPatch : public Patch
  {
  public:
    /*!
    * \brief Constructs a ArnoldPatch object.
    */
    ArnoldPatch() : m_renderloop(NULL) { }

    /*!
    * \brief Construct ArnoldPatch from JSON data.
    *
    * \param data JSONNode containing the ArnoldPatch object data.
    */
    ArnoldPatch(const JSONNode data);

    /*!
    * \brief Destroys the object.
    */
    virtual ~ArnoldPatch();

    /*!
    * \brief Updates the rendering given the list of devices
    * in the rig.
    *
    * The list of devices should be maintained outside of this class. 
    * This function would potentially interrupt the rendering and
    * restart with new parameters.
    */
    virtual void update(set<Device *> devices);

    /*!
    * \brief Initializes Arnold with ArnoldInterface.
    */
    virtual void init();

    /*!
    * \brief Closes the Arnold session.
    */
    virtual void close();

    /*!
    * \brief Exports a JSONNode with the data in this patch
    *
    * \return JSONNode containing the ArnoldPatch object
    */
    virtual JSONNode toJSON();

    /*!
    * \brief Gets the type of this object.
    *
    * \return String containing "ArnoldPatch"
    */
    virtual string getType() { return "ArnoldPatch"; }
      
    /*!
    * \brief Gets the width of result.
    *
    * \return The width of result
    */
    int getWidth() { return m_interface.getWidth(); }

    /*!
    * \brief Gets the height of result.
    *
    * \return The height of result
    */
    int getHeight() { return m_interface.getHeight(); }
      
    /*!
    * \brief Gets the pointer to the frame buffer.
    *
    * \return The pointer to the frame buffer.
    */
    float *getBufferPointer() { return m_interface.getBufferPointer(); }
      
    /*!
    * \brief Gets the sample rate (n * n per pixel).
    *
    * \return The number of AA samples.
    */
    int getSamples() { return m_interface.getSamples(); }
      
    /*!
    * \brief Sets the sample rate (n * n per pixel).
    *
    * \param samples The number of AA samples.
    */
    void setSamples(int samples);
      
    /*!
    * \brief Stops the working rendering procedure if Arnold is running.
    */
    virtual void interruptRender();
    
    /*!
    * \brief Callback function for devices.
    *
    * This function is registered to all devices by the rig. Only devices in the list
    * will change the state of patch.
    * \param d The device which calls this function.
    */
    void onDeviceChanged(Device *d);
      
	/*!
	* \brief Manually schedule a re-rendering.
	*
	* This new rendering task may not be done immediately. This function just sets on the flag for rendering.
	*/
    void rerender();
      
	virtual float getPercentage() const { return m_interface.getPercentage(); }

	virtual BucketPositionInfo *getBucketPositionInfo() const { return m_interface.getBucketPositionInfo(); }

	virtual size_t getBucketNumber() const { return m_interface.getBucketNumber(); }

  protected:
    /*!
    * \brief Checks if any device connected with this patch has updated parameters or metadata.
    * \param devices The device list.
    * \return If there is any update.
    */
    bool isUpdateRequired(set<Device *> devices);
      
    /*!
    * \brief Resets the arnold light node with updated parameters of deices.
    * This function updates light node for renderer.
    * \param devices The device list.
    */
    void updateLight(set<Device *> devices);
    
    /*!
    * \brief Resets the update flags for lights.
    */
    void clearUpdateFlags();
    
    /*!
    * \brief Loads data from a parsed JSON object
    * \param data JSON data to load
    */
    void loadJSON(const JSONNode data);
      
    /*!
    * \brief Loads a arnold light node.
    * This function is also used to update a light node.
    * \param d_ptr The device with updated parameters.
    */
    void loadLight(Device *d_ptr);

	/*!
    * \brief Calls Arnold render function.
    * This function runs in a separate thread.
    */
    void renderLoop();

    /*!
    * \brief A list contains infos about if a light is updated.
    */
    map<string, ArnoldLightRecord> m_lights;

    /*!
    * \brief Arnold Interface
    */
    ArnoldInterface m_interface;

  private:
	
    /*!
    * \brief The separate thread running the render loop.
    */
    std::thread *m_renderloop;
  };
}

#endif

#endif
