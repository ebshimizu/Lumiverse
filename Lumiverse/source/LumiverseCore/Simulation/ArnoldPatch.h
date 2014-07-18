/*! \file ArnoldPatch.h
* \brief Implementation of a patch for Arnold.
*/
#ifndef _ArnoldPATCH_H_
#define _ArnoldPATCH_H_

#pragma once

#include "LumiverseCoreConfig.h"

#include <iostream>
#include <thread>
#include <algorithm>
#include <cstdio>

#include <ai.h>
#include "../Patch.h"
#include "../lib/libjson/libjson.h"
#include "ArnoldParameterVector.h"
#include "ArnoldInterface.h"


namespace Lumiverse {

  struct ArnoldParam;
    
  struct ArnoldLightRecord {
      ArnoldLightRecord()
      : rerender_req(true), light(NULL) { }
      ArnoldLightRecord(AtNode *node)
      : rerender_req(true), light(node) { }
      
      bool rerender_req;
      AtNode *light;
  };
    
  /*!
  * \brief  
  *
  *  
  * \sa  
  */
  class ArnoldPatch : public Patch
  {
  public:
    /*!
    * \brief Constructs a DMXPatch object.
    */
    ArnoldPatch() : m_renderloop(NULL) { }

    /*!
    * \brief Construct DMXPatch from JSON data.
    *
    * \param data JSONNode containing the DMXPatch object data.
    */
    ArnoldPatch(const JSONNode data);

    /*!
    * \brief Destroys the object.
    */
    virtual ~ArnoldPatch();

    /*!
    * \brief Updates the values sent to the DMX network given the list of devices
    * in the rig.
    *
    * The list of devices should be maintained outside of this class.
    */
    virtual void update(set<Device *> devices);

    /*!
    * \brief Initializes connections and other network settings for the patch.
    *
    * Call this AFTER all interfaces have been assigned. May need to call again
    * if interfaces change.
    */
    virtual void init();

    /*!
    * \brief Closes connections to the interfaces.
    */
    virtual void close();

    /*!
    * \brief Exports a JSONNode with the data in this patch
    *
    * \return JSONNode containing the DMXPatch object
    */
    virtual JSONNode toJSON();

    /*!
    * \brief Gets the type of this object.
    *
    * \return String containing "DMXPatch"
    */
    virtual string getType() { return "ArnoldPatch"; }
      
    int getWidth() { return m_interface.getWidth(); }
    int getHeight() { return m_interface.getHeight(); }
      
    float *getBufferPointer() { return m_interface.getBufferPointer(); }
      
    void interruptRender();
      
    void onDeviceChanged(Device *d);

  private:

    void renderLoop();
    
	bool updateLight(set<Device *> devices);

	void loadJSON(const JSONNode data);

	void loadLight(Device *d_ptr);

	map<string, ArnoldLightRecord> m_lights;

    ArnoldInterface m_interface;
      
    std::thread *m_renderloop;
  };
}

#endif
