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

#include "SimulationPatch.h"
#include "../lib/libjson/libjson.h"
#include "ArnoldParameterVector.h"
#include "ArnoldInterface.h"

namespace Lumiverse {

  struct ArnoldParam;

  /*! \brief Record that denotes if a arnold light node requires
   * update.
   */    
  struct ArnoldLightRecord : public SimulationLightRecord {
      ArnoldLightRecord()
		  : light(NULL), SimulationLightRecord() {}
      ArnoldLightRecord(AtNode *node)
		  : light(node) {
		  metadata = AiNodeGetName(node);
		  rerender_req = true;
	  }
      
	  virtual void init() {
		  rerender_req = true;
		  light = NULL;
	  }

	  virtual void clear() {
		  light = NULL;
	  }

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
  class ArnoldPatch : public SimulationPatch
  {
  public:
    /*!
    * \brief Constructs a ArnoldPatch object.
    */
    ArnoldPatch() :
		SimulationPatch() { }

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
	virtual void init() override;

    /*!
    * \brief Closes the Arnold session.
    */
	virtual void close() override;

    /*!
    * \brief Exports a JSONNode with the data in this patch
    *
    * \return JSONNode containing the ArnoldPatch object
    */
	virtual JSONNode toJSON() override;

    /*!
    * \brief Gets the type of this object.
    *
    * \return String containing "ArnoldPatch"
    */
	virtual string getType() override { return "ArnoldPatch"; }
      
    /*!
    * \brief Gets the width of result.
    *
    * \return The width of result
    */
    virtual int getWidth() { 
		if (!m_interface.isOpen())
			m_interface.init();
		return m_interface.getWidth();
	}

    /*!
    * \brief Gets the height of result.
    *
    * \return The height of result
    */
	virtual int getHeight() {
		if (!m_interface.isOpen())
			m_interface.init();
		return m_interface.getHeight();
	}
      
    /*!
    * \brief Gets the pointer to the frame buffer.
    *
    * \return The pointer to the frame buffer.
    */
	virtual float *getBufferPointer() { 
		if (!m_interface.isOpen())
			m_interface.init();
		return m_interface.getBufferPointer();
	}
      
    /*!
    \brief Sets the width and height of the resulting image
    */
    bool setDims(int w, int h) {
      if (w > 0 && h > 0) {
        m_interface.setDims(w, h);
        return true;
      }
      return false;
    }

    size_t getBufferSize() {
      return getWidth() * getHeight() * 4;
    }

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
    virtual void interruptRender() override;

    /*!
    \brief Forcefully stops the current arnold render function.
    */
    void forceInterrupt() { m_interface.interrupt(); }
      
    /*!
    * \brief Gets the progress of current frame in percentage.
    *
    * \return The percent.
    */
    virtual float getPercentage() const { return m_interface.getPercentage(); }

    /*!
    * \brief Gets the current bucket for each worker thread.
    * \return An array of current buckets.
    */
    virtual BucketPositionInfo *getBucketPositionInfo() const { return m_interface.getBucketPositionInfo(); }

    /*!
    * \brief Gets number of buckets rendered simultanously.
    * This is usually the number of threads supported by hardware.
    * \return The number of buckets rendered simultanously.
    */
    virtual size_t getBucketNumber() const { return m_interface.getBucketNumber(); }

  protected:
    /*!
    * \brief Resets the arnold light node with updated parameters of deices.
    * This function updates light node for renderer.
    * \param devices The device list.
    */
    void updateLight(set<Device *> devices);

    /*!
    * \brief Loads a arnold light node.
    * This function is also used to update a light node.
    * \param d_ptr The device with updated parameters.
    */
    virtual void loadLight(Device *d_ptr) override;

    /*!
    * \brief Resets the arnold light node and surface with updated parameters of deices.
    * Experiment with methods from Picture Perfect RGB rendering.
    * \param devices The device list.
    */
    void updateLightPredictive(set<Device *> devices);
    
    /*!
    * \brief Loads data from a parsed JSON object
    * \param data JSON data to load
    */
    virtual void loadJSON(const JSONNode data) override;

    /*!
      * \brief Calls Arnold render function.
      * This function runs in a separate thread.
      */
    virtual bool renderLoop();

    /*!
    * \brief Arnold Interface
    */
    ArnoldInterface m_interface;

  private:
    void setOrientation(AtNode *light_ptr, Device *d_ptr, LumiverseOrientation *pan, LumiverseOrientation *tilt);

    void setOrientation(AtNode *light_ptr, Device *d_ptr, std::string pan_str, std::string tilt_str);

    /*!
      * \brief Modifies light color according to Picture Perfect RGB Rendering Using Spectral Prefiltering and Sharp Color Primaries.
    * \param d The device representing the light.
    * \param white The white spot in sharp RGB. (currently not used)
      */
    void modifyLightColor(Device *d, Eigen::Vector3d white);
  };
}

#endif

#endif
