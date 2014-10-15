/*! \file SimulationPatch.h
* \brief Implementation of a patch for Arnold.
*/
#ifndef _SimulationPATCH_H_
#define _SimulationPATCH_H_

#pragma once

#include "LumiverseCoreConfig.h"

#include <iostream>
#include <thread>
#include <algorithm>
#include <cstdio>
#include <atomic>

#include "../Patch.h"
#include "../lib/libjson/libjson.h"

namespace Lumiverse {

  /*! \brief Record that denotes if a arnold light node requires
   * update.
   */    
  struct SimulationLightRecord {
	  SimulationLightRecord()
		  : metadata(""), rerender_req(true) { }
	  virtual ~SimulationLightRecord() { }
      
	  virtual void init() { rerender_req = true; }

	  std::string metadata;
      bool rerender_req;
  };

    
  /*!
  * \brief The Arnold Patch object is responsible for the communication
  * between the Arnold renderer and the Lumiverse devices. The major part
  * of communication is done with help of an ArnoldInterface object.
  * SimulationPatch handles parsing Json and passing info to ArnoldInterface.
  *  
  * \sa ArnoldInterface, ArnoldAnimationPatch
  */
  class SimulationPatch : public Patch
  {
  public:
    /*!
    * \brief Constructs a SimulationPatch object.
    */
	SimulationPatch() : m_renderloop(NULL) {
		m_interrupt_flag.clear();
	}

    /*!
    * \brief Construct SimulationPatch from JSON data.
    *
    * \param data JSONNode containing the SimulationPatch object data.
    */
    SimulationPatch(const JSONNode data);

    /*!
    * \brief Destroys the object.
    */
    virtual ~SimulationPatch();

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
    * \return JSONNode containing the SimulationPatch object
    */
    virtual JSONNode toJSON();

    /*!
    * \brief Gets the type of this object.
    *
    * \return String containing "SimulationPatch"
    */
    virtual string getType() { return "SimulationPatch"; }
      
    /*!
    * \brief Gets the width of result.
    *
    * \return The width of result
    */
	virtual int getWidth() = 0;

    /*!
    * \brief Gets the height of result.
    *
    * \return The height of result
    */
	virtual int getHeight() = 0;
      
    /*!
    * \brief Gets the pointer to the frame buffer.
    *
    * \return The pointer to the frame buffer.
    */
	virtual float *getBufferPointer() { return NULL; }

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
    virtual void onDeviceChanged(Device *d);
      
	/*!
	* \brief Manually schedule a re-rendering.
	*
	* This new rendering task may not be done immediately. This function just sets on the flag for rendering.
	*/
    virtual void rerender();
      
	/*!
	* \brief Checks if any device connected with this patch has updated parameters or metadata.
	* \param devices The device list.
	* \return If there is any update.
	*/
	virtual bool isUpdateRequired(set<Device *> devices);


	/*!
	* \brief Resets the update flags for lights.
	*/
	virtual void clearUpdateFlags();

	/*!
	* \brief Gets the progress of current frame in percentage.
	*
	* \return The percent.
	*/
	//virtual float getPercentage() const { return m_interface.getPercentage(); }

  protected:

    /*!
    * \brief Resets the arnold light node with updated parameters of deices.
    * This function updates light node for renderer.
    * \param devices The device list.
    */
	virtual void updateLight(set<Device *> devices) = 0;

	/*!
	* \brief Loads a arnold light node.
	* This function is also used to update a light node.
	* \param d_ptr The device with updated parameters.
	*/
	virtual void loadLight(Device *d_ptr);

	/*!
	* \brief Loads a arnold light node.
	* This function is also used to update a light node.
	* \param d_ptr The device with updated parameters.
	*/
	virtual void loadLight(std::string light) = 0;
    
    /*!
    * \brief Loads data from a parsed JSON object
    * \param data JSON data to load
    */
    virtual void loadJSON(const JSONNode data);

	/*!
    * \brief Calls Arnold render function.
    * This function runs in a separate thread.
    */
    virtual bool renderLoop() = 0;

	virtual void bindRenderLoop();

    /*!
    * \brief A list contains infos about if a light is updated.
    */
    map<string, SimulationLightRecord*> m_lights;

	std::atomic_flag m_interrupt_flag;

	/*!
	* \brief The separate thread running the render loop.
	*/
	std::thread *m_renderloop;
  };
}



#endif
