/*
  ==============================================================================

    PolyFMCore.h
    Created: 19 Jan 2024 10:00:43am
    Author:  Alexis ZBIK

  ==============================================================================
*/

#pragma once

#include "DaisyYMNK/DaisyYMNK.h"
#include "PolyFMDSP.h"

#define DSP_PARAM_OP(_name) \
PolyFMDSP::Coarse##_name, \
PolyFMDSP::Fine##_name, \
PolyFMDSP::Attack##_name, \
PolyFMDSP::Decay##_name, \
PolyFMDSP::Sustain##_name, \
PolyFMDSP::Release##_name, \
PolyFMDSP::Amount##_name

class PolyFMCore : public ModuleCore {
public:
    enum {
        KnobGlide = 0,
        KnobAlgorithm,
        KnobFeedback,
        KnobTimeRatio,
        KnobBrightness,
        KnobCoarse,
        KnobFine,
        KnobAttack,
        KnobDecay,
        KnobSustain,
        KnobRelease,
        KnobAmount,
        ButtonPreviousOperator,
        ButtonNextOperator,
        /*ButtonPreviousPreset,
        ButtonNextPreset*/
    };
public:
    PolyFMCore();
    
protected:
    void updateHIDValue(unsigned int index, float value) override;
    
private:
    void lockAllKnobs();
    void setCurrentOperator(unsigned int opIndex);
    
private:
    vector<int> parameterMap = {
        PolyFMDSP::Glide,
        PolyFMDSP::Algorithm,
        PolyFMDSP::Feedback,
        PolyFMDSP::TimeRatio,
        PolyFMDSP::Brightness,
    };
    
    vector<vector<int>> opParameterMap = {
        {DSP_PARAM_OP(A)},
        {DSP_PARAM_OP(B)},
        {DSP_PARAM_OP(C)},
        {DSP_PARAM_OP(D)},
    };
    
    int currentOpIndex = 0;

};
