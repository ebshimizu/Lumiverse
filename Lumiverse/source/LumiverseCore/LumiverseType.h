/*! \file LumiverseType.h
* \brief A LumiverseType contains information about a Device Parameter
*/
#ifndef _LumiverseTYPE_H_
#define _LumiverseTYPE_H_
#pragma once

#include <string>
#include "Logger.h"
#include "lib/libjson/libjson.h"

using namespace std;

namespace Lumiverse {
  /*!
  * \brief This class is a wapper around a variety of different possible
  * data types that might be needed by a Device.
  *
  * You can define a new type be deriving from this class, and then
  * updating the functions in LumiverseTypeUtils
  * \sa LumiverseTypeUtils
  */
  class LumiverseType
  {
  public:
    /*! \brief Destroys the object. */
    virtual ~LumiverseType() { };

    /*!
    * \brief Gets the name of the type.
    * 
    * Each subclass definies this to uniquely identify the type.
    * \return String representing the type of the object
    */
    virtual string getTypeName() = 0;

    /*!
    * \brief Resets the data to a type-defined default.
    */
    virtual void reset() = 0;

    /*! 
    * \brief Converts the type to a JSON object with the specified name
    * 
    * Required for proper serialiaztion.
    */
    virtual JSONNode toJSON(string name) = 0;

    // Yeah actually there's not much here because types are
    // all different.
  };
}
#endif