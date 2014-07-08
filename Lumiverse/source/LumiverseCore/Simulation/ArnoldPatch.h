/*! \file DMXPatch.h
* \brief Implementation of a patch for a DMX system.
*/
#ifndef _ArnoldPATCH_H_
#define _ArnoldPATCH_H_

#pragma once

#include "LumiverseCoreConfig.h"

#include <ai.h>
#include "../Patch.h"
#include "../lib/libjson/libjson.h"

#include <iostream>

namespace Lumiverse {

  struct LightParam {
	  LightParam()
		  : rerender_req(false), reblend_req(false), position(AI_V3_ZERO), color(AI_RGB_BLACK) { }
	  LightParam(AtVector position, AtColor color)
		  : rerender_req(true), reblend_req(true), position(position), color(color) { }
    
	  // TODO: re-render the scene if position changed.
	  bool rerender_req;
	  bool reblend_req;
	  AtVector position;

	  AtColor color;
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
    ArnoldPatch();

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

  private:
      
    void setParameter(AtNode *light_ptr, const std::string &paramName, LumiverseType *val_ptr);

	bool updateLight(set<Device *> devices);

	void loadJSON(const JSONNode data);

	void loadLight(const JSONNode data);

	void loadLight(Device *d_ptr);

	map<string, AtNode*> m_lights;

	map<string, LightParam> m_light_params;
    
    std::string m_ass_file;

  };
}

#endif
