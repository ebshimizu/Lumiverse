/*! \file DynamicDeviceSet.h
* \brief A set of devices described by a query string.
*/
#ifndef _DYNAMICDEVICESET_H_
#define _DYNAMICDEVICESET_H_

#pragma once

#include <sstream>
#include <set>
#include <regex>
#include <functional>

#include "Logger.h"
#include "Device.h"
#include "Rig.h"
#include "DeviceSet.h"

namespace Lumiverse {
  class Rig;
  class LumiverseType;

  /*!
  * \brief A DynamicDeviceSet is a set of devices specified by a query string.
  *
  * DynamicDeviceSets are constructed with the standard query syntax 
  * (https://github.com/ebshimizu/Lumiverse/wiki/Query-Syntax-Notes) and
  * each time they're accessed, the set of devices matching the query will be
  * selected for use.
  * Note that this class is really a wrapper around DeviceSets and may be slow for
  * repeated use of large queries.
  * \sa Device, DeviceSet
  */
  class DynamicDeviceSet
  {
  public:
    /*!
    \brief Default constructor
    
    Like the default constructor for DeviceSet, this isn't particularly useful.
    */
    DynamicDeviceSet() : m_query("") { };

    /*!
    * \brief Constructs a DynamicDeviceSet
    *
    * Similar to a copy constructor.
    * \param rig Pointer to a Rig to get devices from.
    * \param query Query string to associate with this DynamicDeviceSet
    * \sa DeviceSet(Rig*), Device, Rig, DeviceSet
    */
    DynamicDeviceSet(Rig* rig, string query);

    /*!
    \brief Constructs a DynamicDeviceSet from a JSON Node.
    \param rig Pointer to the rig to get devices from.
    \param data JSON data to load from.
    */
    DynamicDeviceSet(Rig* rig, JSONNode data);

    /*!
    * \brief Copy a DynamicDeviceSet
    * 
    * \param dc DynamicDeviceSet to copy data from
    */
    DynamicDeviceSet(const DynamicDeviceSet& dc);

    /*!
    * \brief Destructor for the DeviceSet
    */
    ~DynamicDeviceSet();

    /*!
    * \brief Override for steam output
    *
    * Converts DynamicDeviceSet to a string representation and outputs it to a stream.
    * \return ostream object containing the DynamicDeviceSet string appended to it.
    * \sa info()
    */
    std::ostream & operator<< (std::ostream &str) {
      str << info();
      return str;
    };

    /*!
    \brief Gets a DeviceSet containing the devices that match the query at
    the time this function is called.
    */
    DeviceSet getDeviceSet();

    /*!
    \brief Sets the query string of this DynamicDeviceSet.
    \param query New query string to use.
    */
    void setQuery(string query);

    /*!
    \brief Gets the query string
    */
    string getQuery();


    /*!
    \brief Returns true if the query string is empty. 
    */
    bool isQueryNull();

    /*!
    * \brief Resets all the parameters in each Device in the device set
    * \sa Device::reset()
    */
    void reset();

    // These must mirror the device setparam functions.
    
    /*!
    * \brief Sets the value of a LumiverseFloat parameter on every device in the group
    * 
    * This function will only set the value of an existing parameter. If the
    * parameter doesn't exist, it will not be created by this function.
    * \param param Parameter to modify
    * \param val Value of the parameter
    * \sa Device::setParam(string, float)
    */
    void setParam(string param, float val);

    /*!
    * \brief Sets the value of a LumiverseEnum parameter on every device in the group
    *
    * This function will only set the value of an existing parameter. If the
    * parameter doesn't exist, it will not be created by this function.
    * \param param Parameter to modify
    * \param val Value of the enumeration
    * \param val2 The tweak value of the LumiverseEnum
    * \sa Device::setParam(string, string, float)
    */
    void setParam(string param, string val, float val2 = -1.0f);

    /*! \brief Sets the value of a LumiverseColor parameter
    *
    * LumiverseColors present a bit of a challenge for group selections.
    * Every light has differen color representations, so not every function
    * will work for every group. This function will call the corresponding
    * set param function for each device, but not every device may react to it.
    * \param param Parameter name
    * \param channel Color channel name
    * \param val Value of the color channel
    * \sa LumiverseColor, LumiverseType
    */
    void setParam(string param, string channel, double val);

    /*!
    \brief Sets the value of a LumiverseEnum parameter

    \sa Device::setParam()
    */
    void setParam(string param, string val, float val2, LumiverseEnum::Mode mode, LumiverseEnum::InterpolationMode interpMode);

    /*! \brief Sets the value of a LumiverseColor parameter using LumiverseColor::setxy()
    *
    * x and y are chromaticity coordinates in the xyY color space.
    * Ideally this function would be the unifying function for all devices in a Rig.
    * However, it only works if you know the XYZ values for each color of LED in a light,
    * and it doesn't work with CMY fixtures at the moment.
    * \param param Parameter name.
    * \param x x coordinate
    * \param y y coordinate
    * \sa LumiverseColor, LumiverseType, setParam(string, string, double)
    */
    void setParam(string param, double x, double y, double weight = 1.0);

    /*! \brief Sets the value of a LumiverseColor parameter
    *
    * Not gonna work terribly well if you're mixing fixtures that don't have RGB for
    * some reason in your selection.
    * Proxy for LumiverseColor::setRGBRaw().
    * \sa LumiverseColor::setRGBRaw(), LumiverseColor, LumiverseType
    */
    void setColorRGBRaw(string param, double r, double g, double b, double weight = 1.0);

    /*! \brief Sets the value of a LumiverseColor parameter
    *
    * Proxy for LumiverseColor::setRGB().
    * \sa LumiverseColor::setRGB(), LumiverseColor, LumiverseType
    */
    void setColorRGB(string param, double r, double g, double b, double weight = 1.0, RGBColorSpace cs = sRGB);

    /*!
    * \brief Gets the devices managed by this set.
    * 
    * \return Set of Device* contained by the DynamicDeviceSet
    */
    inline const set<Device *>& getDevices() { return m_rig->query(m_query).getDevices(); }

    /*!
    * \brief Gets a copy of the list of the IDs contained by this DynamicDeviceSet
    */
    vector<string> getIds();

    /*!
    * \brief Gets a copy of the set of all the parameters used by devices in this set.
    *
    * Set means no duplicates
    */
    set<string> getAllParams();

    /*!
    * \brief Gets a copy of the set of all the metadata keys used by devices in this set.
    */
    set<string> getAllMetadata();

    /*!
    \brief Gets a copy of the set of all metadata values used by a particular key.
    */
    set<string> getAllMetadataForKey(string key);

    /*!
    * \brief Returns a string containing info about the DynamicDeviceSet.
    *
    * \return DynamicDeviceSet as a string. String contains the number of devices contained
    * by the set and the IDs of each device in the set.
    */
    string info();

    /*!
    * \brief Returns the number of devices in the DynamicDeviceSet
    * \return Number of devices in the set.
    */
    inline size_t size() { return m_rig->query(m_query).size(); }

    /*!
    \brief Returns true if the device sets have the same number of devices
    and contain the same set of IDs.

    Note that even if this returns true, the device sets could refer
    to different objects, as they store pointers to the devices they contain.
    This function is currently very slow.
    */
    bool hasSameIds(DynamicDeviceSet& devices);

    /*!
    \brief Returns true if the device sets have the same number of devices and
    point to the same Devices. 

    This actually just checks to see if the pointers in the set of devices
    managed by the device set are pointing to the same things.
    */
    bool hasSameDevices(DynamicDeviceSet& devices);

    /*!
    \brief Checks to see if a device is in the device set.

    This version of the contains function takes a pointer and checks to see if
    the exact same device is in the set. This function is faster than contains(string).
    */
    bool contains(Device* d);

    /*!
    \brief Checks to see if a device is in the device set.

    This version of the contains function takes a device ID and checks if
    a device with the same id is in the set.
    */
    bool contains(string id);

    /*!
    \brief Saves the device set as a JSON object with the given name.

    This serializes to a list of IDs that can be used to reconstruct the
    DynamicDeviceSet if it's given the same Rig as it currently has.
    \param name Node name.
    */
    JSONNode toJSON(string name);

  private:
    /*!
    \brief Query string
    */
    string m_query;

    /*!
    * \brief Pointer to the rig for accessing indexes and devices
    */
    Rig* m_rig;
  };
}

#endif