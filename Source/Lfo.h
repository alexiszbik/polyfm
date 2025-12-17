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
    Lfo();
    ~Lfo() = default;

public:
    void init(double sampleRate);
    void process(size_t frameCount);
    
    void setAmount(float amount);
    void setRate(float rate);
    
    float getBuffer(uint8_t frame);

private:
    Oscillator lfo;
    static constexpr uint8_t lfoBufSize = 128;
    float buffer[lfoBufSize];
    
    Oscillator osc;
};
