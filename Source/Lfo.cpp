/*
  ==============================================================================

    Lfo.cpp
    Created: 17 Dec 2025 10:00:43am
    Author:  Alexis ZBIK

  ==============================================================================
*/

#include "Lfo.h"
#include "DaisyYMNK/DSP/DSP.h"

Lfo::Lfo() {
    uint8_t k = lfoBufSize;
    while (k--) {
        buffer[k] = 0;
    }
}


void Lfo::init(double sampleRate) {
    osc.Init(sampleRate);
    osc.SetAmp(1);
    osc.SetFreq(1);
}

void Lfo::setAmount(float amount) {
    osc.SetAmp(amount * amount);
}

void Lfo::setRate(float rate) {
    float mappedRate = ydaisy::valueMap(rate*rate*rate, 0.01f, 25.f);
    osc.SetFreq(mappedRate);
}

void Lfo::process(size_t frameCount) {
    size_t i = frameCount;
    while(i--) {
        buffer[i] = osc.Process();
    }
}

float Lfo::getBuffer(uint8_t frame) {
    return buffer[frame];
}
