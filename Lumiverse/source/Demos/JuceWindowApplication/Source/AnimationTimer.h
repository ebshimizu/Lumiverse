//
//  AnimationTimer.h
//  JuceWindowApp
//
//  Created by Chenxi Liu on 7/14/14.
//
//

#ifndef __JuceWindowApp__AnimationTimer__
#define __JuceWindowApp__AnimationTimer__

#pragma once

#include <iostream>
#include "../JuceLibraryCode/JuceHeader.h"
#include "../../../LumiverseCore/LumiverseCore.h"

using namespace Lumiverse;

class GuiComponent;

enum GuiAnimationMode {
    INTERACTIVE, PLAYBACK
};

class AnimationTimer : public HighResolutionTimer {
public:
    AnimationTimer(GuiComponent *refresh_ptr, ArnoldFrameManager *frame_manager, float *interactive_buffer)
    : m_interactive_buffer(interactive_buffer), m_refresh_pointer(refresh_ptr),
    m_frame_manager(frame_manager), m_callback_count(0) { }
    ~AnimationTimer() { }
    
    void startInteractive() { m_mode = GuiAnimationMode::INTERACTIVE; }
    void startPlayback() { m_mode = GuiAnimationMode::PLAYBACK; reset(); }
    
    void hiResTimerCallback() override;
    
private:
    void reset();
    
    float *m_interactive_buffer;
    GuiComponent *m_refresh_pointer;
    ArnoldFrameManager *m_frame_manager;
    size_t m_callback_count;
    GuiAnimationMode m_mode;
};

#endif /* defined(__JuceWindowApp__AnimationTimer__) */
