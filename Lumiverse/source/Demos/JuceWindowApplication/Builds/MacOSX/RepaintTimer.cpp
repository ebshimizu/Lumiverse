//
//  RenderingComponent.cpp
//  JuceWindowApp
//
//  Created by Chenxi Liu on 7/14/14.
//
//

#include "RepaintTimer.h"

void RepaintTimer::timerCallback() {
    m_refreshPointer->repaint();
}