/*! \file ArnoldFrameManager.h
* \brief Implementation of a patch for a DMX system.
*/
#ifndef _ArnoldFRAMEMANAGER_H_
#define _ArnoldFRAMEMANAGER_H_

#pragma once

#include "LumiverseCoreConfig.h"

#include <atomic>
#include <iostream>

namespace Lumiverse {
  /*!
  * \brief  
  *
  *  
  * \sa  
  */
  class ArnoldFrameManager
  {
  public:
    /*!
    * \brief Constructs a DMXPatch object.
    */
	ArnoldFrameManager() : m_current(0) { }

    /*!
    * \brief Destroys the object.
    */
    virtual ~ArnoldFrameManager();

	virtual void dump(time_t time, float *frame, size_t width, size_t height) = 0;

	virtual float *getCurrentFrameBuffer() const = 0;

	virtual time_t getCurrentTime() const = 0;

	virtual bool hasNext() const = 0;

	virtual void reset() {
		m_current = 0;
	}

	virtual void next() {
		if (hasNext())
			m_current++;
	}

	virtual void clear() {
		reset();
	}

  protected:
	  atomic<unsigned int> m_current;
  };
    
}

#endif
