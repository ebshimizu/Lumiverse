//
//  RenderingComponent.h
//  JuceWindowApp
//
//  Created by Chenxi Liu on 7/14/14.
//
//

#ifndef __JuceWindowApp__RepaintTimer__
#define __JuceWindowApp__RepaintTimer__

#include <iostream>
#include "../JuceLibraryCode/JuceHeader.h"

class RepaintTimer : public Timer {
public:
    RepaintTimer(ScopedPointer<Component> refresh_ptr)
    : m_refreshPointer(refresh_ptr) { int i; }
    ~RepaintTimer() { }
    
    virtual void timerCallback();
    
private:
    ScopedPointer<Component> m_refreshPointer;
};

#endif /* defined(__JuceWindowApp__RefreshTimer__) */
