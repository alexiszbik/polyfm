#include "daisy_seed.h"
#include "daisysp.h"

#include "DaisyYMNK/DaisyYMNK.h"
#include "DaisyYMNK/QSPI/PresetManager.h"
#include "Source/PolyFMCore.h"


using namespace daisy;
using namespace daisysp;
using namespace ydaisy;

DaisySeed hw;
PolyFMCore polyFM;
DaisyBase db = DaisyBase(&hw, &polyFM);

PresetManager pm;
DisplayManager *display = DisplayManager::GetInstance();

char floatChar[8];
ydaisy::Parameter* lastParam = nullptr;
const char* name = nullptr;
int pageIdx = -1;

EveryMs displayTimer(5, [](){

    auto lastChanged = polyFM.getLastChangedParameter();
    int newPageIdx = polyFM.getCurrentPage();
    
    if (pageIdx != newPageIdx) {
        pageIdx = newPageIdx;
        const char* pageName = "OP A / OP B";
        if (pageIdx == 1) {
            pageName = "OP C / OP D";
        } else if (pageIdx == 2) {
            pageName = "Globals";
        }
        
        display->WriteLine(0, pageName);
    }
    
    if (lastChanged && lastChanged != lastParam)
    {
        name = lastChanged->getName();
        lastParam = lastChanged;
        display->WriteLine(1, name);
    }

    if (lastChanged && name)
    {
        float value = lastChanged->getUIValue();
        floatToCString2(value, floatChar);
        display->WriteLine(2, floatChar);
    }
});

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
    db.process(out, size);
}

int currentPreset = 0;

void savePreset() {
    float pData[MAX_PRESET_SIZE];
    auto allParam = polyFM.getAllParameters();
    uint8_t k = 0;
    for (auto& param : allParam) {
        pData[k++] = param->getUIValue();
    }

    bool result = pm.Save(pData, k, currentPreset);
    if (result) {
        display->Write("Save Success!");
    } else {
        display->Write("Save Failed!");
    }
}
/*
void loadPreset(int index) {
    const float* dataToLoad = pm.Load(index);
    if (dataToLoad) {
        polyFM.loadPreset(dataToLoad);
    }
    intToCString2(index, floatChar);
    display->Write("Load Preset", floatChar);
}*/

void ValueChanged(uint8_t index, float v) { 
    if (v == 1.0) {
        if (index == PolyFMCore::ButtonSave) {
            display->Write("Save");
            savePreset();
           
        } /*else if (index == PolyFMCore::ButtonNextPreset) {
            currentPreset = (currentPreset + 1) % MAX_PRESETS;
            loadPreset(currentPreset);
        } else if (index == PolyFMCore::ButtonPreviousPreset) {
            currentPreset = currentPreset - 1;
            if (currentPreset < 0) {
                currentPreset = MAX_PRESETS - 1;
            }
            loadPreset(currentPreset);
        }*/
    }
}

void InitHID()
{
    db.listen();
}

int main(void)
{
    db.init(AudioCallback);

    display->Init(&hw);
    display->WriteNow("YMNK", "PolyFM Synth");

    pm.Init(&hw);

    db.setDisplayManager(display);
    db.setPresetManager(&pm);

    polyFM.setHIDValue(PolyFMCore::MidiLed, 1);

    BlockingAction blocker;
    blocker.Run(1000, InitHID); //Wait for HID to init
    //Is it the best solution ? Maybe ?

    polyFM.setValueChangedCallback(ValueChanged);

    for(;;)
    {
        db.listen();
        displayTimer.Update();
        display->Update();
    }
    
}
