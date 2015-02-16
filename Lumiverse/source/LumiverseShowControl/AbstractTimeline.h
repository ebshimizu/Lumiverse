#ifndef _ABSTRACTTIMELINE_H_
#define _ABSTRACTTIMELINE_H_

#include <LumiverseCore.h>
#include <memory>

namespace Lumiverse {
namespace ShowControl {
  /*!
  \brief This class defines the basic interface for interacting with Timelines.
  
  Timelines really only need a few core things to be Lumiverse Timelines. If you plan
  on writing a new type of timeline, this would be the place to do it.
  */
  class AbstractTimeline {
  public:
    virtual ~AbstractTimeline() { }

    /*!
    \brief Returns the value for the specified identifier at the specified time.

    When requested, the timeline should return a value for the requested device and parameter at the given time.
    */
    virtual shared_ptr<LumiverseType> getValueAtTime(Device* d, string paramName, size_t time, map<string, shared_ptr<AbstractTimeline> >& tls) = 0;

    /*!
    \brief Executes any events assigned to the Timeline.

    Note that the AbstractTimeline does not enforce any requirement to have events,
    just that this function must be present. This is mainly for convenience in the Layer,
    so the Layer doesn't have to check for the existence of events when it's doing the update step.
    You are free to ignore this function if you do not need it.
    \sa Layer
    */
    virtual void executeEvents(size_t prevTime, size_t currentTime) { return; }

    /*!
    \brief Serializes a Timeline

    Required for all Timelines to make themselves into serializable things.
    */
    virtual JSONNode toJSON() = 0;

    /*!
    \brief Returns the length of the Timeline.
    */
    virtual size_t getLength() = 0;

    /*!
    \brief Indicates if the AbstractTimeline is done running.

    The map of timeline ids to AbstractTimelines is needed in the event that the timeline you're running
    has additional timelines nested within it. You are free to ignore this parameter if you wish.
    */
    virtual bool isDone(size_t time, map<string, shared_ptr<AbstractTimeline> >& tls) = 0;

    /*!
    \brief Takes a scene from the Layer running the timeline and updates the current timeline accordingly.

    If your timeline does not require any updating from the layer you are free to ignore this function.
    */
    virtual void setCurrentState(map<string, Device*>& state, shared_ptr<AbstractTimeline> active, size_t time) { return; }

    /*!
    \brief Returns the type of the timeline.
    */
    virtual string getType() = 0;
  };

}
}


#endif