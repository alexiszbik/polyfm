/*
  ==============================================================================

    SynthVoice.h
    Created: 10 Jan 2024 2:48:00pm
    Author:  Alexis ZBIK

  ==============================================================================
*/

#pragma once

#include "DaisyYMNK/DaisyYMNK.h"
#include "daisysp.h"
#include "FmAlgorithm.h"

using namespace ydaisy;
using namespace std;

using namespace daisysp;
//using namespace ydaisy;

class SynthVoice {
public:
    void init(double sampleRate);
    
    void processPhase(float* phase, float ratio);
    
    void setGlide(float glide);
    
    void setOperatorRatio(int operatorId, float ratio);
    void setOperatorAmount(int operatorId, float amount);
    void setOperatorADSR(int operatorId, float attack, float decay, float sustain, float release);
    void setFeedback(float feedbackAmount);
    void setAlgorithm(int index);
    void setBrightness(float brightness);
    
    void setNoteOn(Note note);
    void setNoteOff();
    
    float process();
    
    //TO REWRITE
    int currentPitch() {
        return pitch.getGoal();
    }
    
    bool isPlaying() {
        return gate || op[0].adsr.IsRunning();
    }
    
private:
    void setPitch(int pitch);
    void setGate(bool gate);
    
public:
    unsigned long noteTimeStamp; //TO REWRITE
    float pitchMod = 0; //TO REWRITE
    
    static const int kAlgorithmCount = 11;
    static const int kOperatorCount = 4;
    
private:
    double sampleRate;
    
    float glide = 0;
    
    struct Operator {
        Adsr adsr;
        float ratio = 1;
        float amount = 1;
        float phase = 0;
    };
    
    
    float opOut[kOperatorCount] = {0,0,0,0};

    SmoothValue pitch;
    bool gate = false;

    Operator op[kOperatorCount];
    
    float phaseInc = 0;
    float freq = 0;
    float feedback = 0;
    float feedbackAmount = 0;
    float brigthness = 0;
    
    //THIS COULD BE STATIC
    FmAlgorithm algorithms[kAlgorithmCount] = {
        FmAlgorithm({0}, {1}, {2}, {3}, 3),
        FmAlgorithm({0}, {1}, {2,3}, {}, 3),
        FmAlgorithm({0}, {1,3}, {2}, {}, 2),
        FmAlgorithm({0}, {1,2}, {3}, {3}, 3),
        FmAlgorithm({0, 1}, {2}, {2}, {3}, 3),
        FmAlgorithm({0, 1}, {}, {2}, {3}, 3),
        FmAlgorithm({0}, {1, 2, 3}, {}, {}, 3),
        FmAlgorithm({0, 1}, {2}, {3}, {}, 3),
        FmAlgorithm({0, 1, 2}, {3}, {3}, {3}, 3),
        FmAlgorithm({0, 1, 2}, {}, {}, {3}, 3),
        FmAlgorithm({0, 1, 2, 3}, {}, {}, {}, 3)
    };
    
    int selectedAlgorithm = 0;
    
};
