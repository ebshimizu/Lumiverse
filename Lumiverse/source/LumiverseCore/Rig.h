/*! \file Rig.h
* \brief The Rig contains information about the state of the lighting system.
*/
#ifndef _RIG_H_
#define _RIG_H_

#pragma once

#include <thread>
#include <chrono>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <set>
#include <functional>

#include "LumiverseCoreConfig.h"
#include "Patch.h"
#include "DMX/DMXPatch.h"
#include "Device.h"
#include "Logger.h"
#include "DeviceSet.h"
#include "lib/libjson/libjson.h"

#ifdef USE_ARNOLD
#include "lib/arnold/include/ai.h"
#include "Simulation/ArnoldPatch.h"
#include "Simulation/ArnoldAnimationPatch.h"
#endif

namespace Lumiverse {
  class DeviceSet;

  /*!
  * \brief The Rig contains information about the state of the lighting system.
  *
  * A Rig is responsible for maintaining a list of all devices in the system,
  * and for relaying changes to Lumiverse Device objects to the corresponding
  * network interface. Rigs send Devices to a Patch, which then converts the
  * Lumiverse data to a format that can be understood by the lights on the network.
  */
  // TODO: Right now if you change a device channel or id the rig doesn't
  // know about it. Need to add functions in the Rig that allow
  // this sort of change.
  class Rig
  {
    /*! \sa DeviceSet */
    friend class DeviceSet;

  public:
    /*!
    * \brief Makes an empty rig
    *
    * \sa Rig(string), init()
    */
    Rig();

    /*!
    * \brief Initialize Rig from JSON file
    *
    * This function will initialize Devices and Patches by calling functions in
    * the appropriate class.
    * \param Path to file
    * \sa Rig(), loadJSON(), loadDevices(), loadPatches(), init()
    */
    Rig(string filename);

    /*!
    * \brief Destroys and frees all objects in the rig.
    *
    * Once this is called all resources used by this class are free.
    * Do not attempt to access a Device from a destroyed Rig as the
    * Rig will destroy all objects that it manages.
    */
    ~Rig();

    /*!
    * \brief Initializes the rig.
    *
    * Called after all patching has been done and configuration settings
    * have been selected.
    * \sa Rig(), Rig(string)
    */
    void init();

    /*!
    * \brief Runs the update loop that sends updates to the network.
    *
    * Spawns a child thread managed by this class.
    * \sa m_updateLoop
    */
    void run();

    /*!
    * \brief Stops the update loop.
    *
    * Do this before shutting down the network or potential reconfiguring.
    * \sa m_updateLoop
    */
    void stop();

    /*!
    * \brief Loads a file into an existing rig
    *
    * All existing devices and patches will be deleted and replaced by
    * the contents of the specified file.
    * \return false if an error occurs, true if loaded successfully
    */
    bool load(string filename);

    /*!
    * \brief Adds a device to the Rig.
    *
    * Device memory is managed by the Rig. User should allocate memory,
    * and the Rig is responsible for freeing it at the end.
    * \param device Device to add to the Rig.
    */
    void addDevice(Device * device);

    /*!
    * \brief Gets a device from the rig.
    *
    * Return value can be modified to change
    * the state of the device in the rig.
    * \param id Device id.
    * \return Pointer to the requested Device. If the device doesn't exist in the rig,
    * a nullptr will be returned.
    */
    Device* getDevice(string id);

    /*!
    * \brief Removes the device with specified id from the rig. Also deletes it.
    *
    * This operation calls the device destructor too, so just be sure you actually
    * want to completely get rid of a device before calling this.
    * If you need to save the Device for some reason, get it with getDevice() first.
    * \sa getDevice()
    */
    void deleteDevice(string id);

    /*!
    * \brief Adds a Patch to the rig.
    *
    * User is responsible for allocating memory for the patch,
    * Rig is responsible for freeing an allocated patch.
    * Expects the patch to be configured before sending it to the rig.
    * Not that it can't be edited later, but the rig only knows what you tell it.
    * \param id Identifier for the Patch. Used to retrieve patches from the Rig.
    * \param patch Pointer to the patch.
    * \sa Patch
    */
    void addPatch(string id, Patch* patch);

    /*!
    * \brief Gets a patch from the rig.
    *
    * \return Pointer to the requested patch. Returns a nullptr if patch with specified ID doesn't exist.
    * \sa Patch
    */
    Patch* getPatch(string id);

    /*!
    * \brief Gets a patch from a rig and treats it as a DMXPatch.
    *
    * Use carefully. The rig does not guarantee that the requested patch is actually
    * a DMXPatch.
    * \return Pointer to the requested patch casted to a DMXPatch.\
    * \sa DMXPatch, Patch
    */
    DMXPatch* getPatchAsDMXPatch(string id) { return (DMXPatch*)getPatch(id); }

    /*!
    * \brief Deletes an entire patch from the rig.
    *
    * Memory is freed after delete. If you need to save it for some reason, get it with
    * getPatch() first.
    * \param Patch id
    * \sa getPatch()
    */
    void deletePatch(string id);

    /*!
    * \brief Sets the refresh rate for the update loop in cycles / second
    *
    * The DMX protocol is limited to 44Hz max, but you could run the loop faster.
    * \param rate Number of times the update loop should run per second.
    */
    void setRefreshRate(unsigned int rate);

    /*!
    * \brief Gets the refresh rate for the update loop.
    *
    * \return Refresh rate in cycles/second
    */
    unsigned int getRefreshRate() { return m_refreshRate; }

    /*!
    * \brief Shorthand for getDevice(string)
    *
    * \return Pointer to requested Device.
    * \sa getDevice()
    */
    Device* operator[](string id);

    /*!
    * \brief Gets a DeviceSet based on a query string
    *
    * This will probably replace operator[](string) at some point, but that point is not now.
    * \param q Query string
    * \return DeviceSet containing all devices matching the query string.
    * \sa DeviceSet, DeviceSet::select()
    */
    DeviceSet query(string q);

    /*!
    * \brief Shorthand for getChannel(unsigned int)
    *
    * \return DeviceSet containing all devices in the specified channel
    * \sa getChannel(unsigned int)
    */
    DeviceSet operator[](unsigned int channel);

    // Queries. Most everything starts with the creation of a DeviceSet.
    // Detailed filtering happens there, the Rig provides a few convenience functions
    // to get things started.

    /*!
    * \brief Returns a set consisting of all devices in the rig
    *
    * \return DeviceSet with all the Devices in it.
    */
    DeviceSet getAllDevices();

    /*!
    * \brief Gets all the devices in a channel
    * \param channel Channel to retrieve
    * \return DeviceSet containing the specified devices
    * \sa DeviceSet::add(unsigned int)
    */
    DeviceSet getChannel(unsigned int channel);

    /*!
    * \brief Gets a range of channels
    * \param lower First channel to get (inclusive)
    * \param upper Last channel to get (inclusive)
    * \return DeviceSet containing the specified devices
    * \sa DeviceSet::add(unsigned int, unsigned int)
    */
    DeviceSet getChannel(unsigned int lower, unsigned int upper);

    /*!
    * \brief Gets devices by metadata info.
    * \param key Metadata key
    * \param val Value to check
    * \param isEqual If true, returns all devices that have data equal to val.
    * If false, returns all devices that have data not equal to val.
    * \return DeviceSet containing the specified devices.
    * \sa DeviceSet::add(string, string, bool)
    */
    DeviceSet getDevices(string key, string val, bool isEqual);

    /*!
    * \brief Gets a set of all the parameters used by the devices.
    *
    * Returned parameter names are not guaranteed to be in any particular order.
    * Caller may choose to sort or arrange the parameter names as needed.
    * \return Set of strings. If a device uses a parameter in the rig, the name
    * of that parameter will be returned in this set.
    */
    set<string> getAllUsedParams();

    /*!
    * \brief Gets the raw list of devices.
    *
    * Users shouldn't modify the set of devices through this method,
    * but may read the data and modify device parameters.
    * \return Set of Devices maintained by this Rig
    */
    const set<Device *>& getDeviceRaw() { return m_devices; }

    /*!
    * \brief Writes the rig out to a JSON file
    *
    * \param filename Path to file
    * \param overwrite If the file specified by filename exists, the file will be
    * overwritten if this variable is set to `true`
    * \return True on success, false on failure.
    * \sa toJSON()
    */
    bool save(string filename, bool overwrite = false);

    /*!
    * \brief Gets the JSON data for the rig.
    *
    * \return JSONNode containing all Rig information
    */
    JSONNode toJSON();

    /*!
     * \brief Adds a function to the additional functions list.
     *
     * The added function will be updated before the patch is updated
     * as part of Rig::update(). The update loop will be stopped during
     * the modification of the function list.
     * \param pid ID to assign to the function
     * \param func Function to execute
     * \sa m_updateFunctions
     * \returns False if a function with specified pid already exists. True on success.
     */
    bool addFunction(int pid, function<void()> func);

    /*!
    * \brief Removes a function from the additional functions list.
    *
    * This function will stop the update loop while it modifies the function
    * list, and will automatically restart it.
    * \param pid The function number to remove.
    * \returns True if function removed, false otherwise.
    */
    bool removeFunction(int pid);

    /*!
    * \brief Pushes data over the network
    *
    * Actual transport is handled by the Patch objects in the Rig
    * \sa Patch
    */
    void update();

    /*!
    * \brief Get a simulation patch
    *
    * Checks against all the registered patches and returns the first simulation patch found.
    * \param type Type name to distinguish different Simulation Patch.
    * \return A pointer to a simulation patch (the current implementation is the ArnoldPatch).
    * \sa Patch
    */
    Patch *getSimulationPatch(string type);

    /*!
    * \brief Updates the parameters of the devices stored in the specified map.
    *
    * This function allows you to do mass updates of devices in a Rig.
    * This function will only update parameters not metadata
    * \param devices Map of device id -> Device* containing the data to update the rig with.
    */
    void setAllDevices(map<string, Device*> devices);

    /*!
    \brief Returns the number of devices in the Rig.
    */
    size_t getNumDevices() { return m_devices.size(); }

  private:
    /*!
    * \brief Loads the rig info from the parsed JSON data.
    * \param root JSONNode containing all Rig data.
    * \sa toJSON(), save(), loadDevices(), loadPatches()
    */
    void loadJSON(JSONNode root);

    /*!
    * \brief Loads the devices in the JSON file.
    * \param root JSONNode containing the Devices in the Rig.
    * \sa loadJSON()
    */
    void loadDevices(JSONNode root);

    /*!
    * \brief Load patches in the JSON file.
    * \param root JSONNode containing the Patches in the Rig.
    * \sa loadJSON()
    */
    void loadPatches(JSONNode root);

    /*!
    * \brief Empties all the data from the rig.
    */
    void reset();

    /*!
    * \brief Thread that runs the update loop.
    */
    thread* m_updateLoop;

    /*! \brief Indicates the status of the update loop.
    *
    * True if running.
    */
    bool m_running;

    /*!
    * \brief Sets the speed of the run loop in cycles/second. Default is 40.
    * \sa m_loopTime
    */
    unsigned int m_refreshRate;

    /*!
    * \brief Amount of time an update loop can take in s.
    *
    * Automatically calculated by the rig based on m_refreshRate.
    * \sa m_refreshRate
    */
    float m_loopTime;

    /*!
    * \brief Raw list of devices for sending to the Patch->update function.
    *
    * This is the core list of Device objects that the Rig maintains.
    * Indices are built off of this set as needed.
    * \sa Device
    */
    set<Device *> m_devices;

    /*!
    * \brief Maps Patch id to at Patch object
    *
    * The Patch id only matters to the Rig. The Patch doesn't really care what you call it.
    * \sa Patch
    */
    map<string, Patch *> m_patches;

    /*!
    * \brief Devices mapped by their device ID.
    */
    map<string, Device *> m_devicesById;

    /*! \brief Devices mapped by channel number. */
    multimap<unsigned int, Device *> m_devicesByChannel;

    /*!
    * \brief List of functions to run at the end of the update loop
    *
    * Allows a user to run an additional function during the update loop.
    * This allows for a pretty easy update loop if you're updating things
    * on a regular interval. Essentially the Rig's output loop becomes your
    * graphics draw loop.
    *
    * These functions must have a void() signature. I'm not entirely sure
    * if you can have one access internal Rig variables, but if you
    * have other sorts of things that need to run in a regularly timed loop,
    * you have the option to attach it to the Rig update loop.
    * These functions run before the patches are updated and could potentially
    * be used to inject values into the rig before the patch happens.
    */
    map<int, function<void()> > m_updateFunctions;

    // May have more indicies in the future, like mapping by channel number.
  };
}
#endif
