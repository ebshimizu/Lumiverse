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
#include <cstdio>
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
class InterruptionComponent : public ButtonPropertyComponent
{
public:
    InterruptionComponent (const String& propertyName, Rig *rig)
    : ButtonPropertyComponent (propertyName, true), m_rig(rig)
    {
        refresh();
    }
    
    void buttonClicked() override
    {
        ((ArnoldAnimationPatch*)m_rig->getSimulationPatch("ArnoldAnimationPatch"))->interruptRender();
    }
    
    String getButtonText() const override
    {
        return "Interrupt";
    }
    
private:
    Rig *m_rig;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InterruptionComponent)
};

class SamplesComponent : public SliderPropertyComponent
{
public:
    SamplesComponent (const String& propertyName, Rig *rig, const bool is_preview)
    : SliderPropertyComponent (propertyName, -3, 8, 1), m_rig(rig), m_is_preview(is_preview)
    {
        ArnoldAnimationPatch *aap = (ArnoldAnimationPatch*)m_rig->getSimulationPatch("ArnoldAnimationPatch");
        int defaultVal = (m_is_preview) ? aap->getPreviewSamples() : aap->getRenderSamples();

        setValue (defaultVal);
    }
    
    void setValue (double newValue) override
    {
        slider.setValue (newValue);
    }
    
    void sliderValueChanged (Slider *slider) {
        ArnoldAnimationPatch *aap = (ArnoldAnimationPatch*)m_rig->getSimulationPatch("ArnoldAnimationPatch");
        int val = (int)slider->getValue();
        if (m_is_preview) {
            aap->setPreviewSamples(val);
            aap->rerender();
        }
        else
            aap->setRenderSamples(val);
    }
    
private:
    Rig *m_rig;
    bool m_is_preview;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SamplesComponent)
};

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
    
    void drawMode(Graphics& g);

	void drawBuckets(Graphics& g);
    //[UserVariables]   -- You can add your own custom variables in this section.
    //[/UserVariables]

    //==============================================================================
    InterruptionComponent *m_interrupt;
    Array<SamplesComponent*> m_samples;
    ScopedPointer<LookAndFeel> m_lookandfeel;
    ScopedPointer<AnimationComponent> m_animation_pad;
    Array<PropertyComponent*> m_device_pads;
    
    PropertyPanel m_interactive_panel;
    PropertyPanel m_devices_property_panel;
    ConcertinaPanel m_concertina_panel;
    
    Image m_panel;
    float *m_color_buffer;
    
    int m_width, m_height;
    int m_upper_height;
    
    Rig *m_rig;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuiComponent)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

#endif   // __JUCE_HEADER_F1E5D9BB3E654E28__
