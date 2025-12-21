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
#include "Lfo.h"

//Destinations
/*
 
 lfos
 
 pitch
 octave
 feedback
 brightness
 timeratio
 
 op
 -> coarse
 -> fine
 -> amount

 
 */

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

#define LFO_PARAM(_name) \
LfoType##_name, \
LfoDestination##_name, \
LfoRate##_name, \
LfoAmount##_name

class PolyFMDSP : public DSPKernel {
public:
    enum Parameters {
        PlayMode,
        Glide,
        Algorithm,
        Feedback,
        TimeRatio,
        Brightness,
        Volume,
        
        OPERATOR_PARAM(A),
        OPERATOR_PARAM(B),
        OPERATOR_PARAM(C),
        OPERATOR_PARAM(D),
        
        LFO_PARAM(A),
        LFO_PARAM(B),
        
        EnvDestination,
        EnvAttack,
        EnvDecay,
        EnvAmount,

        Count
    };
    
    enum LfoDest {
        LfoDest_Pitch = 0,
        LfoDest_Octave,
        LfoDest_Feedback,
        LfoDest_Brightness,
        LfoDest_Timeratio,
        
        LfoDest_Count
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
    int getLfoParam(int lfoId, int aParam);
    float opTimeValue(int operatorId, int aParam, bool applyTimeRatio, float min = 0.002f, float max = 2.f);
    
private:
    PolySynth synth;
    float timeRatio = 0;
    
    static constexpr uint8_t lfoCount = 2;
    
    Lfo lfo[lfoCount];
    
    unsigned long timeStamp = 0;

};
