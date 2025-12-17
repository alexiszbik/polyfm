/*
  ==============================================================================

    PolySynth.h
    Created: 10 Jan 2024 3:37:02pm
    Author:  Alexis ZBIK

  ==============================================================================
*/

#pragma once

#include "SynthVoice.h"
#include "DaisyYMNK/Common/Common.h"
#include "daisysp.h"

#define VOICE_COUNT 4
#define UNISON_VOICE_COUNT 3

using namespace std;
using namespace daisysp;

class PolySynth {
public:
    enum EPolyMode {
        Mono = 0,
        Unison,
        Poly
    };

public:
    PolySynth();
    ~PolySynth();

public:
    void init(double sampleRate);
    void setNote(bool isNoteOn, Note note);
    float process();
    
    void setPitchBend(float bend);
    void setModWheel(float value);
    void setPolyMode(EPolyMode newPolyMode);
    void setGlide(float glide);
    void setTune(float tune);
    
    void setOperatorRatio(int operatorId, float ratio);
    void setOperatorAmount(int operatorId, float amount);
    void setOperatorADSR(int operatorId, float attack, float decay, float sustain, float release);
    void setFeedback(float feedbackAmount);
    void setAlgorithm(int algorithmIndex);
    void setBrightness(float brightness);
    void setEnvParameters(float attack, float decay, float amount);

private:
    EPolyMode polyMode = Mono;
    vector<SynthVoice*> voices;
    
    float tune = 0;
    
    SmoothValue bend;
    SmoothValue vibratoAmount;
    
    Oscillator modulation;
    
    vector<Note> noteState;
    
    static constexpr int smoothGlobal = 800;
};
