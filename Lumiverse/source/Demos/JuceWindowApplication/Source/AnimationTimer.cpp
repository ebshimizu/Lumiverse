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
    // For thread safe
    const MessageManagerLock mmLock;
    if (m_mode == GuiAnimationMode::INTERACTIVE) {
        m_refresh_pointer->setBuffer(m_interactive_buffer);
        m_refresh_pointer->repaint();
    }
    else if (m_mode == GuiAnimationMode::PLAYBACK) {
        bool flag = false;
        time_t passed = m_callback_count * getTimerInterval();
    
        if (!m_frame_manager->hasNext()) {
            m_mode = GuiAnimationMode::INTERACTIVE;
        
            return ;
        }
    
        while (m_frame_manager->hasNext() &&
            m_frame_manager->getNextTime() <= passed) {
            flag = true;
            m_frame_manager->next();
        }
    
        if (flag) {
            std::cout << m_frame_manager->getCurrentTime() << std::endl;
    
            m_refresh_pointer->setBuffer(m_frame_manager->getCurrentFrameBuffer());
            m_refresh_pointer->repaint();
        }
    
        m_callback_count++;
    }
}

void AnimationTimer::reset() {
    m_frame_manager->reset();
    m_callback_count = 0;
}