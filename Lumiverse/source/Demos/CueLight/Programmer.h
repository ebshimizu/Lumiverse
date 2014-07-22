#ifndef _PROGRAMMER_H_
#define _PROGRAMMER_H_

#pragma once

#include "Rig.h"
#include "types/LumiverseTypeUtils.h"
#include "Cue.h"

namespace Lumiverse {

/*!
\brief A Programmer is a special Layer-like object used to manipulate specific devices for cues.

A Programmer object stores a specific state of a Rig. It tracks devices that have
been directly modified (captured) and is able to store those into a Cue when requested.
Programmer objects do not contain Patches and cannot start an update loop.

Note that this class never returns a DeviceSet, as those are attached to the Rig that
generated it. Modifying the devices in a DeviceSet will modify the Rig, not a Programmer
that might return them.
*/
class Programmer
{
public:
  /*!
  \brief Programmers must be initialized from Rigs to ensure that they
  contain the same Devices.
  \param rig Rig to create a Programmer for.
  */
  Programmer(Rig* rig);

  /*!
  \brief Destroys a programmer object.
  */
  ~Programmer();

  /*!
  \brief Sets the selected device's parameter to the given value

  This function is the primary way to modify devices in a programmer.
  This function will mark the changed devices as captured and change the parameter.
  \param selection Selected devices
  \param param Parameter to modify
  \param val Value to set the parameter to
  */
  void setParam(DeviceSet selection, string param, LumiverseType* val);

  /*!
  \brief Sets the selected devices's parameter to the given value

  Selection is done with a query string in this instance. Queries are based
  off of the Rig so that parameter related queries hit the right data.
  \param selection Query string
  \param param Param name
  \param val Value to set the parameter to.
  \sa DeviceSet::query
  */
  void setParam(string selection, string param, LumiverseType* val);

  // --------------------------------------------------------------------------
  // Convenience set functions
  // The following functions are pretty much identical to the setParam
  // functions in DeviceSet.

  void setParam(DeviceSet selection, string param, float val);
  void setParam(DeviceSet selection, string param, string val, float val2 = -1.0f);
  void setParam(DeviceSet selection, string param, string channel, double val);
  void setParam(DeviceSet selection, string param, double x, double y, double weight = 1.0);
  void setColorRGB(DeviceSet selection, string param, double r, double g, double b, double weight = 1.0, RGBColorSpace cs = sRGB);
  void setColorRGBRaw(DeviceSet selection, string param, double r, double g, double b, double weight = 1.0);

  void setParam(string selection, string param, float val);
  void setParam(string selection, string param, string val, float val2 = -1.0f);
  void setParam(string selection, string param, string channel, double val);
  void setParam(string selection, string param, double x, double y, double weight = 1.0);
  void setColorRGB(string selection, string param, double r, double g, double b, double weight = 1.0, RGBColorSpace cs = sRGB);
  void setColorRGBRaw(string selection, string param, double r, double g, double b, double weight = 1.0);

  /*!
  \brief Shorthand to access a device by ID.

  If you use this function, the device specified will automatically be added
  to the captured devices.
  \param id Device ID
  \return Pointer to the specified device. Nullptr otherwise.
  */
  Device* operator[](string id);

  /*!
  \brief Returns a device from the Programmer.

  \param id Device ID
  \return Pointer to specified device. nullptr if device does not exist.
  */
  Device* getDevice(string id);

  /*!
  \brief Clears the list of captured devices but does not reset the state of the devices.
  */
  void clearCaptured();

  /*!
  \brief Resets the state of the layer. Does not clear the captured devices.
  */
  void reset();

  /*!
  \brief Clears the list of captured devices and resets the state of the devices.
  */
  void clearAndReset();

  /*!
  \brief Returns the set of captured devices.
  */
  map<string, Device*> getCapturedDevices();

  /*!
  \brief Writes the programmer's captured channels on top of the given device state

  Blend in this case means overwrite. Given a map of Devices by ID, this function will
  write the current state of the captured devices into the state map.
  */
  void blend(map<string, Device*> state);

  /*!
  \brief Gets the set of the Devices the Programmer has.
  \return Reference to the set of Devices managed by the programmer.
  */
  inline const map<string, Device *>& getDevices() { return m_devices; }

  /*!
  \brief Creates a cue from the programmer's state.

  \param time Cue time in seconds. Default is 3.
  */
  Cue getCue(float time = 3);

  /*!
  \brief Captures the selected devices and copies their current values from the rig.

  Any existing captured devices will be added to by this function.
  \param DeviceSet devices to capture.
  */
  void captureFromRig(DeviceSet devices);

private:
  /*!
  \brief Set of devices managed by this Programmer
  
  Note that these are copies of the devices contained in the Rig.
  */
  map<string, Device*> m_devices;

  /*!
  \brief Set of captured devices in the Programmer.
  */
  DeviceSet captured;

  /*!
  \brief The Rig this programmer is storing a state for.
  */
  Rig* m_rig;

};

}

#endif