//
//  RenderingComponent.h
//  JuceWindowApp
//
//  Created by Chenxi Liu on 7/14/14.
//
//

#ifndef __JuceWindowApp__RepaintTimer__
#define __JuceWindowApp__RepaintTimer__

#pragma once

#include <iostream>
#include "../JuceLibraryCode/JuceHeader.h"

class RepaintTimer : public Timer {
public:
    RepaintTimer(Component *refresh_ptr)
    : m_refreshPointer(refresh_ptr) { }
    ~RepaintTimer() { }
    
    virtual void timerCallback();
    
private:
    Component *m_refreshPointer;
};

#endif /* defined(__JuceWindowApp__RepaintTimer__) */
