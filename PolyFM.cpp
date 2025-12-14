#include "daisy_seed.h"
#include "daisysp.h"

#include "DaisyYMNK/DaisyYMNK.h"
#include "Source/PolyFMCore.h"

PolyFMCore polyFM;
DaisyBase db = DaisyBase(&polyFM);

using namespace daisy;
using namespace daisysp;
using namespace ydaisy;

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
    db.process(out, size);
}

int main(void)
{
    db.init(AudioCallback);

    for(;;)
    {
        db.listen();
    }
}
