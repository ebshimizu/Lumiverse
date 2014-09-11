/*! \file ArnoldFrameManager.h
* \brief The superclass for all implementation of frame
* managers. 
*/
#ifndef _ArnoldFRAMEMANAGER_H_
#define _ArnoldFRAMEMANAGER_H_

#pragma once

#include "LumiverseCoreConfig.h"

#ifdef USE_ARNOLD

#include <atomic>
#include <iostream>

namespace Lumiverse {
  /*!
  * \brief The superclass for all implementation of frame managers.
  *
  * A frame manager is used to hide implementation
  * details of frame buffer storage. For example, a frame buffer
  * may be stored in memory or in file system.
  * \sa ArnoldAnimationPatch, ArnoldMemoryFrameManager
  */
  class ArnoldFrameManager
  {
  public:
    /*!
    * \brief Constructs a ArnoldFrameManager object.
    *
    * Sets the cursor to the beginning.
    */
    ArnoldFrameManager(size_t fps = 24) : m_fps(fps), m_current(0) { }

    /*!
    * \brief Destroys the object.
    */
    virtual ~ArnoldFrameManager() { }

    /*!
    * \brief Dumps the frame buffer at given time, with a size of 
    * width * height.
    */
    virtual void dump(time_t time, float *frame, size_t width, size_t height) = 0;

    /*!
    * \brief Returns the frame buffer pointed by the current cursor.
    *
    * \return The current frame buffer.
    */
    virtual float *getCurrentFrameBuffer() = 0;

    /*!
    * \brief Returns the time of current frame  pointed by the current cursor.
    *
    * \return The current time point.
    */
    virtual time_t getCurrentTime() const = 0;
      
    /*!
    * \brief Returns the time of next frame without moving cursor to next.
    * This time point can be used to determine the right moment to call
    * next().
    * \return The time point of next frame.
    */
    virtual time_t getNextTime() const = 0;

    /*!
    * \brief Checks if we are at the end of the frame list.
    *
    * \return If we have reached the end.
    */
    virtual bool hasNext() const = 0;
      
	/*!
	* \brief Checks if there's no frame stored inside.
	* \return If it's empty.
	*/
    virtual bool isEmpty() const = 0;
      
	/*!
	* \brief Gets the number of frames stored.
	*
	* \return The number of frames stored.
	*/
    virtual size_t getFrameNum() const = 0;

    /*!
    * \brief Resets the cursor to the beginning.
    */
    virtual void reset() {
        m_current = 0;
    }

    /*!
    * \brief Moves the cursor to next position.
    */
    virtual void next() {
        if (hasNext())
            m_current++;
    }

    /*!
    * \brief Clears the frame manager.
    * This function should be called before the object is destroyed.
    * This may involve releasing memory block or closing file handle.
    */
    virtual void clear() {
        reset();
    }

	/*!
	* \brief Gets type of a ArnoldFrameManager object.
	*
	* \return The type.
	*/
	virtual std::string getType() { return "ArnoldFrameManager"; }

	virtual void setFps(int fps) { m_fps = fps; }
	virtual int getFps() { return m_fps; }

  protected:
    // An atomic counter to implement the cursor.
    std::atomic<unsigned int> m_current;
	size_t m_fps;
  };
    
}
#endif

#endif
