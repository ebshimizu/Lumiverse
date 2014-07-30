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
    setLookAndFeel(m_lookandfeel = new juce::LookAndFeel_V3());

    addAndMakeVisible (m_abort_button = new TextButton (String::empty));
    m_abort_button->setButtonText ("Abort");
    m_abort_button->addListener (this);
    
    int height = addDevicePads();
    int dcwidth = 0;
    
    if (m_device_pads.size() > 0)
        dcwidth = m_device_pads[0]->getWidth();
    
    m_upper_height = std::max(height, imageHeight);
    setSize (imageWidth + dcwidth, m_upper_height);
    
    addAndMakeVisible(m_animation_pad = new AnimationComponent(rig, this));
    m_animation_pad->setTopLeftPosition(0, imageHeight);
    
    //[UserPreSize]
    //[/UserPreSize]
    setSize (imageWidth + dcwidth, m_upper_height + m_animation_pad->getHeight());

    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
    
    ArnoldAnimationPatch *aap = (ArnoldAnimationPatch*)m_rig->getSimulationPatch("ArnoldAnimationPatch");
    m_animation_timer = new AnimationTimer(this, aap->getFrameManager());
    aap->startInteractive();
    
    m_timer = new RepaintTimer(this);
    //m_timer->startTimer(1000);
}

GuiComponent::~GuiComponent()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]
    ArnoldAnimationPatch *aap = (ArnoldAnimationPatch*)m_rig->getSimulationPatch("ArnoldAnimationPatch");
    aap->close();
    
    m_animation_timer->stopTimer();
    m_timer->stopTimer();
    
    m_animation_timer = nullptr;
    m_timer = nullptr;
    
    m_abort_button = nullptr;
    m_lookandfeel = nullptr;

    for (DeviceComponent *dc : m_device_pads) {
        delete dc;
    }
    
    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void GuiComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]
    Path outline;
    outline.addRectangle (m_width, 0, m_width, m_upper_height);
    Colour baseColour = Colour::fromFloatRGBA(0.95, 0.95, 0.95, 1.f);
    
    g.setGradientFill (ColourGradient (baseColour, 0.0f, 0.0f,
                                       baseColour.darker (0.2f), 0.0f, m_height,
                                       false));
    g.fillPath (outline);
    
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
    m_abort_button->setBounds (getWidth() - 176, m_upper_height - 60, 120, 32);
    
    int last_height = 0;
    for (DeviceComponent *dc : m_device_pads) {
        dc->setBounds(m_width, last_height, dc->getWidth(), dc->getHeight());
        last_height += dc->getHeight();
    }
}

void GuiComponent::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]
    
    if (buttonThatWasClicked == m_abort_button)
    {
        //[UserButtonCode_quitButton] -- add your button handler code here..
        
        ((ArnoldAnimationPatch*)m_rig->getSimulationPatch("ArnoldAnimationPatch"))->interruptRender();
        
        //[/UserButtonCode_quitButton]
    }
    /*
    else if (buttonThatWasClicked == m_switch_button) {
        if (m_switch_button->getButtonText() == "Animation") {            
            m_switch_button->setButtonText("Interactive Rendering");
        }
        else {
            m_animation_timer->stopTimer();
            ArnoldAnimationPatch *aap = (ArnoldAnimationPatch*)m_rig->getSimulationPatch("ArnoldAnimationPatch");
            aap->reset();
            aap->startInteractive();
            
            m_timer->startTimer(1000);
            m_switch_button->setButtonText("Animation");
        }
    }
    else if (buttonThatWasClicked == m_record_button &&
             m_switch_button->getButtonText() == "Interactive Rendering") {
        ArnoldAnimationPatch *aap = (ArnoldAnimationPatch*)m_rig->getSimulationPatch("ArnoldAnimationPatch");
        if (m_record_button->getButtonText() == "Start") {
            aap->reset();
            aap->startRecording();
            m_record_button->setButtonText("Stop");
        }
        else {
            aap->stop();
            m_timer->stopTimer();
            m_animation_timer->startTimer(1000.f / 48);
            m_record_button->setButtonText("Start");
        }
    }
     */
    
    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

//==============================================================================

int GuiComponent::addDevicePads() {
    if (m_rig == NULL)
        return 0;
    
    int height = 0;
    
    for (Device *device : m_rig->getDeviceRaw()) {
        DeviceComponent *dc = new DeviceComponent(device);
        addAndMakeVisible(dc);
        m_device_pads.add(dc);
        
        height += dc->getHeight();
    }
    
    return height;
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
