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
    m_record_button->addShortcut (KeyPress (KeyPress::returnKey));
    
    ArnoldAnimationPatch *aap = (ArnoldAnimationPatch*)m_rig->getSimulationPatch("ArnoldAnimationPatch");
    
    addAndMakeVisible (m_play_button = new ImageButton (String::empty));
    m_play_button->addListener (this);
    m_play_button->addShortcut (KeyPress (KeyPress::spaceKey));
    setUnactivatePlayButton();
    
    setSize (parent->getWidth(), m_component_height);
    
    ArnoldAnimationPatch::FinishedCallbackFunction callback = std::bind(&AnimationComponent::onRenderingFinished,
                                                        this);
    aap->addFinishedCallback(callback);

    m_animation_timer = new AnimationTimer(parent, aap, aap->getBufferPointer());
    m_animation_timer->startTimer(1000.f / 48);
    
    AnimationTimer::FinishedCallbackFunction timer_callback = std::bind(&AnimationComponent::onPlaybackFinished,
                                                                        this);
    m_animation_timer->addFinishedCallback(timer_callback);
    m_animation_timer->startInteractive();
    aap->startInteractive();
}

AnimationComponent::~AnimationComponent()
{
    ArnoldAnimationPatch *aap = (ArnoldAnimationPatch*)m_rig->getSimulationPatch("ArnoldAnimationPatch");
    aap->close();
    
    m_animation_timer->stopTimer();
    m_animation_timer = nullptr;
    
    m_record_button = nullptr;
    m_play_button = nullptr;
    m_lookandfeel = nullptr;
    
    m_record_image = nullptr;
    m_stop_image = nullptr;
    m_play_image = nullptr;
    m_play_stop_image = nullptr;
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
    
    // Draws time
    Font f("Eurostile", 30.f, Font::bold);
    if (m_animation_timer->isInteractive())
        g.setColour(baseColour.brighter(0.6f));
    else
        g.setColour(baseColour.brighter(3.5f));
    g.setFont(f);
    char formattedtime[6];
    sprintf(formattedtime, "%02ld:%02ld", m_animation_timer->getMinutes(), m_animation_timer->getSeconds());
    g.drawSingleLineText(TRANS(formattedtime), 20 + m_play_button->getWidth() + 10 + m_play_button->getWidth() + 10,
                         (getHeight() - f.getHeight()) * 1.8f);
}

void AnimationComponent::resized()
{
    m_record_button->setSize(m_record_image->getWidth(), m_record_image->getHeight());
    m_play_button->setSize(m_play_button->getWidth(), m_play_button->getHeight());
    
    m_record_button->setTopLeftPosition(20 + m_play_button->getWidth() + 10,
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
            m_animation_timer->reset();
            setUnactivatePlayButton();
            
            m_record_button->setButtonText("Stop");
            m_record_button->setImages (true, true, true,
                                        *m_stop_image, 0.9f, Colours::transparentBlack,
                                        *m_stop_image, 1.0f, Colours::transparentBlack,
                                        *m_stop_image, 1.0f, Colours::coral.withAlpha (0.8f),
                                        0.5f);
            aap->rerender();
        }
        else {
            aap->endRecording();

            m_animation_timer->startInteractive();
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
            aap->getMode() != ArnoldAnimationMode::RENDERING) {
            aap->stop();
            m_animation_timer->startPlayback();
            
            m_play_button->setImages (true, true, true,
                                      *m_play_stop_image, 0.9f, Colours::transparentBlack,
                                      *m_play_stop_image, 1.0f, Colours::transparentBlack,
                                      *m_play_stop_image, 1.0f, Colours::coral.withAlpha (0.8f),
                                      0.5f);
            m_play_button->setButtonText("Stop");
        }
        else if (m_play_button->getButtonText() == "Stop") {
            if (aap->getMode() == ArnoldAnimationMode::RENDERING)
                return;
            
            onPlaybackFinished();
            
            /*
            GuiComponent *parent = (GuiComponent *)this->getParentComponent();
            parent->setBuffer(aap->getBufferPointer());
            */
        }
    }
}

void AnimationComponent::loadImages() {
    File record_file(m_record_path);
    File stop_file(m_stop_path);
    File play_file(m_play_path);
    File play_stop_file(m_play_stop_path);
    
    m_record_image = new Image(ImageFileFormat::loadFrom(record_file));
    m_stop_image = new Image(ImageFileFormat::loadFrom(stop_file));
    m_play_image = new Image(ImageFileFormat::loadFrom(play_file));
    
    m_play_stop_image = new Image(ImageFileFormat::loadFrom(play_stop_file));
}

void AnimationComponent::setActivatePlayButton() {
    // For thread safe
    const MessageManagerLock mmLock;
    m_play_button->setImages (true, true, true,
                              *m_play_image, 0.9f, Colours::transparentBlack,
                              *m_play_image, 1.0f, Colours::transparentBlack,
                              *m_play_image, 1.0f, Colours::coral.withAlpha (0.8f),
                              0.5f);
    m_play_button->setButtonText("Play");
}

void AnimationComponent::setUnactivatePlayButton() {
    // For thread safe
    const MessageManagerLock mmLock;
    m_play_button->setImages (true, true, true,
                              *m_play_image, 0.5f, Colours::transparentBlack,
                              *m_play_image, 0.5f, Colours::transparentBlack,
                              *m_play_image, 0.5f, Colours::transparentBlack,
                              0.5f);
    m_play_button->setButtonText("play");
}

void AnimationComponent::setStoppedPlayButton() {
    // For thread safe
    const MessageManagerLock mmLock;
    m_play_button->setImages (true, true, true,
                              *m_play_stop_image, 0.9f, Colours::transparentBlack,
                              *m_play_stop_image, 1.0f, Colours::transparentBlack,
                              *m_play_stop_image, 1.0f, Colours::coral.withAlpha (0.8f),
                              0.5f);
    m_play_button->setButtonText("Stop");
}

void AnimationComponent::onPlaybackFinished() {
    ArnoldAnimationPatch *aap = (ArnoldAnimationPatch*)m_rig->getSimulationPatch("ArnoldAnimationPatch");
    
    m_animation_timer->startInteractive();
    aap->reset();
    aap->startInteractive();
    
    if (aap->getFrameManager()->isEmpty())
        setUnactivatePlayButton();
    else
        setActivatePlayButton();
}

void AnimationComponent::onRenderingFinished() {
    setActivatePlayButton();
}

void AnimationComponent::triggleFirstFrame() const {
    // To triggle param changed (doesn't really change param),
    // so the first frame can be stored.
    if (m_rig->getDeviceRaw().size() > 0) {
        auto i = m_rig->getDeviceRaw().begin();
        for (auto p : (*i)->getRawParameters()) {
            if (p.second->getTypeName() == "float") {
                (*i)->setParam(p.first, p.second);
                break;
            }
        }
    }
}