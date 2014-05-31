/*! \file DMXPatch.h
* \brief Implementation of a patch for a DMX system.
*/
#ifndef _DMXPATCH_H_
#define _DMXPATCH_H_

#pragma once

#include "LumiverseCoreConfig.h"
#include "../Patch.h"
#include "DMXDevicePatch.h"
#include "DMXInterface.h"
#include "../lib/libjson/libjson.h"

#ifdef USE_DMXPRO2
  #include "DMXPro2Interface.h"
#endif

#include <iostream>

namespace Lumiverse {

  /*!
  * \brief The DMX Patch object manages the communication between the DMX network
  * and the Lumiverse devices.
  *
  * To add interfaces to this particular patch, you make a new interface object
  * and assign it to a universe. You can assign multiple interfaces to the same
  * universe, and you can assign a single device to multiple universes. However
  * the latter option is probably only useful if your single device handles
  * multiple universes.
  *
  * Patching devices is done by providing the DMXPatch with a map telling it
  * which device ID goes to which address. That map will also contain information
  * indicating how the LumiverseType should be converted to DMX values. These
  * maps are then stored in a map within the DMXPatch using the device type they
  * control as the key. This allows simple reuse and automatic map usage based
  * on device types.
  *
  * Note that the DMXPatch does not do additional configuration of the DMXInterface
  * beyond what is described in the abstract DMXInterface class. You can access
  * other implementation specific settings directly as permitted by the implementation.
  * \sa DMXDevicePatch, DMXInterface
  */
  class DMXPatch : public Patch
  {
  public:
    /*!
    * \brief Constructs a DMXPatch object.
    */
    DMXPatch();

    /*!
    * \brief Construct DMXPatch from JSON data.
    *
    * \param data JSONNode containing the DMXPatch object data.
    */
    DMXPatch(const JSONNode data);

    /*!
    * \brief Destroys the object.
    */
    virtual ~DMXPatch();

    /*!
    * \brief Updates the values sent to the DMX network given the list of devices
    * in the rig.
    *
    * The list of devices should be maintained outside of this class.
    */
    virtual void update(set<Device *> devices);

    /*!
    * \brief Initializes connections and other network settings for the patch.
    *
    * Call this AFTER all interfaces have been assigned. May need to call again
    * if interfaces change.
    */
    virtual void init();

    /*!
    * \brief Closes connections to the interfaces.
    */
    virtual void close();

    /*!
    * \brief Exports a JSONNode with the data in this patch
    *
    * \return JSONNode containing the DMXPatch object
    */
    virtual JSONNode toJSON();

    /*!
    * \brief Gets the type of this object.
    *
    * \return String containing "DMXPatch"
    */
    virtual string getType() { return "DMXPatch"; }

    // Gets a mapping of device parameters to addresses for the patch type.
    // This is the full patch map.
    // Entries should look something like "deviceId.paramName -> 1 / 25" (DMX example)
    // virtual map<string, string> getPatchParams();

    // Maps Device IDs to starting addresses.
    // This is a more concise view of the patch map compared to getPatchParams()
    // virtual map<string, string> getPatchIDs();

    // Gets a map of patch data with implementation-defined options.
    // Allows flexible querying of patches with implementation-specific details.
    // virtual map<string, string> getPatchInfo(string opts);

    /*!
    * \brief Assigns an interface to a universe.
    *
    * Interfaces allocated by the user will be freed by the patch
    * either on program end or on unpatch. If the interface is already assigned to
    * a universe, it will not be moved.
    * \param iface Pointer to the interface object to use.
    * \param universe Universe to assign to the interface
    * \sa moveInterface
    */
    void assignInterface(DMXInterface* iface, unsigned int universe);

    /*!
    * \brief Deletes an interface with id "id" from the patch.
    *
    * Note that this will unmap ALL universes mapped to this interface and deallocate it.
    * \param id Interface ID to delete.
    */
    void deleteInterface(string id);

    /*!
    * \brief Moves an interface from a specified universe to a specified universe.
    *
    * If the interface isn't in "universeFrom" already it just gets assigned to
    * "universeTo"
    * \param id Interface ID to move
    * \param universeFrom Universe to move from
    * \param universeTo Universe to move to
    */
    void moveInterface(string id, unsigned int universeFrom, unsigned int universeTo);

    /*!
    * \brief Returns the interface map. ID -> Universe.
    * 
    * \return Map of interface id to universe number.
    */
    const multimap<string, unsigned int> getInterfaceInfo() { return m_ifacePatch; }

    /*!
    * \brief Patches a given device to the given DMXDevicePatch.
    * 
    * At some point this should get nicer and do some stuff automatically for you
    * (like looking up profiles on patch).
    * \param device Device to patch
    * \param patch Information on how the device should be patched.
    * \sa DMXDevicePatch, Device
    */
    void patchDevice(Device* device, DMXDevicePatch* patch);

    /*!
    * \brief Alternate patch function which just specifies an ID in a string.
    *
    * This class actually only needs the Device id property to handle mapping
    * devices to DMX addresses.
    * \param id Device id to patch
    * \param patch Information on how the device should be patched.
    * \sa DMXDevicePatch, Device
    */
    void patchDevice(string id, DMXDevicePatch* patch);

    /*!
    * \brief Adds a device map to the Patch's database of mappings.
    *
    * This function will REPLACE a map that already exists.
    * \param id Device type id
    * \param deviceMap Mapping of parameters to DMX addresses.
    */
    void addDeviceMap(string id, map<string, patchData> deviceMap);

    /*!
    * \brief Adds/modifies a parameter to/in a deviceMap.
    *
    * \param mapId Device map ID
    * \param paramId Parameter to modify
    * \param address New address for the parameter
    * \param type Conversion function to use for the parameter
    */
    void addParameter(string mapId, string paramId, unsigned int address, conversionType type);

    /*!
    * \brief Debug function that prints out all DMX values for all universes in the patch.
    */
    void dumpUniverses();

    /*!
    * \brief Debug funtion that prints out all DMX values for a single universe.\
    * \param universe Universe to inspect
    */
    void dumpUniverse(unsigned int universe);

    /*!
    * \brief Directly modifies the DMX data in the specified universe. 
    *
    * Pushes the data to the proper interface after updating. Note that if the update loop is active,
    * this probably won't do much of anything as the manual values will get overwritten
    * by the update loop. To get around this, initialize the rig but don't call Rig::run().
    * Must give an entire universe to this function. If you don't provide the entire universe,
    * the function returns false and doesn't do any updates.
    * \param universe Universe number to set data for
    * \param univData Data to set the universe to
    * \return True on success, false on failure
    */
    bool setRawData(unsigned int universe, vector<unsigned char> univData);

  private:
    /*!
    * \brief Loads data from a parsed JSON object
    * \param data JSON data to load
    */
    void loadJSON(const JSONNode data);

    /*!
    * \brief Loads the device maps from a JSON node
    * \param data JSON node containing the Device Map data
    */
    void loadDeviceMaps(const JSONNode data);

    /*!
    * \brief Converts a device map in the m_deviceMaps object into a JSON object.
    * \param id Device Map id to convert
    * \param data Map data to put in the JSONNode
    */
    JSONNode deviceMapToJSON(string id, map<string, patchData> data);

    /*!
    * \brief Converts a converstionType to a string
    * \param t Type to convert
    * \return String representation of the conversion type
    */
    string conversionTypeToString(conversionType t);

    /*!
    * \brief Stores the state of the DMX universes.
    *
    * Note that DMX Universe 1 is index 0 here due to one-indexing.
    */
    vector<vector<unsigned char> > m_universes;

    /*!
    * \brief Maps interface id to universe number (zero-indexed)
    *
    * An interface can be mapped to multiple universes, since some devices have
    * more than one output.
    */
    multimap<string, unsigned int> m_ifacePatch;

    /*!
    * \brief DMX Interfaces controlled by this patch
    *
    * Maps interface ids to interface. This is a unique mapping.
    * \sa DMXInterface
    */
    map<string, DMXInterface*> m_interfaces;

    /*!
    * \brief Maps devices to DMX outputs.
    *
    * This class will free DevicePatch objects given to it on destruction.
    * \sa DMXDevicePatch
    */
    map<string, DMXDevicePatch*> m_patch;

    /*!
    * \brief Stores information about device maps, which can be reused across
    * devices. Key is the device map name.
    */
    map<string, map<string, patchData> > m_deviceMaps;
  };
}

#endif
