//
//  RenderingComponent.h
//  JuceWindowApp
//
//  Created by Chenxi Liu on 7/14/14.
//
//

#ifndef __JuceWindowApp__RenderingComponent__
#define __JuceWindowApp__RenderingComponent__

#include <iostream>
#include "../JuceLibraryCode/JuceHeader.h"

class RenderingComponent : public Component {
public:
    RenderingComponent(float *buffer, Image::PixelFormat format, int imageWidth, int imageHeight)
    : m_panel(format, imageWidth, imageHeight, true), m_color_buffer(buffer) {
        setSize (imageWidth, imageHeight);
    }
    ~RenderingComponent() { }
    
    // TODO : add double buffer
    void paint (Graphics& g);
    
private:
    Image m_panel;
    float *m_color_buffer;
};

#endif /* defined(__JuceWindowApp__RenderingComponent__) */
