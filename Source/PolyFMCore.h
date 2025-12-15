/*
  ==============================================================================

    PolyFMCore.h
    Created: 19 Jan 2024 10:00:43am
    Author:  Alexis ZBIK

  ==============================================================================
*/

#pragma once

#include "DaisyYMNK/DSP/DSP.h"
#include "PolyFMDSP.h"

#define DSP_PARAM_OP(_name) \
PolyFMDSP::Coarse##_name, \
PolyFMDSP::Fine##_name, \
PolyFMDSP::Mode##_name, \
PolyFMDSP::Amount##_name, \
PolyFMDSP::Attack##_name, \
PolyFMDSP::Decay##_name, \
PolyFMDSP::Sustain##_name, \
PolyFMDSP::Release##_name


class PolyFMCore : public ModuleCore {
public:
    enum {
        MuxKnob_1 = 0,
        MuxKnob_2,
        MuxKnob_3,
        MuxKnob_4,
        MuxKnob_5,
        MuxKnob_6,
        MuxKnob_7,
        MuxKnob_8,
        MuxKnob_9,
        MuxKnob_10,
        MuxKnob_11,
        MuxKnob_12,
        MuxKnob_13,
        MuxKnob_14,
        MuxKnob_15,
        MuxKnob_16,
        
        KnobFeedback,
        KnobTimeRatio,
        KnobBrightness,
        
        ButtonPreviousOperator,
        ButtonNextOperator,
        /*ButtonPreviousPreset,
        ButtonNextPreset*/
        MidiLed
    };
public:
    PolyFMCore();

    int getCurrentOpIdx();

    virtual void processMIDI(MIDIMessageType messageType, int channel, int dataA, int dataB) override;
    
protected:
    void updateHIDValue(unsigned int index, float value) override;
    
private:
    void lockAllKnobs();
    void setCurrentOperators(unsigned int opIndex);
    
private:
    vector<int> parameterMap = {
        PolyFMDSP::Feedback,
        PolyFMDSP::TimeRatio,
        PolyFMDSP::Brightness,
    };
    
    vector<vector<int>> opParameterMap = {
        {DSP_PARAM_OP(A), DSP_PARAM_OP(B)},
        {DSP_PARAM_OP(C), DSP_PARAM_OP(D)}
    };
    
    int currentOpsIndex = 0;

};
