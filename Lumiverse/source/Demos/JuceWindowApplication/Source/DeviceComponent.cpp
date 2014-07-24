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

//[Headers] You can add your own extra header files here...
//[/Headers]

#include "DeviceComponent.h"
#include <sstream>

using namespace Lumiverse;

//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
DeviceComponent::DeviceComponent (Device *d_ptr)
: m_device(d_ptr)
{
    setLookAndFeel(m_lookandfeel = new juce::LookAndFeel_V3());
    
    // Name
    addAndMakeVisible (m_name_label = new Label ("name",
                                                      TRANS(d_ptr->getId())));
    m_name_label->setFont (Font (15.00f, Font::plain));
    m_name_label->setJustificationType (Justification::centredLeft);
    m_name_label->setEditable (false, false, false);
    m_name_label->setColour (TextEditor::textColourId, Colours::black);
    m_name_label->setColour (TextEditor::backgroundColourId, Colour (0x00000000));
    m_name_label->setBounds(m_padding, m_padding, m_component_width, m_component_height);
    
    int height = parseParameters();
    
    //[UserPreSize]
    //[/UserPreSize]
    setSize (200, height);

    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

DeviceComponent::~DeviceComponent()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    m_lookandfeel = nullptr;
    
    for (auto label : m_param_labels) {
        delete label.second;
    }
    
    for (auto slider : m_param_sliders) {
        delete slider.second;
    }
    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void DeviceComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]
    Path outline;
    outline.addRectangle (0, 0, getWidth(), getHeight());
    Colour baseColour = Colour::fromFloatRGBA(0.95, 0.95, 0.95, 1.f);
    
    g.setGradientFill (ColourGradient (baseColour, 0.0f, 0.0f,
                                       baseColour.darker (0.2f), 0.0f, getHeight(),
                                       false));

    g.fillPath (outline);
    
    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void DeviceComponent::resized()
{
    m_name_label->setBounds(m_padding, m_padding, m_component_width, m_component_height);
    
    for (auto label : m_param_labels) {
        int order;
        std::stringstream ss(label.second->getComponentID().toStdString());
        ss >> order;
        label.second->setBounds(m_padding, (m_padding + m_component_height) * order + m_padding,
                                m_component_width, m_component_height);
    }
    
    for (auto slider : m_param_sliders) {
        int order;
        std::stringstream ss(slider.second->getComponentID().toStdString());
        ss >> order;
        slider.second->setBounds(m_padding, (m_padding + m_component_height) * order + m_padding,
                                m_component_width, m_component_height);
    }
    
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void DeviceComponent::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (m_param_sliders.count(sliderThatWasMoved->getName().toStdString()) > 0)
    {
        /*
        m_device->setMetadata("intensity",
                        m_intensity_slider->getTextFromValue(m_intensity_slider->getValue()).toStdString());
        */
        m_device->setParam(sliderThatWasMoved->getName().toStdString(), sliderThatWasMoved->getValue());
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}

//==============================================================================
int DeviceComponent::parseParameters() {
    int last_pos = m_padding + m_component_height;
    int counter = 1;
    
    for (auto param : m_device->getRawParameters()) {
        LumiverseType *val = (LumiverseType*)param.second;
        if (val->getTypeName() == "float") {
            LumiverseFloat *val_float = (LumiverseFloat*)val;
            // Param label
            Label *param_label = new Label (TRANS(param.first) + TRANS(" label"), TRANS(param.first));
            addAndMakeVisible (param_label);
            param_label->setFont (Font (15.00f, Font::plain));
            param_label->setJustificationType (Justification::centredLeft);
            param_label->setEditable (false, false, false);
            param_label->setColour (TextEditor::textColourId, Colours::black);
            param_label->setColour (TextEditor::backgroundColourId, Colour (0x00000000));
            
            param_label->setBounds(m_padding, last_pos + m_padding, m_component_width, m_component_height);
            
            std::stringstream ssl;
            ssl << counter;
            param_label->setComponentID(TRANS(ssl.str()));
            
            m_param_labels[param.first] = param_label;
            
            counter++;
            last_pos = last_pos + m_padding + m_component_height;
            
            // Param slider
            Slider *param_slider = new Slider(TRANS(param.first));
            addAndMakeVisible (param_slider);
            param_slider->setRange (val_float->getMin(), val_float->getMax(), 0);
            param_slider->setValue(val_float->getVal());
            param_slider->setSliderStyle (Slider::LinearHorizontal);
            param_slider->setTextBoxStyle (Slider::TextBoxLeft, false, 30, 20);
            param_slider->addListener (this);
            
            param_slider->setBounds(m_padding, last_pos + m_padding, m_component_width, m_component_height);
            
            std::stringstream sss;
            sss << counter;
            param_slider->setComponentID(TRANS(sss.str()));
            
            counter++;
            last_pos = last_pos + m_padding + m_component_height;
            m_param_sliders[param.first] = param_slider;
        }
    }
    
    return last_pos + m_padding;
}
