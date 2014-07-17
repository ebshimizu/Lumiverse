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

#include "GuiComponent.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
GuiComponent::GuiComponent (float *buffer, Image::PixelFormat format,
                            int imageWidth, int imageHeight, Rig *rig)
: m_panel(format, imageWidth, imageHeight, true), m_color_buffer(buffer),
m_width(imageWidth), m_height(imageHeight), m_rig(rig)
{
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
    m_intensity_slider->addListener (this);

    addAndMakeVisible (m_abort_button = new TextButton (String::empty));
    m_abort_button->setButtonText ("Abort");
    m_abort_button->addListener (this);
    
    //[UserPreSize]
    //[/UserPreSize]
    setSize (imageWidth + 200, imageHeight);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

GuiComponent::~GuiComponent()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    m_intensity_label = nullptr;
    m_intensity_slider = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void GuiComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

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
    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void GuiComponent::resized()
{
    m_abort_button->setBounds (getWidth() - 176, getHeight() - 60, 120, 32);
    
    m_intensity_label->setBounds (16, 16, 150, 24);
    m_intensity_slider->setBounds (24, 48, 150, 24);
    m_intensity_label->setTopLeftPosition(m_width + 10, 10);
    m_intensity_slider->setTopLeftPosition(m_width + 10, 10 + 30);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void GuiComponent::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == m_intensity_slider)
    {
        //[UserSliderCode_slider] -- add your slider handling code here..
        //[/UserSliderCode_slider]
        /*
        m_intensity_label->setText(m_intensity_slider->getTextFromValue(m_intensity_slider->getValue()),
                                   juce::NotificationType::dontSendNotification);
         */
        m_rig->getDevice("mylight")->setMetadata("intensity",
                        m_intensity_slider->getTextFromValue(m_intensity_slider->getValue()).toStdString());
        ((ArnoldPatch*)m_rig->getSimulationPatch())->onDeviceChanged("mylight");
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}

void GuiComponent::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]
    
    if (buttonThatWasClicked == m_abort_button)
    {
        //[UserButtonCode_quitButton] -- add your button handler code here..
        
        ((ArnoldPatch*)m_rig->getSimulationPatch())->abortRender();
        
        //[/UserButtonCode_quitButton]
    }
    
    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Introjucer information section --

    This is where the Introjucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="GuiComponent" componentName=""
                 parentClasses="public Component" constructorParams="" variableInitialisers=""
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="ffffffff"/>
  <LABEL name="new label" id="7f73fc78619c63a7" memberName="label" virtualName=""
         explicitFocusOrder="0" pos="16 16 150 24" edTextCol="ff000000"
         edBkgCol="0" labelText="label text" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15"
         bold="0" italic="0" justification="33"/>
  <SLIDER name="new slider" id="91d4d20eb5a7cf59" memberName="slider" virtualName=""
          explicitFocusOrder="0" pos="24 48 150 24" min="0" max="10" int="0"
          style="LinearHorizontal" textBoxPos="TextBoxLeft" textBoxEditable="1"
          textBoxWidth="80" textBoxHeight="20" skewFactor="1"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]
