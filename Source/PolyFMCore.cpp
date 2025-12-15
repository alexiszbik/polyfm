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
        {MuxKnob_1,             kKnob,      HIDPin(0,0),  "MuxKnob_1"},
        {MuxKnob_2,             kKnob,      HIDPin(0,1),  "MuxKnob_2"},
        {MuxKnob_3,             kKnob,      HIDPin(0,2),  "MuxKnob_3"},
        {MuxKnob_4,             kKnob,      HIDPin(0,3),  "MuxKnob_4"},
        {MuxKnob_5,             kKnob,      HIDPin(0,4),  "MuxKnob_5"},
        {MuxKnob_6,             kKnob,      HIDPin(0,5),  "MuxKnob_6"},
        {MuxKnob_7,             kKnob,      HIDPin(0,6),  "MuxKnob_7"},
        {MuxKnob_8,             kKnob,      HIDPin(0,7),  "MuxKnob_8"},
        {MuxKnob_9,             kKnob,      HIDPin(0,8),  "MuxKnob_9"},
        {MuxKnob_10,            kKnob,      HIDPin(0,9),  "MuxKnob_10"},
        {MuxKnob_11,            kKnob,      HIDPin(0,10), "MuxKnob_11"},
        {MuxKnob_12,            kKnob,      HIDPin(0,11), "MuxKnob_12"},
        {MuxKnob_13,            kKnob,      HIDPin(0,12), "MuxKnob_13"},
        {MuxKnob_14,            kKnob,      HIDPin(0,13), "MuxKnob_14"},
        {MuxKnob_15,            kKnob,      HIDPin(0,14), "MuxKnob_15"},
        {MuxKnob_16,            kKnob,      HIDPin(0,15), "MuxKnob_16"},
    
        {KnobFeedback,          kKnob,      16,           "Feedback"},
        {KnobTimeRatio,         kKnob,      17,           "TimeRatio"},
        {KnobBrightness,        kKnob,      18,           "Brightness"},
    
        {ButtonPreviousOperator,     kButton,    5, "Previous Map"},
        {ButtonNextOperator,         kButton,    6, "Next Map"},
    
        /*{ButtonPreviousPreset, kButton},
        {ButtonNextPreset, kButton}*/
     })
{
    lockAllKnobs();
}

void PolyFMCore::lockAllKnobs() {
    for (auto knob = (int)MuxKnob_1; knob <= (int)MuxKnob_6; knob++) {
        lockHID(knob);
    }
}

void PolyFMCore::setCurrentOperators(unsigned int opIndex) {
    currentOpsIndex = opIndex;
    
    const int opSize = SynthVoice::kOperatorCount / 2;
    if (currentOpsIndex < 0) {
        currentOpsIndex = (int)opSize - 1;
    } else {
        currentOpsIndex = currentOpsIndex % opSize;
    }
    
#if defined _SIMULATOR_
    std::cout << currentOpsIndex << std::endl;
#endif
    lockAllKnobs();
}

bool isBetweenParameterIndex(int x, int a, int b) {
    return x >= a && x <= b;
}

void PolyFMCore::updateHIDValue(unsigned int index, float value) {

    auto currentOpMap = &opParameterMap.at(currentOpsIndex);
    switch (index) {
        case ButtonPreviousOperator:
            setCurrentOperators(currentOpsIndex - 1);
            break;
            
        case ButtonNextOperator:
            setCurrentOperators(currentOpsIndex + 1);
            break;
            
        default:
            if (isBetweenParameterIndex(index, MuxKnob_1, MuxKnob_16)) {
                dspKernel->setParameterValue(currentOpMap->at(index - MuxKnob_1), value);
            } else {
                dspKernel->setParameterValue(parameterMap.at(index - MuxKnob_16 - 1), value);
            }
            break;
    }
}
