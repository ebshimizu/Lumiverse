/*! \file PhotoPatch.h
* \brief Implementation of a patch for Arnold.
*/
#ifndef _PhotoPatch_H_
#define _PhotoPatch_H_

#pragma once

#include "LumiverseCoreConfig.h"
#ifdef USE_ARNOLD

#include <iostream>
#include <thread>
#include <algorithm>
#include <cstdio>
#include <atomic>

#include "../Patch.h"
#include "SimulationPatch.h"
#include "../lib/libjson/libjson.h"

namespace Lumiverse {
    
	struct PhotoLightRecord : SimulationLightRecord {
		PhotoLightRecord()
			: intensity(0), color(1.f, 1.f, 1.f), photo(NULL), SimulationLightRecord() {}
		virtual ~PhotoLightRecord() {
			delete[] photo;
		}
		virtual void init() override {
			SimulationLightRecord::init();
			intensity = 0;
			color.setOnes();
		}

		virtual void clear() override {
			delete[] photo;
			photo = NULL;
		}

		float intensity;
		Eigen::Vector3f color;
		float *photo;// should be the path of the saved images
	};

  /*!
  * \brief The Arnold Patch object is responsible for the communication
  * between the Arnold renderer and the Lumiverse devices. The major part
  * of communication is done with help of an ArnoldInterface object.
  * PhotoPatch handles parsing Json and passing info to ArnoldInterface.
  *  
  * \sa PhotoAnimationPatch
  */
  class PhotoPatch : public SimulationPatch
  {
  public:
    /*!
    * \brief Constructs a PhotoPatch object.
    */
	PhotoPatch() : m_blend(NULL), m_blend_buffer(NULL),
				SimulationPatch() { }

    /*!
    * \brief Construct PhotoPatch from JSON data.
    *
    * \param data JSONNode containing the PhotoPatch object data.
    */
    PhotoPatch(const JSONNode data);

    /*!
    * \brief Destroys the object.
    */
    virtual ~PhotoPatch();

    /*!
    * \brief Initializes Arnold with ArnoldInterface.
    */
	virtual void init() override;

    /*!
    * \brief Exports a JSONNode with the data in this patch
    *
    * \return JSONNode containing the PhotoPatch object
    */
	virtual JSONNode toJSON() override;

    /*!
    * \brief Gets the type of this object.
    *
    * \return String containing "PhotoPatch"
    */
	virtual string getType() override { return "PhotoPatch"; }

	/*!
	* \brief Gets the width of result.
	*
	* \return The width of result
	*/
	virtual int getWidth() { return m_width; }

	/*!
	* \brief Gets the height of result.
	*
	* \return The height of result
	*/
	virtual int getHeight() { return m_height; }
      
	/*!
	* \brief Gets the pointer to the frame buffer.
	*
	* \return The pointer to the frame buffer.
	*/
	virtual float *getBufferPointer() override { return m_blend; }
      
	/*!
	* \brief Gets the progress of current frame in percentage.
	*
	* \return The percent.
	*/
	//virtual float getPercentage() const { return m_interface.getPercentage(); }

  //protected:
    /*!
    * \brief Resets the arnold light node with updated parameters of deices.
    * This function updates light node for renderer.
    * \param devices The device list.
    */
    virtual void updateLight(set<Device *> devices);

	/*!
	* \brief Loads a arnold light node.
	* This function is also used to update a light node.
	* \param d_ptr The device with updated parameters.
	*/
	virtual void loadLight(Device *d_ptr);

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

	virtual void bindRenderLoop() override;

	void interruptRender();

	float *m_blend;

	int m_height;
	int m_width;


  private:

	bool blendUint8(float* blended, unsigned char* light, float intensity, Eigen::Vector3f color);

	bool blendFloat(float* blended, float* light, float intensity, Eigen::Vector3f color);

	float *m_blend_buffer;

	std::string m_default_path;

  };
}


#endif
#endif