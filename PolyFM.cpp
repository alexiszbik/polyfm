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

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
    db.process(out, size);
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

    for(;;)
    {
        db.listen();
        display->Update();
    }
    
}
