/*
  ==============================================================================

  This is an automatically generated GUI class created by the Introjucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Introjucer version: 3.1.0

  ------------------------------------------------------------------------------

  The Introjucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-13 by Raw Material Software Ltd.

  ==============================================================================
*/

#ifndef __JUCE_HEADER_DeviceComponent__
#define __JUCE_HEADER_DeviceComponent__

#pragma once

//[Headers]     -- You can add your own extra header files here --
#include <map>
#include "JuceHeader.h"
#include "GuiConfig.h"
#include "../../../LumiverseCore/LumiverseCore.h"
//[/Headers]

using namespace Lumiverse;

//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Introjucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class DeviceComponent  : public PropertyComponent,
                      public SliderListener
{
public:
    //==============================================================================
    DeviceComponent (Device *d_ptr);
    ~DeviceComponent();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    //[/UserMethods]

    virtual void refresh();
    void paint (Graphics& g);
    void resized();
    void sliderValueChanged (Slider* sliderThatWasMoved);
    
    String getDeviceName() { return TRANS(m_device->getId()); }
    
private:
    int parseParameters();
    
    void initLabel(Label *label, int y, string id);
    void initSlider(Slider *slider, int y, string id,
                    float val, float min, float max);
    
    //[UserVariables]   -- You can add your own custom variables in this section.
    //[/UserVariables]

    //==============================================================================
    ScopedPointer<LookAndFeel> m_lookandfeel;
    
    ScopedPointer<Label> m_name_label;
    map<std::string, Label*> m_param_labels;
    map<std::string, Slider*> m_param_sliders;
    
    Device *m_device;
    
    const int m_padding = 3;
    const int m_component_width = 150;
    const int m_component_height = 24;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeviceComponent)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

#endif   // __JUCE_HEADER_DeviceComponent__
