/*! \file DMXInterface.h
* \brief Base class describing how DMX Interfaces should act
*/
#ifndef _DMXINTERFACE_H_
#define _DMXINTERFACE_H_

#pragma once

#include "../lib/libjson/libjson.h"
#include <string>
using namespace std;

namespace Lumiverse {
  /*!
  * \brief Provides a common interface to various DMX devices.
  *
  * Adding a new type of DMX device should be simple. Just derive
  * from this class and implement the required functions.
  * Interfaces for a DMX system are expected to take a 512 byte buffer and
  * send it across the network. Interfaces should be able to be initialized, closed
  * and reset. Any additional functions must be handled
  * specifically by subclasses.
  */
  class DMXInterface
  {
  public:
    /*! \brief  Virtual destructor yay */
    virtual ~DMXInterface() { };

    /*!
    * \brief Initalizes the interface.
    *
    * This function can throw exceptions if something comes up during
    * the initialization stage.
    */
    virtual void init() = 0;

    /*!
    * \brief Sends DMX through the interface.
    *
    * Also includes the universe number, for
    * devices that may manage more than one universe or need to know the universe number.
    * NOTE: The universe passed in here is ZERO-INDEXED as it is in the DMXPatch class.
    * \param data Data to send across the Universe
    * \param universe Universe number to send the DMX to
    */
    virtual void sendDMX(unsigned char* data, unsigned int universe) = 0;

    /*!
    * \brief Closes the connection to the DMX device
    */
    virtual void close() = 0;

    /*!
    * \brief Resets the interface.
    *
    * Is essentially a disconnect then reconnect and any other refresher code needed.
    */
    virtual void reset() = 0;

    /*!
    * \brief Returns the JSON representation of the interface
    * \return JSON node containing the data for this DMXInterface
    */
    virtual JSONNode toJSON() = 0;

    /*!
    * \brief Returns the name of the interface's type
    * 
    * The type of the interface referrs to what kind of device it is.
    * So for a DMX Pro 2 Interface, this would return "DMXPro2Interface"
    * \return String containing the type of the interface.
    */
    virtual string getInterfaceType() = 0;

    /*!
    * \brief Returns the name of this DMX interface.
    *
    * Note that name is not the same as Interface Type. The intent behind
    * name is to allow users to choose a common name to display for their interfaces
    * and still be able to use unique IDs to differentiate them.
    * \return Human-readable name for this particular interface.
    */
    string getInterfaceName() { return m_ifaceName; }

    /*!
    * \brief Returns the ID of this DMX interface.
    * 
    * \return Unique identifier for this interface
    */
    string getInterfaceId() { return m_ifaceId; }

    /*!
    * \brief Sets the interface ID
    * \param id New interface ID
    */
    void setInterfaceId(string id) { m_ifaceId = id; }

  protected:
    /*!
    * \brief Name of the DMX interface.
    */
    string m_ifaceName;

    /*!
    * \brief Unique string identifying this particular interface.
    */
    string m_ifaceId;
  };
}

#endif