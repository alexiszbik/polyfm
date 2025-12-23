/*
  ==============================================================================

    PolyFMCore.cpp
    Created: 19 Jan 2024 10:00:43am
    Author:  Alexis ZBIK

  ==============================================================================
*/

#include "PolyFMCore.h"
#include "DaisyYMNK/Helpers/StrConverters.h"

bool isBetweenParameterIndex(int x, int a, int b) {
    return x >= a && x <= b;
}

PolyFMCore::PolyFMCore()
: ModuleCore(&polyFm,
     {
        {MuxKnob_1,                 kKnob,      HIDPin(0,0),    "MuxKnob_1"},
        {MuxKnob_2,                 kKnob,      HIDPin(0,1),    "MuxKnob_2"},
        {MuxKnob_3,                 kKnob,      HIDPin(0,2),    "MuxKnob_3"},
        {MuxKnob_4,                 kKnob,      HIDPin(0,3),    "MuxKnob_4"},
        {MuxKnob_5,                 kKnob,      HIDPin(0,4),    "MuxKnob_5"},
        {MuxKnob_6,                 kKnob,      HIDPin(0,5),    "MuxKnob_6"},
        {MuxKnob_7,                 kKnob,      HIDPin(0,6),    "MuxKnob_7"},
        {MuxKnob_8,                 kKnob,      HIDPin(0,7),    "MuxKnob_8"},
        {MuxKnob_9,                 kKnob,      HIDPin(0,8),    "MuxKnob_9"},
        {MuxKnob_10,                kKnob,      HIDPin(0,9),    "MuxKnob_10"},
        {MuxKnob_11,                kKnob,      HIDPin(0,10),   "MuxKnob_11"},
        {MuxKnob_12,                kKnob,      HIDPin(0,11),   "MuxKnob_12"},
        {MuxKnob_13,                kKnob,      HIDPin(0,12),   "MuxKnob_13"},
        {MuxKnob_14,                kKnob,      HIDPin(0,13),   "MuxKnob_14"},
        {MuxKnob_15,                kKnob,      HIDPin(0,14),   "MuxKnob_15"},
        {MuxKnob_16,                kKnob,      HIDPin(0,15),   "MuxKnob_16"},
    
        {KnobVolume,                kKnob,      16,             "Volume"},
        {KnobTimeRatio,             kKnob,      17,             "TimeRatio"},
        {KnobBrightness,            kKnob,      18,             "Brightness"},
    
        {ButtonSave,                kButton,    5,              "Button Save"},
        {ButtonPreviousOperator,    kButton,    6,              "Previous Map"},
        {ButtonNextOperator,        kButton,    7,              "Next Map"},
        {ButtonPreviousPreset,      kButton,    8,              "Previous Preset"},
        {ButtonNextPreset,          kButton,    9,              "Next Preset"},

        {MidiLed,                   kLed,       10,             "Led"},
     })
{
    lockAllKnobs();
    
    needsResetDisplay = true;
}

void PolyFMCore::lockAllKnobs() {
    for (auto knob = (int)MuxKnob_1; knob <= (int)KnobBrightness; knob++) {
        lockHID(knob);
    }
}

void PolyFMCore::loadPreset(const float* values) {
    dspKernel->loadPreset(values);
    lockAllKnobs();
}

int PolyFMCore::getCurrentPage() {
    return currentPage.get();
}

void PolyFMCore::changeCurrentPage(bool increment) {
    if (increment) {
        currentPage.increment();
    } else {
        currentPage.decrement();
    }
    
#if defined _SIMULATOR_ //TODO : move this into a display simulator
    std::cout << currentPage.get() << std::endl;
#endif
    lockAllKnobs();
    displayPageOnScreen();
}

void PolyFMCore::changeCurrentPreset(bool increment) {
    if (increment) {
        currentPreset.increment();
    } else {
        currentPreset.decrement();
    }
    
    const float* dataToLoad = presetManager->Load(currentPreset.get());
    if (dataToLoad) {
        loadPreset(dataToLoad);
    }
    intToCString2(currentPreset.get(), numCharBuffer);
    displayManager->Write("Load Preset", numCharBuffer);
    needsResetDisplay = true;
}

void PolyFMCore::saveCurrentPreset() {
    float pData[MAX_PRESET_SIZE];
    auto allParam = getAllParameters();
    uint8_t k = 0;
    for (auto& param : allParam) {
        pData[k++] = param->getUIValue();
    }

    bool result = presetManager->Save(pData, k, currentPreset.get());
    if (result) {
        displayManager->Write("Save Success!");
    } else {
        displayManager->Write("Save Failed!");
    }
    needsResetDisplay = true;
}

void PolyFMCore::displayPageOnScreen() {
    int pageIdx = currentPage.get();
    const char* pageName = "OP A / OP B";
    if (pageIdx == 1) {
        pageName = "OP C / OP D";
    } else if (pageIdx == 2) {
        pageName = "Globals";
    }
    displayManager->WriteLine(0, pageName);
}

void PolyFMCore::displayValuesOnScreen() {
    
    if (!needsToUpdateValue) {
        return;
    }
    
    if (lastParam) {
        
        if (paramA >= 0) {
            int pos = 0;
            static constexpr uint8_t cSize = 3;
            
            float valueA = dspKernel->getParameter(paramA)->getUIValue();
            floatToCStringPct3(valueA, numCharBuffer);
            
            memcpy(fullNumCharBuffer + pos, numCharBuffer, cSize);
            pos += cSize;
        
            fullNumCharBuffer[pos++] = ' ';
            
            for (uint8_t o = 1; o < 4; o++) {
                int param = polyFm.getOpParam(o, paramA);
                float val = dspKernel->getParameter(param)->getUIValue();
                floatToCStringPct3(val, numCharBuffer);
                
                memcpy(fullNumCharBuffer + pos, numCharBuffer, cSize);
                pos += cSize;
            
                if (o < 3) {
                    fullNumCharBuffer[pos++] = ' ';
                }
            }
            
            fullNumCharBuffer[pos] = '\0';
            
            displayManager->WriteLine(2, fullNumCharBuffer);
         
        } else {
            float value = lastParam->getUIValue();
            floatToCString2(value, numCharBuffer);
            
            displayManager->WriteLine(2, numCharBuffer);
        }
    }
    
    needsToUpdateValue = false;
}

//Well we should make a loop again
void PolyFMCore::displayLastParameterOnScreen() {
    lastParamIndex = dspKernel->getLastChangedParameterIndex();
    Parameter* lastChanged = dspKernel->getParameter(lastParamIndex);
    
    if (needsResetDisplay) {
        needsResetDisplay = false;
        displayPageOnScreen();
    }
    
    if (lastChanged && lastChanged != lastParam) {
        const char* name = lastChanged->getName();
        lastParam = lastChanged;
        displayManager->WriteLine(1, name);
        paramA = polyFm.getOpParameterForA(lastParamIndex);
    }
    
    needsToUpdateValue = true;

}

void PolyFMCore::processMIDI(MIDIMessageType messageType, int channel, int dataA, int dataB) {
    ModuleCore::processMIDI(messageType, channel, dataA, dataB);
    if (messageType == kNoteOn) {
        setHIDValue(MidiLed, 1);
    } else if (messageType == kNoteOff) {
        setHIDValue(MidiLed, 0);
    }
}

void PolyFMCore::updateHIDValue(unsigned int index, float value) {

    auto currentOpMap = &pages.at(currentPage.get());
    switch (index) {
        case ButtonPreviousOperator:
            changeCurrentPage(false);
            break;
            
        case ButtonNextOperator:
            changeCurrentPage(true);
            break;
            
        case ButtonSave:
            saveCurrentPreset();
            break;
            
        case ButtonPreviousPreset:
            changeCurrentPreset(false);
            break;
            
        case ButtonNextPreset:
            changeCurrentPreset(true);
            break;
            
        case MidiLed:
            //Hmmm, this should never happen
            break;
            
        default:
            if (isBetweenParameterIndex(index, MuxKnob_1, MuxKnob_16)) {
                dspKernel->setParameterValue(currentOpMap->at(index - MuxKnob_1), value);
            } else {
                dspKernel->setParameterValue(parameterMap.at(index - MuxKnob_16 - 1), value);
            }
            displayLastParameterOnScreen();
            
            break;
    }
}
