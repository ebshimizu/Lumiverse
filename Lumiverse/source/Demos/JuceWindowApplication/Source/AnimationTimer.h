//
//  RenderingComponent.h
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
#include "GuiComponent.h"
#include "../../../LumiverseCore/LumiverseCore.h"

using namespace Lumiverse;

class AnimationTimer : public Timer {
public:
    AnimationTimer(ScopedPointer<GuiComponent> refresh_ptr, ArnoldFrameManager *frame_manager)
    : m_refreshPointer(refresh_ptr), m_frame_manager(frame_manager), m_callback_count(0) { }
    ~AnimationTimer() { }
    
    virtual void timerCallback();
    
private:
    ScopedPointer<GuiComponent> m_refreshPointer;
    ArnoldFrameManager *m_frame_manager;
    size_t m_callback_count;
};

#endif /* defined(__JuceWindowApp__AnimationTimer__) */
