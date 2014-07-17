/*! \file Patch.h
* \brief The Patch translates Lumiverse Data to Network Data
*/
#ifndef _PATCH_H_
#define _PATCH_H_

#pragma once

#include "Device.h"
#include <set>

using namespace std;

namespace Lumiverse {
  /*!
  * \brief A Patch maps devices to output channels and handles the output of data to the network.
  *
  * Patches do not expose the internal workings of their network transport to the Rig.
  * The user may choose to configure a Patch in a particular way, but the Rig doesn't
  * care as long as it can pass Device objects to a Patch.
  * The root Patch class is an abstract base class that defines the actions all
  * implementations of a Patch must have.
  */
  class Patch
  {
  public:
    /*! \brief Virtual destructor */
    virtual ~Patch() { };

    /*!
    * \brief Grabs values from list of Devices, translates them to proper format for the
    * given network, and outputs the values to the network if the device is patched.
    *
    * Each Patch will get the full list of Devices from the Rig. The Patch implementation
    * is responsible for finding the Devices that it needs, and then transmitting
    * the apropriate data over the network. Each Patch may do this differently
    * depending on the needs of the network.
    */
    virtual void update(set<Device *> devices) = 0;

    /*!
    * \brief Initializes settings for the patch.
    *
    * This can be starting up serial interfaces, network configuration, etc.
    * \sa Rig::init()
    */
    virtual void init() = 0;

    /*!
    * \brief Prepares for patch shutdown.
    */
    virtual void close() = 0;

    /*!
    * \brief Returns a JSON node representing the patch
    *
    * This is necessary for proper serialization of the Patch when the Rig
    * is serialied. If you don't want your Patch to be serializable for some reason,
    * you can return an empty JSONNode
    * \return JSONNode containing the data for the Patch
    * \sa Rig::loadPatches(), Rig::save()
    */
    virtual JSONNode toJSON() = 0;

    /*!
    * \brief Gets the type of the patch as a string.
    *
    * This function should be hardcoded to return a string.
    * This allows each object to be uniquely identified by a single string
    * and allows the user to then do more detailed manipulation of the Patch.
    * \return String representing the type of the Patch
    */
    virtual string getType() = 0;
      
    // Gets a mapping of device parameters to addresses for the patch type.
    // This is the full patch map.
    // Entries should look something like "deviceId.paramName -> 1 / 25" (DMX example)
    // virtual map<string, string> getPatchParams() = 0;

    // Maps Device IDs to starting addresses.
    // This is a more concise view of the patch map compared to getPatchParams()
    // virtual map<string, string> getPatchIDs() = 0;

    // Gets a map of patch data with implementation-defined options.
    // Allows flexible querying of patches with implementation-specific details.
    // virtual map<string, string> getPatchInfo(string opts) = 0;
  };
}

#endif