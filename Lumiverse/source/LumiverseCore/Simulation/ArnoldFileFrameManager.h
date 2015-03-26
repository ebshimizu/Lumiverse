/*! \file ArnoldFileFrameManager.h
* \brief An implemnetation for frame manager using in-memory buffer.
*/
#ifndef _ArnoldFileFRAMEMANAGER_H_
#define _ArnoldFileFRAMEMANAGER_H_

#pragma once

#include "LumiverseCoreConfig.h"

#include "../lib/libjson/libjson.h"
#include "Logger.h"
#include "ArnoldFrameManager.h"
#include "imageio.h"

#include <mutex>
#include <functional>
#include <set>
#include <chrono>
#include <sstream>
#include <iostream>
#include <cstdio>
#include "../lib/libpng/png.h"

namespace Lumiverse {
    /*!
     * \brief An implemnetation for frame manager using in-memory buffer.
     *
     * For speed concern, stores frame buffers in memory. Currently it's
     * not possible to reuse frame buffers from previous run.
     * \sa ArnoldFrameManager
     */
    class ArnoldFileFrameManager : public ArnoldFrameManager
    {
      public:
		/*!
		 * \brief Constructs a ArnoldFileFrameManager object.
		 */
		  ArnoldFileFrameManager(std::string frame_path, size_t fps = 24) :
			  m_frame_path(frame_path), m_prev_frame(-1), m_current(0), m_buffer(NULL),
			  ArnoldFrameManager(fps) { }

		/*!
		 * \brief Destroys the object.
		 */
		virtual ~ArnoldFileFrameManager();

		/*!
		 * \brief Dumps the frame buffer at given time, with a size of 
		 * width * height, to memory.
		 */
		virtual void dump(time_t time, float *frame, size_t width, size_t height);

    /*!
    \brief Saves the specified image buffer to a file.
    */
    void saveToFile(string file, float *frame, size_t width, size_t height);

		/*!
		 * \brief Returns the frame buffer pointed by the current cursor.
		 *
		 * The current implementation is to move iterator from beginning
		 * every time.
		 * \return The current frame buffer.
		 */
		virtual float *getCurrentFrameBuffer();

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
		virtual bool isEmpty() const;

		/*!
		* \brief Gets the number of frames stored.
		*
		* \return The number of frames stored.
		*/
		virtual size_t getFrameNum() const;
        
		/*!
		 * \brief Clears the frame manager.
		 * This function should be called before the object is destroyed.
		 * Deletes files for each frame buffer.
		 */
		virtual void clear();

		/*!
		* \brief Gets type of a ArnoldFrameManager object.
		*
		* \return The type.
		*/
		virtual std::string getType() { return "ArnoldFileFrameManager"; }

      private:
		 /*!
		 * \brief Checks if a given file exists.
		 *
		 * \return If the file exists.
		 */
		bool fileExists(std::string fileName) const;

		void deleteFile(std::string fileName) const;

		// A frame buffer of current frame. This buffer is only used to display.
		float *m_buffer;

		// The path to the directory containing all frames.
		std::string m_frame_path;

		// The index of the last frame from the previous call.
		int m_prev_frame;

		// The index of current frame.
		size_t m_current;
    };
    
}

#endif
