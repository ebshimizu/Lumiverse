#ifndef _CUE_H_
#define _CUE_H_

#pragma once

#include <LumiverseCore.h>
#include <memory>

#ifdef _MSC_VER
#pragma warning(disable : 4503) // Debug symbol name too long for MSVS, pretty sure it's line 172
#endif

namespace Lumiverse {

/*!
\brief A Keyframe stores the value of a parameter at the specified time.
 
Keyframes can be set to use the previous cue value in a transition. This is useful in the
event that you want to run timlines back to back that pick up automatically from where
the last timeline left off, so if you update any cue in the sequence, the changes propagate correctly.
For this to be used effectively, the cue transition time is recommended to be 0.
*/
struct Keyframe {
  /*! \brief Time at which this keyframe is located. t=0 is start of timeline. */
  float t;

  /*!
  \brief Value of the keyframe at time t.
  */
  shared_ptr<Lumiverse::LumiverseType> val;

  /*!
  \brief If true, the value of this keyframe will be pulled from the previous cue in the transition.
  
  Note that the keyframe will still have a value, but it won't be used unless there is no previous cue.
  */
  bool usePreviousValue;

  // Planned interpolation mode selection here. Additional parameters probably needed
  // once this thing gets activated
  // enum interpMode

  bool operator<(Keyframe other) const {
    return t < other.t;
  }

  /*! \brief Empty constructor */
  Keyframe() { }

  /*!
  \brief Constructor with all values filled in.
  \param time Keyframe temporal location
  \param v Value at specified time
  \param uct Use Cue Timing (see useCueTiming member variable)
  */
  Keyframe(float time, shared_ptr<Lumiverse::LumiverseType> v, bool upv) :
    t(time), val(v), usePreviousValue(upv) { }

  /*! \brief Creates a keyframe from a JSON node. */
  Keyframe(JSONNode node);
};

/*!
\brief A cue stores data for a particular look (called a cue)

In the traditional theatrical sense, cues are preset configurations of lighting device parameters.
Cues are placed into cue lists and are then transitioned between with a simple cross fade
when a user presses a button (typically labeled "GO").
 
Cues in Lumiverse are timelines, where each parameter can be assigned a 
value at an arbitrary time. Cue transitions are handled by doing an linear interpolation between
the last keyframe of the previous cue, and the first keyframe of the current cue.
 
There are two ways to program Lumiverse Cues. You can use them as you would a normal theatrical
lighting board, with each cue being a static snapshot, or you can program them as animation timelines.
 
Since Lumiverse Cues are essentially animation timelines, transitioning between them in the
theatrical sense is a little more complicated than normal. The timing rules for two cues A and B
are as follows:
- Upfade time acts on a parameter that is increasing from cue A to cue B.
- Downfade time acts on a parameter that is decreasing from cue A to cue B.
- Delay indicates that the cue will wait for the specified number of seconds before transitioning.
- When transitioning from an aribtrary cue A to a cue B, cue B's timing will be used.
  So if cue B has an upfade time of 3, and a downfade time of 5, the transition will take 
  5 seconds total.
- The time it takes for the transition to complete is called the transition time.
- If a Cue has more than one Keyframe for a parameter, the end of the Cue is the time of the
  last keyframe over all parameters. For example, if cue B has upfade and downfade set to 3, and 
  a parameter with a keyframe set at time 10, cue B will end after 13s.
- If a Cue has keyframes set after time 0, they will begin execution once the transition for the
  parameter has completed. Note that if you have different up and down fade times, the beginning of
  cue keyframe execution will be different for each affected parameter. It is recommended for
  complex transitions to not use a transition time and instead set the first keyframe in the Cue
  to use the previous Cue values and manually handle the transition.
 
Lumiverse Cues can also have various triggers that activate at a specified time.
By default, a Cue provides a trigger at the beginning of a cue transition and end of the cue.
*/
class Cue {
public:
  /*!
  \brief Classifies the cue based on properties of its keyframes.
  */
  enum Type {
    SCENE,      /*!< Cue only has keyframes at time 0 and no keyframes use data from a previous cue. */
    STANDALONE, /*!< Cue has multiple keyframes beyond time 0. Cue also has a total transition time of 0 and no data comes from previous cues. */
    HYBRID,     /*!< Using this cue type is not recommended. Cue is Standalone but has a non-zero transition time. */
    LINKED,     /*!< Keyframes at time 0 use data from the previous cue, and the transition time is 0. */
    PARTIAL_LINK, /*!< Some Keyframes at time 0 use data from the previous cue, and the transition time is 0. */
    OTHER,      /*!< Cue cannot be classified as one of the standard options. */
    INVALID     /*!< Invalid type. This value is used internally to signal a refresh of the type of the cue. */
  };

  typedef map<string, map<string, shared_ptr<Lumiverse::LumiverseType> > > changedParams;

  /*!
  \brief Makes a blank cue.
  */
  Cue() : m_upfade(3.0f), m_downfade(3.0f) { }

  /*!
  \brief Constructs a cue from a rig. Default time is 3.

  This constructor will make a cue by pulling all of the values from the Rig
  and storing them in the cue. It essentially "takes a picture" of the rig for use later.
  \param rig Rig to create the cue from.
  */
  Cue(Rig* rig);

  /*!
  \brief Creates a cue from the current state of the rig with the given time.
  \param time Fade time for the cue
  */
  Cue(Rig* rig, float time);

  /*!
  \brief Creates a cue from a set of devices.
  
  Generally this gets called by Layers or by the Programmer when creating a cue.
  \param devices Map of devices to store. 
  \param time Default cue timing to use
  */
  Cue(map<string, Device*> devices, float up, float down, float delay);

  // Creates a cue with different up and down fades.
  Cue(Rig* rig, float up, float down);

  // Creates a cue with different up and down fades, and a different delay
  Cue(Rig* rig, float up, float down, float delay);

  /*! \brief Creates a cue from a JSON node. */
  Cue(JSONNode node);

  /*!
  \brief Copy a cue.
  */
  Cue(const Cue& other);

  // Destructor
  ~Cue();

  /*!
  \brief Does a deep copy of the cue data.
  */
  void operator=(const Cue& other);

  // Modifiers

  /*!
  \brief Updates the changes between the rig and this cue.
  
  Tracking happens at the cue list level
  Returns a mapping of device id -> changed parameter and old value.
  Note that if you have internal cues, this will try to track changes through to those keyframes.
  \param rig Rig that we're looking at for the update
  */
  changedParams update(Rig* rig);

  // Slightly different arguments, acts the same as update(Rig*)
  changedParams update(map<string, Device*> devices);

  // Only updates the devices with IDs in the changedParams.
  // Will remove IDs in oldVals if parameters don't match the older values, which
  // is when you'd stop tracking the changes through.
  void trackedUpdate(changedParams& oldVals, Rig* rig);

  /*!
  \brief Sets the delay for a cue
  
  The delay tells the cue how long to wait before actually animating anything.
  \param delay Delay in seconds.
  */
  void setDelay(float delay);

  /*!
  \brief Sets the upfade and downfade to the specified value.
  \param time Upfade/downfade in seconds.
  */
  void setTime(float time);

  /*!
  \brief Sets a time with an up and down fade that are different.
  \param up Upfade in seconds
  \param down Downfade in seconds
  */
  void setTime(float up, float down);

  // Keyframe modifiers
  // Does a raw insert of a single parameter's data into a keyframe
  inline void insertKeyframe(string id, string param, Lumiverse::LumiverseType* data, float time, bool uct = true);

  // Inserts a new keyframe for all selected devices and all parameters taken from the
  // current state of the rig.
  // Set uct to false if you don't want to use cue timing if the inserted point is at the end of a cue
  // Overwrites old keyframes if they exist.
  void insertKeyframe(float time, DeviceSet devices, bool uct = true);
  
  // Raw delete of keyframe given device id, parameter name, and time
  void deleteKeyframe(string id, string param, float time);
  
  // Deletes a keyframe at the given time for the given devices.
  void deleteKeyframe(float time, DeviceSet devices);

  // Not implemented yet but planned
  // If ripple = true, will move the next cue to the location of the deleted cue
  // and subtract time on future cues accordingly.
  // void deleteKeyframe(float time, DeviceSet devices, bool ripple = false);

  // Returns the cue data stored in this cue.
  map<string, map<string, set<Keyframe> > >& getCueData() { return m_cueData; }

  // Gets the upfade
  float getUpfade() { return m_upfade; }

  // Gets the downfade
  float getDownfade() { return m_downfade; }

  /*! \brief Gets the delay from a cue. */
  float getDelay() { return m_delay; }

  // Returns the cue data for a device's parameter
  set<Keyframe>& getParamData(string deviceId, string param) { return m_cueData[deviceId][param]; }

  /*! \brief Returns the JSON representation of the cue. */
  JSONNode toJSON();

  /*!
  \brief Returns the length of the cue.
  
  If the length hasn't been updated recently, this will calculate it.
  Otherwise it'll return the m_length value calculated at an earlier time.
  */
  float getLength();
  
  /*!
  \brief Returns the total transition time of the cue.
   
  \return `max(upfade, downfade) + delay`
  */
  float getTransitionTime() { return max(m_upfade, m_downfade) + m_delay; }

  /*!
  \brief Returns a one word description of the keyframe characteristics

  Returns one of three values at the moment:
  Standalone - Cue contains no null keyframes (does not cross fade between cues)
  Hybrid - Cue contains null keyframes (some parameters cross fade between cues)
  Linked - All parameters have null keyframes (all parameters cross fade between cues)
  */
  Type getType();

  /*!
  \brief Returns a copy of the first keyframe for the specified device and parameter.
  */
  Keyframe getFirstKeyframe(string device, string param);

  /*!
  \brief Returns a copy of the last keyframe for the specified device and parameter
  */
  Keyframe getLastKeyframe(string device, string param);
  
  /*!
  \brief Gets the value of the cue at the specified time including the transition time.
   
  Note that this function includes the transition time in its index. So if you have a manual
  keyframe at time 0, and a transition time of 5, the manual keyframe value will be found at
  `t = 5`.
  \param previousCue The cue to draw values from if a keyframe has no data. Typically this is the previous cue in a list.
  \param device Device id
  \param param Parameter name
  \param t Time to access the cue value.
  */
  shared_ptr<LumiverseType> getValueAtTime(Cue* previousCue, string device, string param, float t);

  /*!
  \brief Gets the value of the cue at the specified time excluding the transition time.

  Note that this function DOES NOT include the transition time in its index.
  This function will also skip uneven or manual transition times added by the user and start at time 0.
  \param previousCue The cue to draw values from if a Keyframe has no data. Typically this is the previous cue in a list.
  \param device Device id
  \param param Parameter name
  \param t Time to access the cue value.
  */
  shared_ptr<LumiverseType> getValueAtCueTime(Cue* previousCue, string device, string param, float t);

  /*!
  \brief Indicates if a parameter should continue to be animated at the given time.

  Parameters with function driven keyframes at the end should continue to be updated indefinitely.
  */
  bool paramIsActive(Cue* previousCue, string device, string param, float t);

private:
  // Upfade time
  float m_upfade;

  // Downfade time
  float m_downfade;

  // Delay before doing any fades, default timing.
  float m_delay;

  /*! \brief Stores the length of the cue in seconds. */
  float m_length;

  /*! \brief Indicates if the length stored in m_length is up to date. */
  bool m_lengthIsUpdated;

  /*!
  \brief Cue type.
  
  If equal to "" then getType() will recheck the type of the cue;
  */
  Type m_type;

  /*!
  \brief Data for this particular cue.
  
  Stored in a map from ID -> parameter -> set of keyframes in ascending order (t=0 first)
  It's pretty much the device without the metadata.
  */
  map<string, map<string, set<Keyframe> > > m_cueData;

  // Gets the parameters for the device and returns them in a map
  map<string, set<Keyframe> > getParams(Device* d);

  // Updates the parameters for a device in the cue.
  // If a parameter changes, returns the name of the param and the
  // old value of the param.
  void updateParams(Device* d, map<string, shared_ptr<Lumiverse::LumiverseType> >& changed);
  
  // Reserved for future use.
  // m_follow - cue follow time (time to wait before automatically taking the next cue)
};
}
#endif