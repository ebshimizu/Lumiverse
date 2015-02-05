#ifndef _TIMELINE_H_
#define _TIMELINE_H_

#pragma once

#include "LumiverseCore.h"
#include "Keyframe.h"

namespace Lumiverse {
namespace ShowControl {

/*!
\brief A Timeline is a list of device parameter values at arbitrary times

Lumiverse Timelines should be familiar to anyone who's used a timeline based system before.
The one difference to highlight specifically is the addition of a new keyframe data type called
"Use Current State." When this keyframe value is present, the state of the layer the timeline is
being played on will be used as the "current state" whenever this keyframe is encountered. Because
of this, this keyframe is typically used only at the beginning of timelines.
*/
class Timeline {
public:
  /*!
  \brief Creates an empty timeline
  */
  Timeline();

  /*!
  \brief Load a Timeline from JSON data.

  \param data JSONNode containing a timeline.
  */
  Timeline(JSONNode data);

  /*!
  \brief Copies a timeline.
  */
  Timeline(const Timeline& other);

  void operator=(const Timeline& other);

  /*!
  \brief Deletes a timeline
  */
  ~Timeline();

  /*!
  \brief Gets the identifier used to refer to a device-parameter keyframe set.
  */
  string getTimelineKey(Device* d, string paramName);

  /*!
  \brief Gets the keyframe for a given identifier and time. Read-only.

  Note that if the keyframe doesn't exist, you will receive an invalid keyframe.
  */
  Keyframe getKeyframe(string identifier, size_t time);

  /*!
  \brief Gets the keyframes for a given device and time. Read-only.

  If a keyframe doesn't exist for the parameter, it will be empty. 
  */
  map<string, Keyframe> getKeyframes(Device* d, size_t time);

  /*!
  \brief Gets the keyframes for the entire timeline.
  */
  map<string, map<size_t, Keyframe> >& getAllKeyframes();

  /*!
  \brief Sets the value for the specified keyframe.

  This function will overwrite existing keyframes without warning.
  */
  void setKeyframe(string identifier, size_t time, LumiverseType* data, bool ucs = false);

  /*!
  \brief Sets the value of the keyframes for all parameters of the given device.
  */
  void setKeyframe(Device* d, size_t time, bool ucs = false);

  /*!
  \brief Stores a keyframe from a rig
  */
  void setKeyframe(Rig* rig, size_t time, bool ucs = false);

  /*!
  \brief Stores a keyframe for a group of selected devices
  */
  void setKeyframe(DeviceSet devices, size_t time, bool ucs = false);

  /*!
  \brief Deletes the keyframe with the specified identifier at the specified time.
  */
  void deleteKeyframe(string identifier, size_t time);

  /*!
  \brief Deletes the keyframes for the specified device at the specified time.
  */
  void deleteKeyframe(Device* d, size_t time);

  /*!
  \brief Deletes the keyframes for the specified devices at the specified time.
  */
  void deleteKeyframe(DeviceSet devices, size_t time);

  /*!
  \brief Adds a new event to the end of the timeline

  Id is needed for this to allow easy access to end events.
  \param id Identifier for the end event.
  \param e Event to add
  \return true on success, false if an event with the given ID alreay exists.
  */
  bool addEndEvent(string id, shared_ptr<Event> e);

  /*!
  \brief Deletes an end event

  \param id Identifier for the event to delete.
  */
  void deleteEndEvent(string id);

  /*!
  \brief Adds an Event to the Timeline

  \param time Time to execute the Event
  \param e Event to add.
  */
  bool addEvent(size_t time, shared_ptr<Event> e);

  /*!
  \brief Deletes Events from the Timeline

  If an ID is not specified, this function will delete all events at the specified time
  \param time Time to delete the events at
  \param id Optional identifier for the Event to delete.
  */
  void deleteEvent(size_t time, string id = "");

  /*!
  \brief Gets Events at the specified time.

  If an ID is specified, the vector will contain only the specified Event (if it exists).
  \param time Time to get Events
  \param id Optional identifier to get a specific Event.
  */
  vector<shared_ptr<Event> > getEvents(size_t time, string id = "");

  /*!
  \brief Gets an end Event.

  \param id Identifier for the desired ID.
  \return Specified Event if it exists. nullptr if it does not exist.
  */
  shared_ptr<Event> getEndEvent(string id);

  /*!
  \brief Returns the multimap containing the Events for this Timeline.
  */
  multimap<size_t, shared_ptr<Event> >& getAllEvents();

  /*!
  \brief Returns the end Events for this Timeline
  */
  map<string, shared_ptr<Event> >& getAllEndEvents();

  /*!
  \brief Returns the value of the specified parameter for the specified device at the specified time.

  \param identifier [deviceID]:[paramName] for the desired device and parameter
  \param time Time in milliseconds to get the value.
  \return A LumiverseType value for the specified time in the timeline.
  */
  shared_ptr<LumiverseType> getValueAtTime(string identifier, size_t time);

  /*!
  \brief Gets the length of the timeline based on stored keyframes.
  */
  size_t getLength();

  /*!
  \brief Gets the JSON representation of the timeline object.
  */
  JSONNode toJSON();

  /*!
  \brief Indicates if the Timeline has no more keyframes after the specified time.

  Note that in the event of nested timelines, this function will recursively call isDone().
  For a Timeline to be completely done, every sub-timeline must also be done. In the event of loops,
  this function might never return true.

  \param time Time to check for done-ness
  \return true if the Timeline has no keyframes specified after the given time.
  */
  bool isDone(size_t time);

  /*!
  \brief Takes a state from the layer and updates the keyframes marked with 
  "Use Current State"

  \param state Layer state
  */
  void setCurrentState(map<string, Device*>& state);

private:
  /*!
  \brief Stores the length of the timeline.

  The length is calculated as needed, as it is a potentially time consuming thing
  to figure out.
  */
  size_t _length;

  /*!
  \brief Indicates if the timeline's length is updated.
  */
  bool _lengthIsUpdated;

  // right so the map should at some point be changed to a specialized data structure that meets
  // the following properties:
  // -given a time, can find the first and next keyframes (if they exist) as quickly as possible
  // -can handle insertions and deletions quickly (doesn't have to be as fast as possible)
  /*!
  \brief Map from unique identifier to timeline keyframes.

  The unique identifier for device parameter pair is: [deviceID]:[paramName]
  */
  map<string, map<size_t, Keyframe> > _timelineData;

  /*!
  \brief List of events and times that the events happen.
  */
  multimap<size_t, shared_ptr<Event> > _events;

  /*!
  \brief List of events to call at the end.

  Use this when you know you want something to happen at the end and don't want
  to continuously update keyframes.
  */
  map<string, shared_ptr<Event> > _endEvents;

  /*!
  \brief Updates the Keyframes marked as "Use Current State" in the Timeline's data 
  */
  void updateKeyframeState(Device* d, string paramName);
};

}
}
#endif