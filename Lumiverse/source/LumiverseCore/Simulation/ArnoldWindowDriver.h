/*! \file DMXPatch.h
* \brief Implementation of a patch for a DMX system.
*/
#ifndef _ArnoldWindowPATCH_H_
#define _ArnoldWindowPATCH_H_

#pragma once

#include "LumiverseCoreConfig.h"

#include <ai.h>
#include <SDL.h>
#include "../Patch.h"
#include "../lib/libjson/libjson.h"

#include <iostream>

namespace Lumiverse {

  /*!
  * \brief  
  *
  *  
  * \sa  
  */
  class ArnoldWindowDriver
  {
  public:
    /*!
    * \brief Constructs a DMXPatch object.
    */
    ArnoldWindowDriver();

    /*!
    * \brief Destroys the object.
    */
    virtual ~ArnoldWindowDriver();

    static void drawToSurface(SDL_Surface *surface, const size_t x, const size_t y, const AtRGBA &rgba);

  private:
    
      
  };
}

#endif
