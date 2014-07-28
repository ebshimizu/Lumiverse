/*! \file ArnoldAnimationPatch.h
* \brief Implementation of a patch for a DMX system.
*/
#ifndef _ArnoldAnimationPATCH_H_
#define _ArnoldAnimationPATCH_H_

#pragma once

#include "LumiverseCoreConfig.h"

#include "../lib/libjson/libjson.h"
#include "ArnoldPatch.h"
#include "ArnoldMemoryFrameManager.h"

#include <future>
#include <thread>
#include <chrono>
#include <iostream>

namespace Lumiverse {
  struct FrameDeviceInfo {
	  time_t time;
	  std::set<Device *> devices;
      bool rerender_req;

	  FrameDeviceInfo() : time(-1), rerender_req(true) { }

	  void clear() {
		  time = -1;

		  for (Device *d : devices) {
			  if (d != NULL)
				  delete d;
		  }
		  devices.clear();
	  }
  };

  /*!
  * \brief  
  *
  *  
  * \sa  
  */
  class ArnoldAnimationPatch : public ArnoldPatch
  {
  public:
    /*!
    * \brief Constructs a DMXPatch object.
    */
	ArnoldAnimationPatch() : m_worker(NULL), 
		m_startPoint(std::chrono::system_clock::from_time_t(0)),
		m_frameManager(NULL) { }

	ArnoldAnimationPatch(const JSONNode data);

    /*!
    * \brief Destroys the object.
    */
    virtual ~ArnoldAnimationPatch();

	virtual void init();

	virtual string getType() { return "ArnoldAnimationPatch"; }

	virtual void update(set<Device *> devices);

	/*!
    * \brief Closes the Arnold session.
    */
    virtual void close();

    ArnoldFrameManager *getFrameManager() const;
      
  private:
	void workerLoop();

	bool isEndInfo(const FrameDeviceInfo &data) const;

    std::thread *m_worker;

	std::mutex m_queue;
	std::vector<FrameDeviceInfo> m_queuedFrameDeviceInfo;
	std::chrono::time_point<std::chrono::system_clock> m_startPoint;

	ArnoldFrameManager *m_frameManager;
  };
    
}

#endif
