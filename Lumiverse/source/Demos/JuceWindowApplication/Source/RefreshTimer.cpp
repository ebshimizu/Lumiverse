//
//  RenderingComponent.cpp
//  JuceWindowApp
//
//  Created by Chenxi Liu on 7/14/14.
//
//

#include "RenderingComponent.h"

void RenderingComponent::paint (Graphics& g) {
    if (m_color_buffer == NULL)
        return ;
    
    for (size_t y = 0; y < m_panel.getHeight(); y++) {
        for (size_t x = 0; x < m_panel.getWidth(); x++) {
            size_t pixelWidth = (m_panel.getFormat() == Image::PixelFormat::RGB) ? 3 : 4;
            size_t offset = (y * m_panel.getWidth() + x) * pixelWidth;
            Colour color(m_color_buffer[offset], m_color_buffer[offset + 1], m_color_buffer[offset + 2]);
            m_panel.setPixelAt(x, y, color);
        }
    }
    
    g.drawImageAt(m_panel, 0, 0);
}