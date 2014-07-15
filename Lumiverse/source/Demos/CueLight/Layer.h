#ifndef _LAYER_H_
#define _LAYER_H_

#pragma once

#include "LumiverseCore.h"
#include "CueList.h"
#include "Cue.h"
#include <memory>

namespace Lumiverse {
  // A Layer stores a state of the Rig.
  // Layers can contain a CueList, a static state, or an Effect (effects to
  // be added later). Layers maintain their current state, blend mode, and
  // visibility settings. The Playback object will interpret these settings
  // and perform the appropriate functions to flatten the layers.
  // Layers work by creating duplicates of the devices in a Rig 
  // and manipulating their state.
  class Layer
  {
  public:
    enum BlendMode {
      BLEND_OPAQUE,   /*!< Normal blending. Uses the opacity value (0-1) */
      NULL_DEFAULT,   /*!< Ignores parameters that are left at the default values */
      NULL_INTENSITY, /*!< Ignores devices with intensity set to 0. If devices have no intensity, they will be active. */
      SELECTED_ONLY   /*!< Uses a DeviceSet to determine which devices to affect. */
    };

    /*!
    \brief Constructs a Layer.

    Copies all devices from the Rig, resets them to defaults, and sets the mode.
    By default the Layer will be set to NULL_DEFAULT, essentially ignoring all
    parameters in the layer when it's constructed. A layer is set to inactive on construction.
    */
    Layer(Rig* rig, BlendMode mode = NULL_DEFAULT);

    /*!
    \brief Constructs a Layer using the BLEND_OPAQUE mode with specified opacity.

    Creates an empty layer using the selected opacity.
    */
    Layer(Rig* rig, float opacity);

    /*!
    \brief Constructs a Layer using the SELECTED_ONLY mode with specified devices.

    Creates an empty layer with the selected devices. Note that the layer will
    contain all other devices in the rig, but will only look at the selected set
    when flattening.
    */
    Layer(Rig* rig, DeviceSet set);

    /*! \brief Destroys a layer */
    ~Layer();

    /*! 
    \brief Assigns a cue list to the layer.

    The layer will automatically go to the first cue in the list.
    */
    void setCueList(shared_ptr<CueList*> list);

    /*! \brief Removes the active cue list from the layer. Layer will reset to defaults. */
    void removeCueList();

    /*! \brief Gets the layer's blend mode */
    BlendMode getMode() { return m_mode; }

    /*! \brief Sets the Layer's blend mode. */
    void setMode(BlendMode mode);

    /*! \brief Get the layer's opacity. */
    float getOpacity() { return m_opacity; }

    /*! \brief Set the layer's opactiy. */
    void setOpacity(float val);

    /*! \brief Retrieves the visibility of the layer */
    bool isActive() { return m_active; }

    /*! \brief Set m_active to true. */
    void activate();
    
    /*! \brief Set m_active to false. */
    void deactivate();

    /*! \brief Sets the parameter filter using a fully specified set. */
    void setParamFilter(set<string> filter);

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

    /*!
    \brief Gets the layer state.

    The layer state can be manipulated through this map.
    */
    map<string, Device*> getLayerState() { return m_layerState; }

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

    /*!
    \brief Goes to the specified cue in the cue list if a cue list exists in the layer.
    \return False if no cue list is in the layer or cue number doesn't exist.
    */
    bool goToCue(float cueNum);

    /*!
    \brief Updates the Layer. If cues a running, the cues get updated.
    */
    void update();

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
    shared_ptr<CueList*> m_cueList;
  };
}
#endif