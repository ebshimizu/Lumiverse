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

#ifndef __JUCE_HEADER_F1E5D9BB3E654E28__
#define __JUCE_HEADER_F1E5D9BB3E654E28__

#pragma once

//[Headers]     -- You can add your own extra header files here --
#include <vector>
#include "JuceHeader.h"
#include "GuiConfig.h"
#include "DeviceComponent.h"
#include "AnimationComponent.h"
#include "../../../LumiverseCore/LumiverseCore.h"
#include "RepaintTimer.h"
#include "AnimationTimer.h"
//[/Headers]

using namespace Lumiverse;

//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Introjucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class GuiComponent  : public Component,
                      public ButtonListener
{
public:
    //==============================================================================
    GuiComponent (float *buffer, Image::PixelFormat format,
                  int imageWidth, int imageHeight, Rig *rig);
    ~GuiComponent();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void buttonClicked (Button* buttonThatWasClicked);
    
    void setBuffer(float *buffer) {
        m_color_buffer = buffer;
    }
    
private:
    int addDevicePads();
    //[UserVariables]   -- You can add your own custom variables in this section.
    //[/UserVariables]

    //==============================================================================
    ScopedPointer<Button> m_abort_button;
    ScopedPointer<LookAndFeel> m_lookandfeel;
    
    ScopedPointer<AnimationComponent> m_animation_pad;
    Array<DeviceComponent*> m_device_pads;
    
    Image m_panel;
    float *m_color_buffer;
    
    int m_width, m_height;
    int m_upper_height;
    
    Rig *m_rig;
    ScopedPointer<RepaintTimer> m_timer;
    ScopedPointer<AnimationTimer> m_animation_timer;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuiComponent)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

#endif   // __JUCE_HEADER_F1E5D9BB3E654E28__
