//
//  AnimationTimer.h
//  JuceWindowApp
//
//  Created by Chenxi Liu on 7/14/14.
//
//

#ifndef __JuceWindowApp__AnimationTimer__
#define __JuceWindowApp__AnimationTimer__

#pragma once

#include <iostream>
#include <chrono>
#include "../JuceLibraryCode/JuceHeader.h"
#include "../../../LumiverseCore/LumiverseCore.h"

using namespace Lumiverse;
using namespace std;

class GuiComponent;
class AnimationComponent;

enum GuiAnimationMode {
    INTERACTIVE, PLAYBACK
};

class AnimationTimer : public HighResolutionTimer {
public:
    AnimationTimer(GuiComponent *refresh_ptr, ArnoldAnimationPatch *animation_patch, float *interactive_buffer)
    : m_interactive_buffer(interactive_buffer),
    m_refresh_pointer(refresh_ptr), 
    m_animation_patch(animation_patch), m_start(chrono::high_resolution_clock::now()) { }
    ~AnimationTimer() { }
    
    void startInteractive() { m_mode = GuiAnimationMode::INTERACTIVE; }
    void startPlayback() { reset(); m_mode = GuiAnimationMode::PLAYBACK; }
    
    void hiResTimerCallback() override;
    
    time_t getMinutes() const;
    time_t getSeconds() const;
    
    bool isInteractive() const {
        return (m_animation_patch->getMode() == ArnoldAnimationMode::INTERACTIVE);
    }
    
    typedef function<void()> FinishedCallbackFunction;
    
    /*!
     * \brief Registers a callback function for parameter changed event.
     *
     * All registered functinos would be called when a parameter is changed
     * by Device::setParam and Device::reset function.
     * \param func The callback function.
     * \return The int id for the registered function.
     * \sa addMetadataChangedCallback(DeviceCallbackFunction func)
     */
    int addFinishedCallback(FinishedCallbackFunction func);
    
    /*!
     * \brief Deletes a registered callback for parameter change
     *
     * \param id The id returned when the callback is registered
     * \sa addParameterChangedCallback(DeviceCallbackFunction func)
     */
    void deleteFinishedCallback(int id);
    
    void reset();
    
private:
    void onFinished();
    
    float *m_interactive_buffer;
    GuiComponent *m_refresh_pointer;
    
    ArnoldAnimationPatch *m_animation_patch;
    GuiAnimationMode m_mode;
    chrono::time_point<chrono::high_resolution_clock> m_start;
    chrono::time_point<chrono::high_resolution_clock> m_current;
    
    map<int, FinishedCallbackFunction> m_onFinishedFunctions;
};

#endif /* defined(__JuceWindowApp__AnimationTimer__) */
