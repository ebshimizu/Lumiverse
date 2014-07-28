//
//  RenderingComponent.cpp
//  JuceWindowApp
//
//  Created by Chenxi Liu on 7/14/14.
//
//

#include "AnimationTimer.h"

void AnimationTimer::timerCallback() {
    bool flag = false;
    time_t passed = m_callback_count * getTimerInterval();
    
    if (!m_frame_manager->hasNext()) {
        std::cout << "Reseted" << std::endl;
        m_callback_count = 0;
        m_frame_manager->reset();
        
        return ;
    }
    
    while (m_frame_manager->hasNext() &&
           m_frame_manager->getNextTime() <= passed) {
        flag = true;
        m_frame_manager->next();
    }
    
    if (flag) {
        std::cout << m_frame_manager->getCurrentTime() << std::endl;
    
        m_refreshPointer->setBuffer(m_frame_manager->getCurrentFrameBuffer());
        m_refreshPointer->repaint();
    }
    
    m_callback_count++;
}