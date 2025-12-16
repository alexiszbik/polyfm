/*
  ==============================================================================

    MultiOscDSP.h
    Created: 8 Nov 2023 4:51:13pm
    Author:  Alexis ZBIK

  ==============================================================================
*/

#pragma once

#include "DaisyYMNK/DSP/DSP.h"
#include "PolySynth.h"

#include "daisysp.h"

using namespace daisysp;
using namespace ydaisy;

#define MIDI_CC_START 10

#define OPERATOR_PARAM(_name) \
Coarse##_name, \
Fine##_name, \
Mode##_name, \
Attack##_name, \
Decay##_name, \
Sustain##_name, \
Release##_name, \
Amount##_name


class PolyFMDSP : public DSPKernel {
public:
    enum Parameters {
        PlayMode,
        Glide,
        Algorithm,
        Feedback,
        TimeRatio,
        Brightness,
        //ChorusState,
        
        OPERATOR_PARAM(A),
        OPERATOR_PARAM(B),
        OPERATOR_PARAM(C),
        OPERATOR_PARAM(D),

        Count
    };
    
public:
    PolyFMDSP();
    ~PolyFMDSP();
    
public:
    virtual void init(int channelCount, double sampleRate) override;
    virtual void process(float** buf, int frameCount) override;
    virtual void processMIDI(MIDIMessageType messageType, int channel, int dataA, int dataB) override;
    
protected:
    virtual void updateParameter(int index, float value) override;
    
private:
    int getOpParam(int operatorId, int aParam);
    float opIimeValue(int operatorId, int aParam, float min = 0.002f, float max = 2.f);
    
private:
    PolySynth synth;
    //Chorus chorus;
    
    unsigned long timeStamp = 0;
    
    /*Preset<const char*> preset = {
        {"PlayMode", 0.989821},
        {"Glide", 0},
        {"Algorithm", 0},
        {"Feedback", 0.394465},
        {"TimeRatio", 0},
        {"Brightness", 0.929108},
        //{"ChorusState", 1},
        {"CoarseA", 0.113132},
        {"FineA", 0},
        {"AttackA", 0.917869},
        {"DecayA", 0.425301},
        {"SustainA", 1},
        {"ReleaseA", 0.371905},
        {"AmountA", 0.402244},
        {"CoarseB", 0.0814685},
        {"FineB", 0},
        {"AttackB", 0.93505},
        {"DecayB", 0},
        {"SustainB", 0.836318},
        {"ReleaseB", 0.476794},
        {"AmountB", 0.789774},
        {"CoarseC", 0.0529164},
        {"FineC", 0},
        {"AttackC", 1},
        {"DecayC", 0},
        {"SustainC", 1},
        {"ReleaseC", 0.346365},
        {"AmountC", 0.455575},
        {"CoarseD", 0.0811209},
        {"FineD", 0.00324417},
        {"AttackD", 1},
        {"DecayD", 0},
        {"SustainD", 1},
        {"ReleaseD", 0.603648},
        {"AmountD", 0.644962},
        };*/
    
};
