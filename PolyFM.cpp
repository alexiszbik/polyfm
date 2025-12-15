#include "daisy_seed.h"
#include "daisysp.h"

#include "DaisyYMNK/DaisyYMNK.h"
#include "Source/PolyFMCore.h"

PolyFMCore polyFM;
DaisyBase db = DaisyBase(&polyFM);

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

int main(void)
{
    db.init(AudioCallback);

    display->Init(&hw);
    display->WriteNow("YMNK", "PolyFM Synth");

    polyFM.setHIDValue(PolyFMCore::MidiLed, 1);

    for(;;)
    {
        db.listen();
        displayTimer.Update(); 
        display->Update();
    }
}
