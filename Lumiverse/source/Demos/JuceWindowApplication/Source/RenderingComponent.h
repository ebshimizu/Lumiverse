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
    RenderingComponent();
    RenderingComponent(float *buffer, Image::PixelFormat format,
                       int imageWidth, int imageHeight);
    
    ~RenderingComponent() {

    }
    
    // TODO : add double buffer
    void paint (Graphics& g);
    
private:
    Image m_panel;
    float *m_color_buffer;
    
    ScopedPointer<Label> m_intensity_label;
    ScopedPointer<Slider> m_intensity_slider;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RenderingComponent)
};

#endif /* defined(__JuceWindowApp__RenderingComponent__) */
