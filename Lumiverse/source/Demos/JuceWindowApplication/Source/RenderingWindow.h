//
//  RenderingWindow.h
//  JuceWindowApp
//
//  Created by Chenxi Liu on 7/14/14.
//
//

#ifndef __JuceWindowApp__RenderingWindow__
#define __JuceWindowApp__RenderingWindow__

#include <iostream>
#include "../JuceLibraryCode/JuceHeader.h"
#include "GuiComponent.h"
#include "../../../LumiverseCore/LumiverseCore.h"
//[/Headers]

using namespace Lumiverse;

class RenderingWindow  : public DocumentWindow
{
public:
    //==============================================================================
    RenderingWindow(int width, int height, float *buffer, Rig *rig)
    : DocumentWindow ("Arnold Result",
                      Colours::lightgrey,
                      DocumentWindow::allButtons,
                      true)
    {
        // Create an instance of our main content component, and add it to our window..
        //setContentOwned (new RenderingComponent(buffer, Image::PixelFormat::ARGB, width, height), true);
        
        setContentOwned(new GuiComponent(buffer, Image::PixelFormat::ARGB, width, height, rig), true);
        
        // Centre the window on the screen
        centreWithSize (getWidth(), getHeight());
        
        // And show it!
        setVisible (true);
    }
    
    ~RenderingWindow()
    {
        // (the content component will be deleted automatically, so no need to do it here)
    }
    
    //==============================================================================
    void closeButtonPressed()
    {
        // When the user presses the close button, we'll tell the app to quit. This
        // HelloWorldWindow object will be deleted by the JUCEHelloWorldApplication class.
        JUCEApplication::quit();
    }
};

#endif /* defined(__JuceWindowApp__RenderingWindow__) */
