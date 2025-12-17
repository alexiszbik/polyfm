#include "daisy_seed.h"
#include "daisysp.h"

#include "DaisyYMNK/DaisyYMNK.h"
#include "DaisyYMNK/QSPI/PresetManager.h"
#include "Source/PolyFMCore.h"


DaisySeed hw;
PolyFMCore polyFM;
DaisyBase db = DaisyBase(&hw, &polyFM);
PresetManager pm;

DisplayManager *display = DisplayManager::GetInstance();

using namespace daisy;
using namespace daisysp;
using namespace ydaisy;

// buffer MINIMUM: 8 chars -> "-123.45\0"
inline void floatToCString2(float v, char* buf)
{
    // gestion du signe
    if (v < 0.0f)
    {
        *buf++ = '-';
        v = -v;
    }

    // arrondi à 2 décimales
    int value = (int)(v * 100.0f + 0.5f);

    int intPart  = value / 100;
    int fracPart = value % 100;

    // conversion partie entière (max 3–4 digits typiquement)
    char tmp[6];
    int i = 0;
    do {
        tmp[i++] = '0' + (intPart % 10);
        intPart /= 10;
    } while (intPart);

    // reverse
    while (i--)
        *buf++ = tmp[i];

    *buf++ = '.';
    *buf++ = '0' + (fracPart / 10);
    *buf++ = '0' + (fracPart % 10);
    *buf = '\0';
}



char floatChar[8];
ydaisy::Parameter* lastParam = nullptr;
const char* name = nullptr;
int opIdx = -1;

EveryMs displayTimer(5, [](){

    auto lastChanged = polyFM.getLastChangedParameter();
    int newOpIdx = polyFM.getCurrentOpIdx();
    if (opIdx != newOpIdx) {
        opIdx = newOpIdx;
        display->WriteLine(0, opIdx == 0 ? "OP A / OP B" : "OP C / OP D");
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

bool isInit = false;

/*
#define WAVE_LENGTH 64
float DSY_QSPI_BSS qspi_buffer[WAVE_LENGTH];
float wavform_ram[WAVE_LENGTH];*/

void savePresetTest() {
    float pData[64];
    auto allParam = polyFM.getAllParameters();
    uint8_t k = 0;
    for (auto& param : allParam) {
        pData[k++] = param->getUIValue();
    }

    bool result = pm.Save(pData, k);
    if (result) {
        display->Write("Save Success!");
    } else {
        display->Write("Save Failed!");
    }
}

void loadPresetTest() {
    const float* dataToLoad = pm.Load();
    uint8_t k = 16;
    auto allParam = polyFM.getAllParameters();
    while (k--) {
        float v = dataToLoad[k];
        floatToCString2(v, floatChar);
        display->WriteNow("Load",allParam.at(k)->getName(), floatChar);
        System::Delay(200);
        
    }
    polyFM.loadPreset(dataToLoad);
}

void ValueChanged(uint8_t index, float v) { 
    if (v == 1.0) {
        if (index == PolyFMCore::ButtonSave) {
            if (isInit) {
                display->Write("Save");
                savePresetTest();
            }
           
        } else if (index == PolyFMCore::ButtonLoad) {
            display->Write("Load");
            loadPresetTest();
            isInit = true;
        }
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
