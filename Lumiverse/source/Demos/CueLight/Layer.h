#ifndef _LAYER_H_
#define _LAYER_H_

#pragma once

#include "LumiverseCore.h"
#include "CueList.h"
#include "Cue.h"

#include <memory>
#include <chrono>
#include <unordered_map>

namespace Lumiverse {
  /*! \brief Data that tracks the progress of a cue and stores the data used in the cue transition. */
  struct PlaybackData {
    chrono::time_point<chrono::high_resolution_clock> start;    // Cue start time. More accurate to take difference between now and start instead of summing.
    map<string, set<string> > activeParams;
    Cue targetCue;
    Cue previousCue;
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
      BLEND_OPAQUE,   /*!< Blend all devices. */
      NULL_DEFAULT,   /*!< Ignores parameters that are left at the default values */
      NULL_INTENSITY, /*!< Ignores devices with intensity set to 0. If devices have no intensity, they will be active. */
      SELECTED_ONLY   /*!< Uses a DeviceSet to determine which devices to affect using normal blending. */
    };

    /*!
    \brief Constructs a Layer.

    Copies all devices from the Rig, resets them to defaults, and sets the mode.
    By default the Layer will be set to NULL_DEFAULT, essentially ignoring all
    parameters in the layer when it's constructed. A layer is set to inactive on construction.
    */
    Layer(Rig* rig, string name, int priority, BlendMode mode = NULL_DEFAULT);

    /*!
    \brief Constructs a Layer using the BLEND_OPAQUE mode with specified opacity.

    Creates an empty layer using the selected opacity.
    */
    Layer(Rig* rig, string name, int priority, float opacity);

    /*!
    \brief Constructs a Layer using the SELECTED_ONLY mode with specified devices.

    Creates an empty layer with the selected devices. Note that the layer will
    contain all other devices in the rig, but will only look at the selected set
    when flattening.
    */
    Layer(Rig* rig, string name, int priority, DeviceSet set);

    /*!
    \brief Constructs a Layer from a JSON node.
    */
    Layer(JSONNode node);

    /*! \brief Destroys a layer */
    ~Layer();

    /*! 
    \brief Assigns a cue list to the layer.

    The layer will automatically go to the first cue in the list.
    */
    void setCueList(shared_ptr<CueList> list, bool resetCurrentCue = true);

    /*! \brief Removes the active cue list from the layer. Layer will reset to defaults. */
    void removeCueList();

    /*! \brief Checks if the current Layer has a cue list attached. */
    bool hasCueList() { return m_cueList != nullptr; }

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

    /*! \brief Sets the parameter filter using a fully specified set. */
    void setParamFilter(set<string> filter) { m_parameterFilter = filter; }

    /*! \brief Adds a single parameter to the filter. */
    void addParamFilter(string param);

    /*! \brief Removes a single parameter from the filter. */
    void removeParamFilter(string param);

    /*! \brief Removes the device filter entirely. */
    void deleteParamFilter();

    /*! \brief Inverts the filter. m_invertFilter = !m_invertFilter. */
    void invertFilter();

    /*! \brief True = inverted, false = normal. */
    bool getFilterStatus() { return m_invertFilter; }

    /*! \brief Sets the selected devices for this layer.*/
    void setSelectedDevices(DeviceSet devices);

    /*! \brief Removes the given devices from the selected devices for the layer. */
    void removeSelectedDevices(DeviceSet devices);

    /*! \brief Adds the given devices to the selected devices fot the layer. */
    void addSelectedDevices(DeviceSet devices);

    /*! \brief Removes the selected devices filter from this layer. */
    void clearSelectedDevices();

    /*! \brief Gets the selected devices for this layer. */
    DeviceSet getSelectedDevices() { return m_selectedDevices; }

    /*! \brief Set the layer name */
    void setName(string name) { m_name = name; }
    
    /*! \brief Get the layer name */
    string getName() { return m_name; }

    /*! \brief Sets the priority */
    void setPriority(int priority) { m_priority = priority; }

    /*! \brief Gets the priority. */
    int getPriority() { return m_priority; }

    /*! \brief Gets the cue list. */
    const shared_ptr<CueList>& getCueList() { return m_cueList; }

    /*! \brief Gets the current cue. */
    float getCurrentCue() { return m_currentCue; }

    /*!
    \brief Gets the layer state.

    The layer state can be manipulated through this map.
    */
    map<string, Device*>& getLayerState() { return m_layerState; }

    /*!
    \brief Goes to the next cue in the cue list if a cue list exists in the layer.

    Equivalent to a go button for a submaster on a light board.
    */
    void go();

    /*!
    \brief Goes back a cue in the cue list if a cue list exists in the layer.

    Equivalent to a back button for a submaster on a light board.
    */
    void back();

    // The cue adding process has changed a bit in this version of the Playback system.
    // Cues now must be added to a CueList which must then be added to an active Layer
    // for them to have any effect on the actual rig. It might seem like a bunch of
    // extra unnecessary steps, but it should make running muiltiple simultaneous
    // cue stacks easier.

    /*!
    \brief Goes to the specified cue in a cue list.

    \param num The cue number to go to. If the cue doesn't exist, the layer will not change.
    \param up Upfade. Defaults to 3s.
    \param down Downfade. Defaults to 3s.
    \param delay Delay. Defaults to 0s.
    */
    void goToCue(float num, float up = 3, float down = 3, float delay = 0);

    /*!
    \brief Sets the layer state to the state at the specified time in a cue.

    This seeks to a position in a timeline. If time is greater than the length of the cue,
    it will be clamped to the beginning of the next cue (if one exists).
    \param num Cue number
    \param time Location to seek to
    */
    void goToCueAtTime(float num, float time);

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

  private:
    /*!
    \brief Holds the information on the current state of the layer.

    Might notice that this is the same structure used inside of the Rig itself.
    */
    map<string, Device*> m_layerState;

    /*!
    \brief Holds information about parameters that should or shouldn't be
    included when this layer is flattened.

    You can choose to only affect a certain number of parameters in a
    layer. So while the layer may store all of the parameters, you can
    choose to ignore some.
    */
    set<string> m_parameterFilter;

    /*! \brief Layer Name */
    string m_name;

    /*! \brief Layer priority. High priority layers are on top of low priority ones. */
    int m_priority;

    /*! \brief If true, filter will be for all parameters except the specified params. */
    bool m_invertFilter;

    /*! \brief Layer visibility flag. */
    bool m_active;

    /*! \brief Layer blend mode. */
    BlendMode m_mode;

    /*! \brief If using SELECTED_ONLY mode, the devices to use. */
    DeviceSet m_selectedDevices;

    /*! \brief If using BLEND_OPAQUE, the opacity of the layer. */
    float m_opacity;

    /*! \brief Selected CueList if the layer is using one. */
    shared_ptr<CueList> m_cueList;

    /*! \brief Copies the devices and does other Layer initialization */
    void init(Rig* rig);

    /*! \brief Current cue in the cue list. */
    float m_currentCue;

    /*!
    \brief Goes to an arbitrary cue from an arbitrary cue.

    \param first Starting cue.
    \param next Cue to end up in at the end.
    \param assert Asserts that at the end of the transition, the Rig state is exactly Next
    */
    void goToCue(Cue* first, Cue* next, bool assert);

    /*!
    \brief Returns the set of parameters to animate.

    \param a Starting Cue.
    \param b Ending Cue.
    \param assert Make sure that the Rig state at cue B is exactly cue B
    */
    map<string, set<string> > diff(Cue* a, Cue* b, bool assert = false);

    /*! \brief Stores the data used during playback. */
    vector<PlaybackData> m_playbackData;

    /*! \brief Queue for adding playback data to the update loop. */
    vector<PlaybackData> m_queuedPlayback;

    mutex m_queue;
  };

  /*! \brief Enum translation to string for JSON output */
  static unordered_map<Layer::BlendMode, string, std::hash<unsigned int>> BlendModeToString {
      { Layer::BLEND_OPAQUE, "BLEND_OPAQUE" },
      { Layer::NULL_DEFAULT, "NULL_DEFAULT" },
      { Layer::NULL_INTENSITY, "NULL_INTENSITY" },
      { Layer::SELECTED_ONLY, "SELECTED_ONLY" }
  };

  /*! \brief String translation back to Enum for data load */
  static unordered_map<string, Layer::BlendMode> StringToBlendMode {
      { "BLEND_OPAQUE", Layer::BLEND_OPAQUE },
      { "NULL_DEFAULT", Layer::NULL_DEFAULT },
      { "NULL_INTENSITY", Layer::NULL_INTENSITY },
      { "SELECTED_ONLY", Layer::SELECTED_ONLY }
  };

  // Comparison op overloads
  /*!
  \brief Compares two layer priorities for equality.
  \return True if the priorities of the two layers are equal.
  */
  inline bool operator==(Layer& lhs, Layer& rhs) {
    return (lhs.getPriority() == rhs.getPriority());
  }

  inline bool operator!=(Layer& lhs, Layer& rhs) {
    return !(lhs == rhs);
  }

  /*!
  \brief Compares two layer priorities for order.
  \return True if the priority of lhs is lower than the priority for rhs.
  */
  inline bool operator<(Layer& lhs, Layer& rhs) {
    return (lhs.getPriority() < rhs.getPriority());
  }

  inline bool operator>(Layer& lhs, Layer& rhs) {
    return rhs < lhs;
  }

  inline bool operator<=(Layer& lhs, Layer& rhs) {
    return !(lhs > rhs);
  }

  inline bool operator>=(Layer& lhs, Layer& rhs) {
    return !(lhs < rhs);
  }
}

namespace std
{
    template<>
    struct hash<Lumiverse::Layer::BlendMode>
    {
        size_t operator()( const Lumiverse::Layer::BlendMode& arg ) const
        {
            std::hash<unsigned int> hasher;
            return hasher( static_cast<unsigned int>( arg ) );
        }
    };
}

#endif