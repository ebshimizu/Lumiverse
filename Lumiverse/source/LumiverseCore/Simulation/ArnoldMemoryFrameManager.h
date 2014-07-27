/*! \file ArnoldMemoryFrameManager.h
* \brief Implementation of a patch for a DMX system.
*/
#ifndef _ArnoldMemoryFRAMEMANAGER_H_
#define _ArnoldMemoryFRAMEMANAGER_H_

#pragma once

#include "LumiverseCoreConfig.h"

#include "../Patch.h"
#include "../lib/libjson/libjson.h"
#include "ArnoldFrameManager.h"

#include <future>
#include <thread>
#include <chrono>
#include <iostream>

namespace Lumiverse {
  struct FrameData {
	  time_t time;
	  float *buffer;

	  bool operator<(FrameData other) const {
		return time < other.time;
	  }
  };

  /*!
  * \brief  
  *
  *  
  * \sa  
  */
  class ArnoldMemoryFrameManager : public ArnoldFrameManager
  {
  public:
    /*!
    * \brief Constructs a DMXPatch object.
    */
	ArnoldMemoryFrameManager() :
		m_bufferSet([] (FrameData l, FrameData r) { return l < r; }) { }

    /*!
    * \brief Destroys the object.
    */
    virtual ~ArnoldMemoryFrameManager();

	virtual void dump(time_t time, float *frame, size_t width, size_t height);

	virtual float *getCurrentFrameBuffer() const;

	virtual time_t getCurrentTime() const;

	virtual bool hasNext() const;

	virtual void clear();

  private:
	  std::set<FrameData, function<bool(FrameData, FrameData)>> m_bufferSet;
	  std::mutex m_buffer;
  };
    
}

#endif
