 /*! \file LumiverseTypeUtils.h
* \brief Utility functions for manipulating LumiverseTypes
*/
#ifndef _LUMIVERSETYPEUTILS_H_
#define _LUMIVERSETYPEUTILS_H_

#pragma once

#include "LumiverseFloat.h"
#include "LumiverseEnum.h"
#include "LumiverseColor.h"
#include "LumiverseOrientation.h"
#include <math.h>

namespace Lumiverse {
  /*! 
  * \namespace Lumiverse::LumiverseTypeUtils
  * \brief Functions to make life with Lumiverse generic types easier
  *
  * This namespace contains utility functions for manipulating and comparing
  * LumiverseType objects. As new types are added, these functions
  * will be updated as needed to handle the new types.
  * \sa LumiverseType, LumiverseFloat, LumiverseEnum
  */
  namespace LumiverseTypeUtils {
    /*!
    * \brief Copies a LumiverseType and returns an abstracted pointer to the new value.
    *
    * \return Pointer to a copy of the type or nullptr if type is unknown to the Lumiverse system.
    * Memory is allocated for a copy, caller must free.
    */
    LumiverseType* copy(LumiverseType* data);

    /*!
    * \brief Copies the data from source into target.
    *
    * \param source Pointer to the data source
    * \param target Pointer to the copy location
    */
    void copyByVal(LumiverseType* source, LumiverseType* target);

    /*!
    * \brief Compares two generic LumiverseType pointers for equality
    *
    * Identifies the type of the objects and then invokes the proper comparison.
    * \param lhs Object on the left hand side
    * \param rhs Object on the right hand side
    */
    bool equals(LumiverseType* lhs, LumiverseType* rhs);

    /*!
    * \brief Compares two LumiverseType objects.
    *
    * \return 0 if lhs and rhs are equal,
    * 1 if lhs > rhs,
    * -1 if lhs < rhs,
    * -2 if lhs and rhs are not the same type, are null, or are of unknown type
    */
    int cmp(LumiverseType* lhs, LumiverseType* rhs);

    /*!
    * \brief Lerps the values of a LumiverseType and returns the value
    *
    * \returns Value of the lerp in a new LumiverseType object.
    * Value is equal to `lhs * (1 - t) + rhs * t`.
    */
    shared_ptr<LumiverseType> lerp(LumiverseType* lhs, LumiverseType* rhs, float t);

    /*!
    * \brief Checks the types of two LumiverseType objects
    * 
    * \return True if lhs and rhs are not null and both the same type
    */
    inline bool areSameType(LumiverseType* lhs, LumiverseType* rhs);

    Eigen::Matrix3f getRotationMatrix(Eigen::Vector3f lookat, Eigen::Vector3f up, LumiverseOrientation pan, LumiverseOrientation tilt);

    /*!
    * \brief Compares two LumiverseTypes with <
    *
    * Uses cmp to determin this inequality.
    * \return true if lhs < rhs
    */
    bool lessThan(LumiverseType* lhs, LumiverseType* rhs);

    /*!
    \brief Multiplies a parameter by a constant value (typically from 0 to 1, but no limits
    are enforced in this function).

    The scaling is done inline, as in this function directly modifies the value passed in.
    \param val LumiverseType to scale.
    \param scale The scaling factor.
    */
    void scaleParam(LumiverseType* val, float scale);

    /*! \brief Loads a LumiverseType from a JSON node. */
    LumiverseType* loadFromJSON(JSONNode node);
  }
}
#endif