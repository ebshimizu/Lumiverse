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

#include "AnimationComponent.h"
#include "GuiComponent.h"

//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
AnimationComponent::AnimationComponent (Rig *rig, GuiComponent *parent)
: m_rig(rig)
{
    setLookAndFeel(m_lookandfeel = new juce::LookAndFeel_V3());
    
    loadImages();
    
    addAndMakeVisible (m_record_button = new ImageButton (String::empty));
    m_record_button->setButtonText ("Start");
    m_record_button->addListener (this);
    m_record_button->setImages (true, true, true,
                   *m_record_image, 0.9f, Colours::transparentBlack,
                   *m_record_image, 1.0f, Colours::transparentBlack,
                   *m_record_image, 1.0f, Colours::coral.withAlpha (0.8f),
                   0.5f);
    
    
    addAndMakeVisible (m_play_button = new ImageButton (String::empty));
    m_play_button->setButtonText ("Play");
    m_play_button->addListener (this);
    m_play_button->setImages (true, true, true,
                                *m_play_image, 0.9f, Colours::transparentBlack,
                                *m_play_image, 1.0f, Colours::transparentBlack,
                                *m_play_image, 1.0f, Colours::coral.withAlpha (0.8f),
                                0.5f);
    
    setSize (parent->getWidth(), m_component_height);
    
    ArnoldAnimationPatch *aap = (ArnoldAnimationPatch*)m_rig->getSimulationPatch("ArnoldAnimationPatch");
    m_animation_timer = new AnimationTimer(parent, aap->getFrameManager());
    //aap->startInteractive();
    
    m_timer = new RepaintTimer(parent);
    m_timer->startTimer(1000);
}

AnimationComponent::~AnimationComponent()
{
    ArnoldAnimationPatch *aap = (ArnoldAnimationPatch*)m_rig->getSimulationPatch("ArnoldAnimationPatch");
    aap->close();
    
    m_animation_timer->stopTimer();
    m_timer->stopTimer();
    
    m_animation_timer = nullptr;
    m_timer = nullptr;
    
    m_record_button = nullptr;
    m_play_button = nullptr;
    m_lookandfeel = nullptr;
    
    m_record_image = nullptr;
    m_stop_image = nullptr;
    m_play_image = nullptr;
}

//==============================================================================
void AnimationComponent::paint (Graphics& g)
{
    Path outline;
    outline.addRectangle (0, 0, getWidth(), getHeight());
    Colour baseColour = Colour::fromFloatRGBA(0.1f, 0.1f, 0.1f, 1.f);
    
    g.setGradientFill (ColourGradient (baseColour, 0.0f, 0.0f,
                                       baseColour.brighter(0.4f), 0.0f, getHeight(),
                                       false));
    g.fillPath (outline);
}

void AnimationComponent::resized()
{
    m_record_button->setSize(m_record_image->getWidth(), m_record_image->getHeight());
    m_play_button->setSize(m_play_button->getWidth(), m_play_button->getHeight());
    
    m_record_button->setTopLeftPosition(getWidth() / 2 - m_record_button->getWidth() / 2,
                                (getHeight() - m_record_button->getHeight()) / 2);
    m_play_button->setTopLeftPosition(20, (getHeight() - m_play_button->getHeight()) / 2);
}

void AnimationComponent::buttonClicked (Button* buttonThatWasClicked)
{
    ArnoldAnimationPatch *aap = (ArnoldAnimationPatch*)m_rig->getSimulationPatch("ArnoldAnimationPatch");
    
    if (buttonThatWasClicked == m_record_button) {
        
        if (m_record_button->getButtonText() == "Start") {
            aap->reset();
            aap->startRecording();
            m_record_button->setButtonText("Stop");
            m_record_button->setImages (true, true, true,
                                        *m_stop_image, 0.9f, Colours::transparentBlack,
                                        *m_stop_image, 1.0f, Colours::transparentBlack,
                                        *m_stop_image, 1.0f, Colours::coral.withAlpha (0.8f),
                                        0.5f);
        }
        else {
            aap->endRecording();
            
            if (!aap->isWorking()) {
                aap->startInteractive();
            
                GuiComponent *parent = (GuiComponent *)this->getParentComponent();
                parent->setBuffer(aap->getBufferPointer());
                m_timer->startTimer(1000);
            }
            
            m_record_button->setImages (true, true, true,
                                        *m_record_image, 0.9f, Colours::transparentBlack,
                                        *m_record_image, 1.0f, Colours::transparentBlack,
                                        *m_record_image, 1.0f, Colours::coral.withAlpha (0.8f),
                                        0.5f);
            m_record_button->setButtonText("Start");
        }
    }
    else if (buttonThatWasClicked == m_play_button) {
        if (m_record_button->getButtonText() == "Start" &&
            m_play_button->getButtonText() == "Play" &&
            !aap->isWorking()) {
            m_timer->stopTimer();
            m_animation_timer->startTimer(1000.f / 48);
            m_play_button->setButtonText("Stop");
        }
        else if (m_play_button->getButtonText() == "Stop") {
            if (aap->isWorking())
                return;
            m_animation_timer->stopTimer();

            aap->reset();
            aap->startInteractive();
            
            GuiComponent *parent = (GuiComponent *)this->getParentComponent();
            parent->setBuffer(aap->getBufferPointer());
            m_timer->startTimer(1000);
            
            m_play_button->setButtonText("Start");
        }
    }
}


void AnimationComponent::loadImages() {
    File record_file(m_record_path);
    File stop_file(m_stop_path);
    File play_file(m_play_path);
    
    m_record_image = new Image(ImageFileFormat::loadFrom(record_file));
    m_stop_image = new Image(ImageFileFormat::loadFrom(stop_file));
    m_play_image = new Image(ImageFileFormat::loadFrom(play_file));
}

