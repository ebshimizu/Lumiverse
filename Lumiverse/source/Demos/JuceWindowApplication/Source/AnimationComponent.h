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

#ifndef __JUCE_HEADER_AnimationComponent__
#define __JUCE_HEADER_AnimationComponent__

#pragma once

//[Headers]     -- You can add your own extra header files here --
#include <vector>
#include "JuceHeader.h"
#include "GuiConfig.h"
#include "../../../LumiverseCore/LumiverseCore.h"
#include "RepaintTimer.h"
#include "AnimationTimer.h"
//[/Headers]

using namespace Lumiverse;

class GuiComponent;
//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Introjucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class AnimationComponent  : public Component,
                      public ButtonListener
{
public:
    //==============================================================================
    AnimationComponent (Rig *rig, GuiComponent *parent);
    ~AnimationComponent();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void buttonClicked (Button* buttonThatWasClicked);
    
    void onPlaybackFinished();
    
    void onRenderingFinished();

private:
    void loadImages();
    
    void setActivatePlayButton();
    void setUnactivatePlayButton();
    void setStoppedPlayButton();
    
    void triggleFirstFrame() const;
    
    ScopedPointer<ImageButton> m_play_button;
    ScopedPointer<ImageButton> m_record_button;
    ScopedPointer<LookAndFeel> m_lookandfeel;
    
    ScopedPointer<Image> m_record_image;
    ScopedPointer<Image> m_stop_image;
    ScopedPointer<Image> m_play_image;
    ScopedPointer<Image> m_play_stop_image;
    
    Rig *m_rig;
    ScopedPointer<AnimationTimer> m_animation_timer;
    const int m_component_height = 50;
	const String m_record_path = File::getCurrentWorkingDirectory().getFullPathName() + "/../../Resource/record.png";
	const String m_stop_path = File::getCurrentWorkingDirectory().getFullPathName() + "/../../Resource/stop.png";
	const String m_play_path = File::getCurrentWorkingDirectory().getFullPathName() + "/../../Resource/play.png";
	const String m_play_stop_path = File::getCurrentWorkingDirectory().getFullPathName() + "/../../Resource/play_stop.png";
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnimationComponent)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

#endif   // __JUCE_HEADER_AnimationComponent__
