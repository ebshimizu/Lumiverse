//
//  AnimationTimer.cpp
//  JuceWindowApp
//
//  Created by Chenxi Liu on 7/14/14.
//
//

#include "AnimationTimer.h"
#include "GuiComponent.h"

void AnimationTimer::hiResTimerCallback() {
    ArnoldFrameManager *frame_manager = m_animation_patch->getFrameManager();
    ArnoldAnimationMode mode = m_animation_patch->getMode();
    
    // For thread safe
    const MessageManagerLock mmLock;
    if (m_mode == GuiAnimationMode::INTERACTIVE) {
        m_refresh_pointer->setBuffer(m_interactive_buffer);
        
        if (mode == ArnoldAnimationMode::RECORDING) {
            m_current= chrono::high_resolution_clock::now();
        }
    }
    else if (m_mode == GuiAnimationMode::PLAYBACK) {
        bool flag = false;
        m_current= chrono::high_resolution_clock::now();
        time_t passed = chrono::duration_cast<chrono::milliseconds>(m_current - m_start).count();
        
        if (!frame_manager->hasNext()) {
            m_mode = GuiAnimationMode::INTERACTIVE;
            onFinished();
            
            return ;
        }
    
        while (frame_manager->hasNext() &&
            frame_manager->getNextTime() <= passed) {
            flag = true;
            frame_manager->next();
        }
    
        if (flag) {
            std::cout << frame_manager->getCurrentTime() << std::endl;
    
            m_refresh_pointer->setBuffer(frame_manager->getCurrentFrameBuffer());
        }
    }
    
    m_refresh_pointer->repaint();
}

time_t AnimationTimer::getMinutes() const {
    ArnoldAnimationMode mode = m_animation_patch->getMode();
    
    if (mode == ArnoldAnimationMode::INTERACTIVE)
        return 0;
    time_t min = chrono::duration_cast<chrono::minutes>(m_current - m_start).count();
    return (min < 0) ? 0 : min;
}

time_t AnimationTimer::getSeconds() const {
    ArnoldAnimationMode mode = m_animation_patch->getMode();
    
    if (mode == ArnoldAnimationMode::INTERACTIVE)
        return 0;
    time_t sec = chrono::duration_cast<chrono::seconds>(m_current - m_start).count();
    return (sec < 0) ? 0 : sec;
}

void AnimationTimer::reset() {
    m_animation_patch->getFrameManager()->reset();
    m_start = chrono::high_resolution_clock::now();
}

void AnimationTimer::onFinished() {
    for (auto func : m_onFinishedFunctions) {
        func.second();
    }
}

int AnimationTimer::addFinishedCallback(FinishedCallbackFunction func) {
    size_t id = m_onFinishedFunctions.size();
    m_onFinishedFunctions[id] = func;
    
    return id;
}

void AnimationTimer::deleteFinishedCallback(int id) {
    if (m_onFinishedFunctions.count(id) > 0) {
        m_onFinishedFunctions.erase(id);
    }
}