#ifndef _DEVICE_H_
#define _DEVICE_H_

/*! \file Device.h
* \brief Represents a physical lighting Device in Lumiverse
*/
#pragma once

#include <map>
#include <vector>
#include <string>
#include <memory>
#include <sstream>

#include "LumiverseCoreConfig.h"
#include "Logger.h"
#include "LumiverseType.h"
#include "types/LumiverseFloat.h"
#include "types/LumiverseEnum.h"
#include "types/LumiverseColor.h"
#include "types/LumiverseTypeUtils.h"
#include "lib/libjson/libjson.h"
#include "lib/Eigen/Dense"
using namespace std;

namespace Lumiverse {
 
  /*!
  * \brief A Device in Lumiverse maintains information about a lighting device.
  * 
  * This class is meant to hold information about different parameters in
  * a Lumiverse friendly way. Conversion to network values happens in a
  * different class to separate the abstract representation of a device
  * from the practical network control details.
  */
  class Device
  {
  public:
    /*!
    * \brief Default constructor. Every device needs an id, channel, and type.
    *
    * May in the future pull default parameter map from a database of known
    * fixture types.
    * \param id Unique identifier for the device.
    * \param channel Channel number for the device. Multiple devices can be part of the same channel.
    * \param type String identifying the type of fixture ("Source Four 26deg" for example).
    * \sa Device(string, const JSONNode), ~Device()
    */
    Device(string id, unsigned int channel, string type);

    /*!
    * \brief Constructs a device given a property formatted JSONNode
    *
    * Primarily used to load data from a Lumiverse Rig file.
    * \param id Unique identifier for the device.
    * \param data JSONNode containing the device information.
    * \sa Rig, Device(string, unsigned int, string), ~Device()
    */
    Device(string id, const JSONNode data);

    /*!
    * \brief Copies a Device
    */
    Device(const Device& other);

    /*!
    * \brief Copies a Device
    */
    Device(Device* other);

    /*! 
    * \brief Destroys a device.
    */
    ~Device();

    /*!
    * \brief Override for stream operator
    * 
    * Dumps the output of Device::toString() into the stream.
    * \sa toString()
    */
    std::ostream & operator<< (std::ostream &str) {
      str << toString();
      return str;
    };

    /*! 
    * \brief Accessor for Device id
    *
    * \return The Device's id
    */
    inline string getId() { return m_id; }

    /*!
    * \brief Accessor for channel
    *
    * \return The Device's channel number
    */
    inline unsigned int getChannel() { return m_channel; }
    
    /*!
    * \brief Assigns channel number
    *
    * \param newChan New channel number for the device
    */
    inline void setChannel(unsigned int newChan) { m_channel = newChan; }

    /*!
    * \brief Accessor for Device type
    *
    * \return The Device's type as a string
    */
    inline string getType() { return m_type; }

    /*!
    * \brief Assigns Device type
    *
    * \param newType New type for the device.
    */
    inline void setType(string newType) { m_type = newType; }

    /*!
    * \brief Gets the value of a float parameter
    *
    * This function only returns true if the parameter exists, and if the parameter
    * is a LumiverseFloat. If successful, the value will be copied to val.
    * \param param Parameter name
    * \param[out] val Output value of the parameter if it exists and is a float.
    * \return False if no parameter with the given name exists,
    * true with the parameter value in val if successful.
    * \sa getParam(string), LumiverseType, LumiverseFloat
    */
    bool getParam(string param, float& val);

    /*!
    * \brief Returns a pointer to the raw LumiverseType data associated with a parameter.
    *
    * This function gives you direct access to the object stored in the Device.
    * Modifying the data in the returned pointer will propagate throughout the Rig.
    * \param param Parameter name
    * \return Pointer to LumiverseType object associated with the paramater.
    * `nullptr` if parameter does not exist in the device.
    * \sa getParam(string, float&), LumiverseType, LumiverseFloat
    */
    LumiverseType* getParam(string param);

    /*!
    * \brief Gets a pointer to a LumiverseColor parameter.
    *
    * Gives direct access to a color parameter. Probably the easiest way to
    * modify a color for a device.
    * \param param Parameter name. Defaults to "color" assuming that most people
    * will call the color parameter "color" in their devices.
    * \return Pointer to LumiverseColor object associated with parameter. `nullptr`
    * is returned if the parameter does not exist.
    * \sa LumiverseColor, LumiverseType
    */
    LumiverseColor* getColor(string param = "color");

    /*!
    * \brief Sets the value of a parameter.
    * 
    * Can set arbitrary data with this version of the function.
    * When you pass a LumiverseType* into a Device with this function, the memory is
    * now owned by the Device, which will attempt to free it when it's destroyed.
    * For this reason, you should not use heap-allocated variables with this function.
    * This function will create new parameters if the specified key doesn't exist.
    * \param param Parameter name
    * \param val object to assign to the parameter
    * \return False if the parameter does not exist prior to set. True otherwise.
    * \sa LumiverseType
    */
    bool setParam(string param, LumiverseType* val);

    /*!
    * \brief Sets the value of a LumiverseFloat or LumiverseOrientation parameter
    *
    * This function will not create a new parameter if the key doesn't exist.
    * Use of this function is reserved specifically for floating point valued parameters.
    * It is up to the caller to insure that the parameter is actually a LumiverseFloat,
    * however messing that up won't kill the program.
    * \param param Parameter name
    * \param val Value to assign to the parameter
    * \return true on success, false on failure.
    * \sa LumiverseType, LumiverseFloat, LumiverseOrientation
    */
    bool setParam(string param, float val);

    /*!
    * \brief Sets the value of a LumiverseEnum parameter
    *
    * This function will not create a new parameter if the key doesn't exist.
    * If val2 is not set, the tweak value isn't passed to the enumeration, allowing it to do default behavior.
    * \param param Parameter name
    * \param val Enumeration name
    * \param val2 The tweak value for the enumeration. See LumiverseEnum for details.
    * \return true on success, false on failure.
    * \sa LumiverseType, LumiverseEnum
    */
    bool setParam(string param, string val, float val2 = -1.0f);

    /*!
    \brief Fully specify the value of an enumeration.

    This function will not create a new parameter if the key doesn't exist.
    \return true on success, false on failure.
    \sa LumiverseEnum
    */
    bool setParam(string param, string val, float val2, LumiverseEnum::Mode mode, LumiverseEnum::InterpolationMode interpMode);

    /*! \brief Sets the value of a LumiverseColor parameter
    *
    * Does not create a new parameter if the key doesn't exist.
    * \param param Parameter name
    * \param channel Color channel name
    * \param val Value of the color channel
    * \return true on success, false on failure.
    * \sa LumiverseColor, LumiverseType
    */
    bool setParam(string param, string channel, double val);

    /*! \brief Sets the value of a LumiverseColor parameter using LumiverseColor::setxy() 
    *
    * Does not create a new parameter if the key doesn't exist.
    * x and y are chromaticity coordinates in the xyY color space.
    * \param param Parameter name.
    * \param x x coordinate
    * \param y y coordinate
    * \return true on success, false on failure
    * \sa LumiverseColor, LumiverseType, setParam(string, string, double)
    */
    bool setParam(string param, double x, double y, double weight = 1.0);

    /*! \brief Sets the value of a LumiverseColor parameter
    *
    * Does not create a new parameter if the key doesn't exist.
    * Proxy for LumiverseColor::setRGBRaw().
    * \return true on success, false on failure
    * \sa LumiverseColor::setRGBRaw(), LumiverseColor, LumiverseType
    */
    bool setColorRGBRaw(string param, double r, double g, double b, double weight = 1.0);

    /*! \brief Sets the value of a LumiverseColor parameter
    *
    * Proxy for LumiverseColor::setRGB().
    * \return False if parameter changed does not exist prior to set.
    * \sa LumiverseColor::setRGB(), LumiverseColor, LumiverseType
    */
    bool setColorRGB(string param, double r, double g, double b, double weight = 1.0, RGBColorSpace cs = sRGB);

    /*! \brief Sets the value of a LumiverseColor parameter
    *
    * Does not create a new parameter if they key doesn't exist.
    * Proxy for LumiverseColor::setColorChannel().
    * \param param Parameter name.
    * \param channel Channel name.
    * \param val Channel value.
    * \return true on success, false on failure
    * \sa LumiverseColor::setColorChannel(), LumiverseColor, LumiverseType
    */
    bool setColorChannel(string param, string channel, double val);

      
    // Will need additional overloads for each new type. Which kinda sucks.
      
    /*!
    * \brief Copies the data from source into target parameter.
    *
    * \param param Id of the target parameter
    * \param source Pointer to the data source
    */
    void copyParamByValue(string param, LumiverseType* source);
      
    /*! 
    * \brief Checks for the existance of a parameter
    *
    * Existance is defined by the parameter map containing a value for the key `param`. 
    * \param param Parameter name
    * \return True if the parameter exists.
    */
    bool paramExists(string param);

    /*!
    * \brief Get the number of parameters in the device.
    * \return Number of parameters in the device.
    */
    unsigned int numParams();

    /*!
    * \brief Get list of parameter names in the device.
    * \return List of parameters in the device.
    */
    vector<string> getParamNames();

    /*!
    * \brief Retrieve metatata value for a given key.
    *
    * \param key Metadata key
    * \param[out] val Value of the metadata field if it exists.
    * \return False if no key exists. True and the value in `val` if it does exist.
    */
    bool getMetadata(string key, string& val);

    /*!
    * \brief Sets the metadata value for a given key.
    * 
    * \param key Metadata key
    * \param val New value of the metadata field.
    * \return False if metadata key does not exist prior to set, true otherwise.
    */
    bool setMetadata(string key, string val);

    /*!
    * \brief Erases all values in the metadata fields
    *
    * Resets metadata values to "" but leaves the keys intact.
    */
    void clearMetadataValues();

    /*!
    * \brief Deletes the entire metadata map
    *
    * Empties everything in the metadata hash: keys and values. All gone.
    */
    void clearAllMetadata();

    /*!
    * \brief Get the number of metadata keys in the Device.
    */
    unsigned int numMetadataKeys();

    /*!
    * \brief Gets list of metadata keys the device currently has values for.
    */
    vector<string> getMetadataKeyNames();

    /*!
    * \brief Resets the values in the parameters to 0 (or equivalent default)
    * Defaults are defined in the implementations of LumiverseType
    * \sa LumiverseType
    */
    void reset();

    /*! 
    * \brief Converts the device data into a string.
    *
    * \return JSON representation of the device formatted into a string.
    */
    string toString();

    /*! 
    * \brief Converts the device to a JSONNode.
    * \return JSONNode representing the Device. Can be written to file.
    */
    JSONNode toJSON();

    /*!
    * \brief Gets the raw map of parameters to data
    * 
    * This function is intended to provide the collection of parameters for a calling
    * function to iterate though. You may modify the data contained by the map
    * in a calling function.
    * \return Reference to the map of parameter data.
    */
    map<string, LumiverseType*>& getRawParameters() { return m_parameters; }
      
    /** Indicates the function signature for parameter and metadata callbacks.
    Currently a device have to pass in "this" pointer. It seems to be other
    way to access fields inside Device class. This typedef makes it easier to
    change signature. */
    typedef function<void(Device*)> DeviceCallbackFunction;
    
    /*!
    * \brief Registers a callback function for parameter changed event.
    *
    * All registered functinos would be called when a parameter is changed
    * by Device::setParam and Device::reset function.
    * \param func The callback function.
    * \return The int id for the registered function.
    * \sa addMetadataChangedCallback(DeviceCallbackFunction func)
    */
    int addParameterChangedCallback(DeviceCallbackFunction func);
    
    /*!
    * \brief Registers a callback function for metadata changed event.
    *
    * All registered functinos would be called when a metadata is changed
    * by Device::setMetadata, Device::clearMetadataValues, Device::clearAllMetadata,
    * and Device::reset.
    * \param func The callback function.
    * \return The int id for the registered function.
    * \sa addParameterChangedCallback(DeviceCallbackFunction func)
    */
    int addMetadataChangedCallback(DeviceCallbackFunction func);

    /*!
    * \brief Deletes a registered callback for parameter change
    *
    * \param id The id returned when the callback is registered
    * \sa addParameterChangedCallback(DeviceCallbackFunction func)
    */
    void deleteParameterChangedCallback(int id);

    /*!
    * \brief Deletes a registered callback for metadata change
    *
    * \param id The id returned when the callback is registered
    * \sa addMetadataChangedCallback(DeviceCallbackFunction func)
    */
    void deleteMetadataChangedCallback(int id);

    /*!
    \brief Returns true if the device is identical to the given device.

    Identical means that the parameters are all the same, the metadata is
    all the same, and the other properties of the device are the same.
    */
    bool isIdentical(Device* d);
      
  private:
    /*! \brief Sets the id for the device
    *
    * Note that this is private because changing the unique ID after creation
    * can lead to a number of unintended side effects (DMX patch doesn't work,
    * Rig can't find the device, other indexes may fail). This is a little annoying,
    * but in the big scheme of devices and rigs, the ID shouldn't change after initial
    * creation. If the user needs a different display name, the metadata fields say hi.
    * In ACN terms, this would be the CID (if I remember this right), which is unique and
    * immutable for each device). Making this function public would require some additional
    * checks in the Rig to make sure the change propagates correctly.
    * \param newId New deivce id
    */
    void setId(string newId) { m_id = newId; }

    /*!
    * \brief Takes parsed JSON data and makes a device.
    * \param data JSON data to turn into a device.
    */
    void loadJSON(const JSONNode data);

    /*!
    * \brief Loads the parameters of the device from JSON data.
    * Helper for loadJSON
    * \param data JSON node containing the parameters of the device.
    * \sa loadJSON()
    */
    void loadParams(const JSONNode data);

    /*!
    * \brief Serializes the parameters into a JSON node
    * \return JSONNode representation of the parameters
    * \sa toJSON()
    */
    JSONNode parametersToJSON();

    /*!
    * \brief Serializes the metadata to a JSON node
    * \return JSONNode representation of the metadata
    * \sa toJSON()
    */
    JSONNode metadataToJSON();

    /*!
    * \brief Calls all the registered callbacks of parameter changed iterately.
    * \sa onMetadataChanged()
    */
    void onParameterChanged();
      
    /*!
    * \brief Calls all the registered callbacks of metadata changed iterately.
    * \sa onParameterChanged()
    */
    void onMetadataChanged();
      
    /*!
    * \brief Unique identifier for the device.
    *
    * Note that while you can use any characters you want in this, you really shouldn't
    * use special characters such as @#$%^=()[]/{} etc.
    */
    // TODO: This should be built in to the set ID function at some point
    // Uniqueness isn't quite enforceable at the device level.
    string m_id;

    /*!
    * \brief Channel number for the fixture. Does not have to be unique.
    */
    unsigned int m_channel;

    /*! 
    * \brief Device type name.
    * "Source Four ERS" for example.
    */
    string m_type;

    /*!
    * \brief Map for time-varying parameters.
    *
    * These parameters correspond to network-controllable functions of
    * the lighting fixtures. If you can't control it over DMX, Ethernet, or
    * other protocol, it's not a parameter.
    */
    // Type may change in the future as more specialized datatypes come up.
    map<string, LumiverseType*> m_parameters;

    /*!
    * \brief Map for program-side information.
    * 
    * This data can be anything you want. The core system uses it to add search
    * filters and automatic device grouping. Any sort of data can be stored in it,
    * assuming it can be serialized to a string.
    */
    map<string, string> m_metadata;
    
    /*!
    * \brief List of functions to run when a parameter is changed. Each function has an int id.
    *
    * Currently a device have to pass in "this" pointer. It seems to be other
    * way to access fields inside Device class. This function would inform
    * instances which need to respond to the update.
    */
    map<int, DeviceCallbackFunction> m_onParameterChangedFunctions;
      
    /*!
    * \brief List of functions to run when a metadata is changed. Each function has an int id.
    *
    * Currently a device have to pass in "this" pointer. It seems to be other
    * way to access fields inside Device class. This function would inform
    * instances which need to respond to the update.
    */
    map<int, DeviceCallbackFunction> m_onMetadataChangedFunctions;
  };
}

#endif