/*
  ==============================================================================

    Lfo.h
    Created: 17 Dec 2025 10:00:43am
    Author:  Alexis ZBIK

  ==============================================================================
*/

#pragma once

#include "daisysp.h"

using namespace std;
using namespace daisysp;

class Lfo {
public:
    enum LfoDest {
        LfoDest_None = 0,
        LfoDest_Pitch,
        LfoDest_Feedback,
        LfoDest_Brightness,
        LfoDest_TimeRatio,
        
        LfoDest_Count
    };
    
    const char* destinationNames[5] = {
        "None", "Pitch", "Feedback", "Brightness", "Time Ratio"
    };
    
    enum LfoType {
        LfoType_Sin,
        LfoType_Random,
        
        LfoType_Count
    };
    
public:
    Lfo();
    ~Lfo() = default;

public:
    void init(double sampleRate);
    void process(size_t frameCount);
    
    void setAmount(float amount);
    void setRate(float rate);
    void setType(float typeValue);
    void retrigger();
    
    void setDestinationValue(float value);
    
    float getBuffer(LfoDest target, uint8_t frame, float multiplier = 1.f);
    
    const LfoDest& getDestination();

private:
    static constexpr uint8_t lfoBufSize = 128;
    float buffer[lfoBufSize];
    
    Oscillator osc;
    LfoDest dest;
};
