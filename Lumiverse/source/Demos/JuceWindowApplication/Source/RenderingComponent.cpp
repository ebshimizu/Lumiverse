//
//  RenderingComponent.cpp
//  JuceWindowApp
//
//  Created by Chenxi Liu on 7/14/14.
//
//

#include "RenderingComponent.h"
#include "GuiComponent.h"

RenderingComponent::RenderingComponent() {
    addAndMakeVisible (m_intensity_label = new Label ("new label",
                                                      TRANS("Intensity")));
    m_intensity_label->setFont (Font (15.00f, Font::plain));
    m_intensity_label->setJustificationType (Justification::centredLeft);
    m_intensity_label->setEditable (false, false, false);
    m_intensity_label->setColour (TextEditor::textColourId, Colours::black);
    m_intensity_label->setColour (TextEditor::backgroundColourId, Colour (0x00000000));
    
    addAndMakeVisible (m_intensity_slider = new Slider ("new slider"));
    m_intensity_slider->setRange (0, 10, 0);
    m_intensity_slider->setSliderStyle (Slider::LinearHorizontal);
    m_intensity_slider->setTextBoxStyle (Slider::TextBoxLeft, false, 80, 20);
    
    
    //[UserPreSize]
    //[/UserPreSize]
    setSize (800, 600);
}

RenderingComponent::RenderingComponent(float *buffer, Image::PixelFormat format, int imageWidth, int imageHeight)
: m_panel(format, imageWidth, imageHeight, true), m_color_buffer(buffer) {
    addAndMakeVisible (m_intensity_label = new Label ("new label",
                                                      TRANS("Intensity")));
    m_intensity_label->setFont (Font (15.00f, Font::plain));
    m_intensity_label->setJustificationType (Justification::centredLeft);
    m_intensity_label->setEditable (false, false, false);
    m_intensity_label->setColour (TextEditor::textColourId, Colours::black);
    m_intensity_label->setColour (TextEditor::backgroundColourId, Colour (0x00000000));
    
    addAndMakeVisible (m_intensity_slider = new Slider ("new slider"));
    m_intensity_slider->setRange (0, 10, 0);
    m_intensity_slider->setSliderStyle (Slider::LinearHorizontal);
    m_intensity_slider->setTextBoxStyle (Slider::TextBoxLeft, false, 80, 20);
    
    
    //[UserPreSize]
    //[/UserPreSize]
    setSize (imageWidth, imageHeight);
}

void RenderingComponent::paint (Graphics& g) {
    if (m_color_buffer == NULL)
        return ;
    
    for (size_t y = 0; y < m_panel.getHeight(); y++) {
        for (size_t x = 0; x < m_panel.getWidth(); x++) {
            size_t pixelWidth = (m_panel.getFormat() == Image::PixelFormat::RGB) ? 3 : 4;
            size_t offset = (y * m_panel.getWidth() + x) * pixelWidth;
            Colour color = Colour::fromFloatRGBA(m_color_buffer[offset], m_color_buffer[offset + 1],
                                                 m_color_buffer[offset + 2], m_color_buffer[offset + 3]);
            m_panel.setPixelAt(x, y, color);
        }
    }
    
    g.drawImageAt(m_panel, 0, 0);
}