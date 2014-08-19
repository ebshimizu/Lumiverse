/*! \file ArnoldMemoryFrameManager.h
* \brief An implemnetation for frame manager using in-memory buffer.
*/
#ifndef _ArnoldMemoryFRAMEMANAGER_H_
#define _ArnoldMemoryFRAMEMANAGER_H_

#pragma once

#include "LumiverseCoreConfig.h"

#include "../lib/libjson/libjson.h"
#include "ArnoldFrameManager.h"

#include <mutex>
#include <functional>
#include <set>
#include <chrono>
#include <iostream>

namespace Lumiverse {
    /*! \brief The frame buffer data. */
    struct FrameData {
	// The time point
	time_t time;
	// RGBA frame buffer
	float *buffer;

	// To make the ordered set.
	bool operator<(FrameData other) const {
	    return time < other.time;
	}
    };

    /*!
     * \brief An implemnetation for frame manager using in-memory buffer.
     *
     * For speed concern, stores frame buffers in memory. Currently it's
     * not possible to reuse frame buffers from previous run.
     * \sa ArnoldFrameManager
     */
    class ArnoldMemoryFrameManager : public ArnoldFrameManager
    {
      public:
	/*!
	 * \brief Constructs a ArnoldMemoryFrameManager object.
	 */
    ArnoldMemoryFrameManager() :
        	m_bufferSet([] (FrameData l, FrameData r) { return l < r; }) { }

	/*!
	 * \brief Destroys the object.
	 */
	virtual ~ArnoldMemoryFrameManager();

	/*!
	 * \brief Dumps the frame buffer at given time, with a size of 
	 * width * height, to memory.
	 */
	virtual void dump(time_t time, float *frame, size_t width, size_t height);

	/*!
	 * \brief Returns the frame buffer pointed by the current cursor.
	 *
	 * The current implementation is to move iterator from beginning
	 * every time.
	 * \return The current frame buffer.
	 */
	virtual float *getCurrentFrameBuffer() const;

	/*!
	 * \brief Returns the time of current frame  pointed by the current cursor.
	 *
	 * The current implementation is to move iterator from beginning
	 * every time.
	 * \return The current time point.
	 */
	virtual time_t getCurrentTime() const;

	/*!
	 * \brief Returns the time of next frame without moving cursor to next.
	 * This time point can be used to determine the right moment to call
	 * next().
	 * \return The time point of next frame.
	 */
	virtual time_t getNextTime() const;
      
	/*!
	 * \brief Checks if we are at the end of the frame list.
	 *
	 * \return If we have reached the end.
	 */
	virtual bool hasNext() const;
        
	/*!
	* \brief Checks if there's no frame stored inside.
	* \return If it's empty.
	*/
    virtual bool isEmpty() const { return m_bufferSet.size() == 0; }

	/*!
	* \brief Gets the number of frames stored.
	*
	* \return The number of frames stored.
	*/
    virtual size_t getFrameNum() const { return m_bufferSet.size(); }
        
	/*!
	 * \brief Clears the frame manager.
	 * This function should be called before the object is destroyed.
	 * Releases memory for each frame buffer.
	 */
	virtual void clear();

      private:
	// A set for frame data ordered by time ascendingly.
	std::set<FrameData, std::function<bool(FrameData, FrameData)>> m_bufferSet;
	// Lock for the set.
	std::mutex m_buffer;
    };
    
}

#endif
