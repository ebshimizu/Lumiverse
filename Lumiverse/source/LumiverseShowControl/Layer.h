#ifndef _LAYER_H_
#define _LAYER_H_

#pragma once

#include "LumiverseCore.h"
#include "Timeline.h"
#include "Playback.h"

#include <memory>
#include <chrono>
#include <unordered_map>

namespace Lumiverse {
namespace ShowControl {
  class Playback;
  
  /*! \brief Data that tracks the progress of a Timeline. */
  struct PlaybackData {
    chrono::time_point<chrono::high_resolution_clock> start;    // Timeline start time. More accurate to take difference between now and start instead of summing.
    map<string, set<string> > activeParams;
    string timelineID;
    bool complete;
    chrono::time_point<chrono::high_resolution_clock> elapsed;
  };

  /*!
  \brief A Layer stores a state of the Rig.
  
  Layers can contain a CueList, a static state, or an Effect (effects to
  be added later). Layers maintain their current state, blend mode, and
  visibility settings. The Playback object will interpret these settings
  and perform the appropriate functions to flatten the layers.
  Layers work by creating duplicates of the devices in a Rig 
  and manipulating their state.
  */
  class Layer
  {
  public:
    enum BlendMode {
      ALPHA,      /*!< Blend all parameters using traditional alpha blending. */
      OVERWRITE,  /*!< Overwrite all previous parameter values */
      MAX,        /*!< Highest parameter value takes precedence */
      MIN         /*!< Lowest parameter value takes precedence*/
    };

    /*!
    \brief Constructs a Layer.

    Copies all devices from the Rig, resets them to defaults, and sets the mode.
    By default the Layer will be set to ALPHA. A Layer is set to inactive on construction.
    */
    Layer(Rig* rig, Playback* pb, string name, int priority, BlendMode mode = ALPHA);

    /*!
    \brief Constructs a Layer using the ALPHA mode with specified opacity.

    Creates an empty layer using the selected opacity.
    */
    Layer(Rig* rig, Playback* pb, string name, int priority, float opacity);

    /*!
    \brief Constructs a Layer using the selected devices and all of their parameters.
    */
    Layer(DeviceSet set, Playback* pb, string name, int priority, BlendMode mode = ALPHA);

    /*!
    \brief Constructs an empty Layer
    */
    Layer(Playback* pb, string name, int priority, BlendMode mode = ALPHA);

    /*!
    \brief Constructs a Layer from a JSON node.
    */
    Layer(Playback* pb, JSONNode node);

    /*! \brief Destroys a layer */
    ~Layer();

    /*!
    \brief Plays a Timeline on the layer.

    This is the main function to actually get things to animate. You can hand in any Timeline you
    want as long as it's in the Playback object.

    Layers can play multiple timelines back at once, however there won't be much in the way of intelligent
    blending. System should do a latest takes precedence merge for multiple timelines.
    */
    void play(string id);

    /*!
    \brief Pauses playback of the current Timeline(s).

    If there are no Timelines being played back, this function does nothing.
    */
    void pause();

    /*!
    \brief Resumes playback of the current Timeline(s).

    Note that if there are no Timelines being played back, this function does nothing.
    */
    void resume();

    /*!
    \brief Stops and clears all current playback information from the Layer.

    The Layer state will remain in whatever state the Timeline it was playing left it.
    Calling this with no active playbacks will have no effect.
    */
    void stop();

    /*! \brief Gets the layer's blend mode */
    BlendMode getMode() { return m_mode; }

    /*! \brief Sets the Layer's blend mode. */
    void setMode(BlendMode mode) { m_mode = mode; }

    /*! \brief Get the layer's opacity. */
    float getOpacity() { return m_opacity; }

    /*! \brief Set the layer's opactiy. */
    void setOpacity(float val);

    /*! \brief Retrieves the visibility of the layer */
    bool isActive() { return m_active; }

    /*! \brief Set m_active to true. */
    void activate() { m_active = true; }
    
    /*! \brief Set m_active to false. */
    void deactivate() { m_active = false; }

    /*! \brief Set the layer name */
    void setName(string name) { m_name = name; }
    
    /*! \brief Get the layer name */
    string getName() { return m_name; }

    /*! \brief Sets the priority */
    void setPriority(int priority) { m_priority = priority; }

    /*! \brief Gets the priority. */
    int getPriority() { return m_priority; }

    /*!
    \brief Adds the selected devices and all parameters to the Layer state
    \return True on success, false on failure
    */
    bool addDevices(DeviceSet d);

    /*!
    \brief Adds a single device and parameter to the layer.
    \return True on success, false on failure
    */
    bool addDevice(Device* d, string param);

    /*!
    \brief Adds the selected devices and selected parameters to the Layer state
    \return True on success, false on failure
    */
    bool addDevicesWithParams(DeviceSet d, set<string> params);

    /*!
    \brief Adds the specified parameter with the specified type to the existing
    devices in the Layer.

    Use with caution, it's better to use addDevicesWithParams instead of using this
    \return True on success, false on failure
    */
    bool addParamToAllDevices(string param, LumiverseType* type);

    /*! \brief Deletes a single parameter from a single device in the layer */
    bool deleteParameter(string id, string param);

    /*!
    \brief Deletes the selected devices from the layer along with all of their
    parameters.
    \return True on success, false on failure
    */
    bool deleteDevices(DeviceSet d);

    /*! \brief Deletes the selected parameter values from the layer. */
    bool deleteParametersFromDevices(DeviceSet d, set<string> params);

    /*! \brief Deletes all parameters from all devices in the layer. */
    bool deleteParametersFromAllDevices(set<string> params);

    /*!
    \brief Returns the ID of the Timeline most being played on the Layer.

    \return ID of the Timeline that's being played back.
    */
    string getRecentTimeline();

    /*!
    \brief Gets the layer state.

    The layer state can be manipulated through this map.
    */
    map<string, map<string, LumiverseType*> >& getLayerState() { return m_layerState; }

    /*!
    \brief Updates the Layer. If cues a running, the cues get updated.
    \param updateStart The time at which the update loop started. Used to make sure
    each cue updates to the same point at this update.
    */
    void update(chrono::time_point<chrono::high_resolution_clock> updateStart);

    /*!
    \brief Blends this layer with the given state.

    Generally you'll let the Playback update call this function automatically.
    Layers are blended from bottom to top according to the priorities (low -> high)
    and adhere to their given BlendMode.

    The return type is void since the memory pointed to by the Device* parameter
    will be modified, and thus we don't have to write the results into a separate
    return data structure.
    */
    void blend(map<string, Device*> currentState);

    /*! \brief Returns the JSON representation of a Layer. */
    JSONNode toJSON();

    /*! \brief Restores the layer state to defaults and gets out of the current cue. */
    void reset();

  private:
    /*!
    \brief Holds the information on the current state of the layer.

    Note that the Layer will only update the parameters contained within this data structure.
    Shortcuts have been provided for layers that use all parameters, however
    you can create layers that manage a handful of parameters for a small number of devices.
    */
    map<string, map<string, LumiverseType*> > m_layerState;

    /*!
    \brief Playback object associated with the Layer.

    This object contains the Timeline objects used by the Layer during playback.
    */
    Playback* m_pb;

    /*! \brief Layer Name */
    string m_name;

    /*! \brief Layer priority. High priority layers are on top of low priority ones. */
    int m_priority;

    /*! \brief Layer visibility flag. */
    bool m_active;

    /*! \brief Layer blend mode. */
    BlendMode m_mode;

    /*! \brief If using alpha blending, the opacity of the layer. */
    float m_opacity;

    /*! \brief Indicates if playback is paused on this layer. */
    bool m_pause;

    /*! \brief Indicates that the Layer is stopping playback.
    
    This allows the update function to finish a full update and then clear the playback queue.
    */
    bool m_stop;

    /*!
    \brief Indicates if the layer is currently playing back a timeline
    */
    bool m_playing;

    /*!
    \brief Stores the previous loop start time in milliseconds.

    Can be used to calculate elapsed time since last loop.
    */
    chrono::time_point<chrono::high_resolution_clock> m_previousLoopStart;

    /*!
    \brief Stores the ID of the most recently played back Timeline.
    */
    string m_lastPlayedTimeline;

    /*! \brief Copies the devices and does other Layer initialization */
    void init(Rig* rig);

    // unsure if this function should remain in this rework.
    /*!
    \brief Returns the set of parameters to animate.

    \param a Starting Cue.
    \param b Ending Cue.
    \param assert Make sure that the Rig state at cue B is exactly cue B
    */
    //map<string, set<string> > diff(Cue* a, Cue* b, bool assert = false);

    /*!
    \brief Stores the data used during playback.
    
    Layers can only play back one timeline at a time.
    */
    PlaybackData* m_playbackData;
    
    /*! \brief Next timeline to run. */
    PlaybackData* m_queuedPlayback;

    mutex m_queue;
  };

#ifdef USE_C11_MAPS
  /*! \brief Enum translation to string for JSON output */
  static unordered_map<Layer::BlendMode, string, std::hash<unsigned int>> BlendModeToString {
      { Layer::ALPHA, "ALPHA" },
      { Layer::OVERWRITE, "OVERWRITE" },
      { Layer::MAX, "MAX" },
      { Layer::MIN, "MIN" }
  };

  /*! \brief String translation back to Enum for data load */
  static unordered_map<string, Layer::BlendMode> StringToBlendMode {
      { "ALPHA", Layer::ALPHA },
      { "OVERWRITE", Layer::OVERWRITE },
      { "MAX", Layer::MAX },
      { "MIN", Layer::MIN }
  };
#else
	static string BlendModeToString(Layer::BlendMode b) {
		switch (b) {
		case Layer::ALPHA: return "ALPHA";
		case Layer::OVERWRITE: return "OVERWRITE";
		case Layer::MAX: return "MAX";
		case Layer::MIN: return "MIN";
		default: return "";
		}
	}

	static Layer::BlendMode StringToBlendMode(string b) {
		if (b == "ALPHA") return Layer::ALPHA;
		if (b == "OVERWRITE") return Layer::OVERWRITE;
		if (b == "MAX") return Layer::MAX;
		if (b == "MIN") return Layer::MIN;
		return Layer::BLEND_OPAQUE;
	}
#endif


}

// Note that for some reason when generating SWIG bindings, having these outside of the Lumiverse
// Namespace (they were in Lumiverse::ShowControl) causes some problems with SWIG's intermediate code
// generator.
// Comparison op overloads
/*!
\brief Compares two layer priorities for equality.
\return True if the priorities of the two layers are equal.
*/
inline bool operator==(ShowControl::Layer& lhs, ShowControl::Layer& rhs) {
  return (lhs.getPriority() == rhs.getPriority());
}

inline bool operator!=(ShowControl::Layer& lhs, ShowControl::Layer& rhs) {
  return !(lhs == rhs);
}

/*!
\brief Compares two layer priorities for order.
\return True if the priority of lhs is lower than the priority for rhs.
*/
inline bool operator<(ShowControl::Layer& lhs, ShowControl::Layer& rhs) {
  return (lhs.getPriority() < rhs.getPriority());
}

inline bool operator>(ShowControl::Layer& lhs, ShowControl::Layer& rhs) {
  return rhs < lhs;
}

inline bool operator<=(ShowControl::Layer& lhs, ShowControl::Layer& rhs) {
  return !(lhs > rhs);
}

inline bool operator>=(ShowControl::Layer& lhs, ShowControl::Layer& rhs) {
  return !(lhs < rhs);
}
}

namespace std
{
    template<>
    struct hash<Lumiverse::ShowControl::Layer::BlendMode>
    {
        size_t operator()( const Lumiverse::ShowControl::Layer::BlendMode& arg ) const
        {
            std::hash<unsigned int> hasher;
            return hasher( static_cast<unsigned int>( arg ) );
        }
    };
}

#endif