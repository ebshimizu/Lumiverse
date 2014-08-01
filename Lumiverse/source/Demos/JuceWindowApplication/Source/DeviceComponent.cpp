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
: PropertyComponent(TRANS(d_ptr->getId()), m_component_height), m_device(d_ptr)
{
    setLookAndFeel(m_lookandfeel = new juce::LookAndFeel_V3());
    
    // Name
    addAndMakeVisible (m_name_label = new Label ("name", TRANS(d_ptr->getId())));
    m_name_label->setFont (Font (15.00f, Font::plain));
    m_name_label->setJustificationType (Justification::centredLeft);
    m_name_label->setEditable (false, false, false);
    m_name_label->setColour (TextEditor::textColourId, Colours::black);
    m_name_label->setColour (TextEditor::backgroundColourId, Colour (0x00000000));
    m_name_label->setBounds(m_padding, m_padding, m_component_width, m_component_height);
    
    int height = parseParameters();
    
    //[UserPreSize]
    //[/UserPreSize]
    setPreferredHeight(height);
    setSize (200, height);

    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

DeviceComponent::~DeviceComponent()
{
    m_lookandfeel = nullptr;
    
    for (auto label : m_param_labels) {
        delete label.second;
    }
    
    for (auto slider : m_param_sliders) {
        delete slider.second;
    }

}

//==============================================================================
void DeviceComponent::refresh() {
    repaint();
}

void DeviceComponent::paint (Graphics& g)
{
    Path outline;
    outline.addRectangle (0, 0, getWidth(), getHeight());
    Colour baseColour = Colour::fromFloatRGBA(0.95, 0.95, 0.95, 1.f);
    
    g.setGradientFill (ColourGradient (baseColour, 0.0f, 0.0f,
                                       baseColour.darker (0.2f), 0.0f, getHeight(),
                                       false));

    g.fillPath (outline);
}

void DeviceComponent::resized()
{
    /*
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
     */
}

void DeviceComponent::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (m_param_sliders.count(sliderThatWasMoved->getName().toStdString()) > 0) {
        // Try to set parameter. If the param doesn't exist, use it as a color channel
        // Assume the name is "color".
        std::string param = sliderThatWasMoved->getName().toStdString();
        float val = sliderThatWasMoved->getValue();
        if (m_device->paramExists(param)) {
            m_device->setParam(param, val);
        }
        else if (m_device->paramExists("color")) {
            m_device->setParam("color", sliderThatWasMoved->getName().toStdString(),
                               sliderThatWasMoved->getValue());
        }
    }
    
    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}

//==============================================================================
void DeviceComponent::initLabel(Label *label, int y, string id) {
    addAndMakeVisible (label);
    label->setFont (Font (15.00f, Font::plain));
    label->setJustificationType (Justification::centredLeft);
    label->setEditable (false, false, false);
    label->setColour (TextEditor::textColourId, Colours::black);
    label->setColour (TextEditor::backgroundColourId, Colour (0x00000000));
    
    label->setBounds(m_padding, y, m_component_width, m_component_height);
    
    label->setComponentID(TRANS(id));
}

void DeviceComponent::initSlider(Slider *slider, int y, string id, float val, float min, float max) {
    addAndMakeVisible (slider);
    slider->setRange (min, max, 0);
    slider->setValue(val);
    slider->setSliderStyle (Slider::LinearHorizontal);
    slider->setTextBoxStyle (Slider::TextBoxLeft, false, 30, 20);
    slider->addListener (this);
    
    slider->setBounds(m_padding, y, m_component_width, m_component_height);
    
    slider->setComponentID(TRANS(id));
}

int DeviceComponent::parseParameters() {
    int last_pos = m_padding + m_component_height;
    int counter = 1;
    
    for (auto param : m_device->getRawParameters()) {
        LumiverseType *val = (LumiverseType*)param.second;
        if (val->getTypeName() == "float") {
            LumiverseFloat *val_float = (LumiverseFloat*)val;
            // Param label
            Label *param_label = new Label (TRANS(param.first) + TRANS(" label"), TRANS(param.first));

            std::stringstream ssl;
            ssl << counter;
            initLabel(param_label, last_pos + m_padding, ssl.str());

            m_param_labels[param.first] = param_label;
            counter++;
            last_pos += m_padding + m_component_height;
            
            // Param slider
            Slider *param_slider = new Slider(TRANS(param.first));
            
            std::stringstream sss;
            sss << counter;
            initSlider(param_slider, last_pos + m_padding, sss.str(),
                       val_float->getVal(), val_float->getMin(), val_float->getMax());
            
            m_param_sliders[param.first] = param_slider;
            counter++;
            last_pos += m_padding + m_component_height;
        }
        else if (val->getTypeName() == "color") {
            LumiverseColor *color = (LumiverseColor*)val;
            map<string, double> channels = color->getColorParams();
            
            Label *param_label = new Label (TRANS(param.first) + TRANS(" label"), TRANS(param.first));
            
            std::stringstream ssl;
            ssl << counter;
            initLabel(param_label, last_pos + m_padding, ssl.str());
            
            m_param_labels[param.first] = param_label;
            counter++;
            last_pos += m_padding + m_component_height;
            
            // Adds color channels
            for (auto chan : channels) {
                Slider *param_slider = new Slider(TRANS(chan.first));
                
                std::stringstream sss;
                sss << counter;
                initSlider(param_slider, last_pos + m_padding, sss.str(),
                           chan.second, 0.f, 10.f);
                
                m_param_sliders[chan.first] = param_slider;
                counter++;
                
                // Creates channel label
                LumiverseColor chan_color(color);
                chan_color.reset();
                chan_color.setColorChannel(chan.first, 0.5f);
                Eigen::Vector3d rgb = chan_color.getRGB();
                
                Label *chan_label = new Label (TRANS(chan.first) + TRANS(" label"));
                addAndMakeVisible (chan_label);
                chan_label->setFont (Font (12.f, Font::plain));
                chan_label->setJustificationType (Justification::centredLeft);
                chan_label->setEditable (false, false, false);
                chan_label->setColour (Label::ColourIds::backgroundColourId,
                                       Colour::fromFloatRGBA(rgb[0], rgb[1], rgb[2], 1.f));
                chan_label->setBounds(m_padding / 2 + m_component_width, last_pos + m_padding,
                                      m_component_width, m_component_height);
                
                chan_label->setComponentID(TRANS(sss.str()));
                
                m_param_labels[chan.first] = chan_label;
                last_pos += m_padding + m_component_height;
            }
        }
    }
    
    return last_pos + m_padding;
}
