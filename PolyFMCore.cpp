/*
  ==============================================================================

    PolyFMCore.cpp
    Created: 19 Jan 2024 10:00:43am
    Author:  Alexis ZBIK

  ==============================================================================
*/

#include "PolyFMCore.h"

PolyFMCore::PolyFMCore()
: ModuleCore(new PolyFMDSP(),
     {
        //12 Knobs
        {KnobGlide,             kKnob,      15, "Glide"},
        {KnobAlgorithm,         kKnob,      16, "Algorithm"},
        {KnobFeedback,          kKnob,      17, "Feedback"},
        {KnobTimeRatio,         kKnob,      18, "TimeRatio"},
        {KnobBrightness,        kKnob,      19, "Brightness"},
        {KnobCoarse,            kKnob,      20, "Coarse"},
        {KnobFine,              kKnob,      21, "Fine"},
        {KnobAttack,            kKnob,      22, "Attack"},
        {KnobDecay,             kKnob,      23, "Decay"},
        {KnobSustain,           kKnob,      24, "Sustain"},
        {KnobRelease,           kKnob,      25, "Release"},
        {KnobAmount,            kKnob,      26, "Amount"},
    
        {ButtonPreviousOperator,     kButton,    27, "Previous Map"},
        {ButtonNextOperator,         kButton,    28, "Next Map"},
    
        /*{ButtonPreviousPreset, kButton},
        {ButtonNextPreset, kButton}*/
     })
{
    lockAllKnobs();
}

void PolyFMCore::lockAllKnobs() {
    for (auto knob = (int)KnobCoarse; knob <= (int)KnobAmount; knob++) {
        lockHID(knob);
    }
}

void PolyFMCore::setCurrentOperator(unsigned int opIndex) {
    currentOpIndex = opIndex;
    
    const int opSize = SynthVoice::kOperatorCount;
    if (currentOpIndex < 0) {
        currentOpIndex = (int)opSize - 1;
    } else {
        currentOpIndex = currentOpIndex % opSize;
    }
    
#if defined _SIMULATOR_
    std::cout << currentOpIndex << std::endl;
#endif
    lockAllKnobs();
}

bool isBetweenParameterIndex(int x, int a, int b) {
    return x >= a && x <= b;
}

void PolyFMCore::updateHIDValue(unsigned int index, float value) {

    auto currentOpMap = &opParameterMap.at(currentOpIndex);
    switch (index) {
        case ButtonPreviousOperator:
            setCurrentOperator(currentOpIndex - 1);
            break;
            
        case ButtonNextOperator:
            setCurrentOperator(currentOpIndex + 1);
            break;
            
        default:
            if (isBetweenParameterIndex(index, KnobGlide, KnobBrightness)) {
                dspKernel->setParameterValue(parameterMap.at(index), value);
            } else if (isBetweenParameterIndex(index, KnobCoarse, KnobAmount)) {
                dspKernel->setParameterValue(currentOpMap->at(index - KnobCoarse), value);
            }
            break;
    }
}
