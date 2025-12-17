/*
  ==============================================================================

    PolyFMDSP.cpp
    Created: 8 Nov 2023 4:51:13pm
    Author:  Alexis ZBIK

  ==============================================================================
*/

#include "PolyFMDSP.h"

#define DECLARE_OPERATOR(_name) \
{Coarse##_name,  xstr(Coarse##_name),   false}, \
{Fine##_name,    xstr(Fine##_name),     false}, \
{Mode##_name,    xstr(Mode##_name),     false}, \
{Attack##_name,  xstr(Attack##_name),   false}, \
{Decay##_name,   xstr(Decay##_name),    false}, \
{Sustain##_name, xstr(Sustain##_name),  false}, \
{Release##_name, xstr(Release##_name),  false}, \
{Amount##_name,  xstr(Amount##_name),   false}

#define DECLARE_LFO(_name) \
{LfoType##_name,            xstr(LfoType##_name),           false}, \
{LfoDestination##_name,     xstr(LfoDestination##_name),    false}, \
{LfoRate##_name,            xstr(LfoRate##_name),           false}, \
{LfoAmount##_name,          xstr(LfoAmount##_name),         false}

PolyFMDSP::PolyFMDSP()
: DSPKernel({
    {PlayMode,      "PlayMode",     false},
    {Glide,         "Glide",        false},
    {Algorithm,     "Algorithm",    false},
    {Feedback,      "Feedback",     false},
    {TimeRatio,     "TimeRatio",    false},
    {Brightness,    "Brightness",   false},
    {Volume,        "Volume",       false},
    
    DECLARE_OPERATOR(A),
    DECLARE_OPERATOR(B),
    DECLARE_OPERATOR(C),
    DECLARE_OPERATOR(D),
    
    DECLARE_LFO(A),
    DECLARE_LFO(B),
    
    {EnvDestination,    "EnvDestination",   false},
    {EnvAttack,         "EnvAttack",        false},
    {EnvDecay,          "EnvDecay",         false},
    {EnvAmount,         "EnvAmount",        false},
    
}){
#if defined _SIMULATOR_
    std::cout << getParameterCount() << " parameters" << std::endl;
    
#endif
}

PolyFMDSP::~PolyFMDSP() {
}

void PolyFMDSP::init(int channelCount, double sampleRate) {
    DSPKernel::init(channelCount, sampleRate);

    synth.init(sampleRate);
    
    uint8_t k = lfoCount;
    while (k--) {
        lfo[k].init(sampleRate);
    }
}

void PolyFMDSP::processMIDI(MIDIMessageType messageType, int channel, int dataA, int dataB) {
    DSPKernel::processMIDI(messageType, channel, dataA, dataB);
    
    switch (messageType) {
        case MIDIMessageType::kNoteOn : {
            synth.setNote(true, Note(dataA, dataB, timeStamp++));
        }
            break;
        case MIDIMessageType::kNoteOff : {
            synth.setNote(false, Note(dataA, 0, 0));
        }
            break;
        case MIDIMessageType::kControlChange : {
            if (dataA == 1 /* mod wheel */) {
                synth.setModWheel(dataB/127.f);
            } else {
                int parameterIndex = dataA - MIDI_CC_START;
                if (parameterIndex >= 0 && parameterIndex < getParameterCount()) {
                    setParameterValue(parameterIndex,dataB);
                }
            }
        }
            break;
        case MIDIMessageType::kPitchBend : {
            static const int midPB = 8192;
            float pitchBend;
            if (dataA < midPB) {
                pitchBend = -(1.f-(dataA/(float)midPB))*2.f;
            } else if (dataA > midPB) {
                pitchBend = ((dataA - midPB)/(16383.f - midPB))*2.f;
            } else {
                pitchBend = 0;
            }
            synth.setPitchBend(pitchBend);
        }
            break;
        default:
            break;
    }
}

void PolyFMDSP::updateParameter(int index, float value) {
    auto param = static_cast<Parameters>(index);
    switch (param) {
        case PlayMode :
            synth.setPolyMode(static_cast<PolySynth::EPolyMode>(valueMap(value, 0, 2)));
            break;
        default:
            break;
    }
}

int PolyFMDSP::getLfoParam(int lfoId, int aParam) {
    static int lfoParamCount = (LfoAmountA - LfoTypeA) + 1;
    return (lfoId * lfoParamCount) + aParam;
}

int PolyFMDSP::getOpParam(int operatorId, int aParam) {
    static int opParamCount = (AmountA - CoarseA) + 1;
    return (operatorId * opParamCount) + aParam;
}
/*
float PolyFMDSP::opIimeValue(int operatorId, int aParam, float min, float max) {
    return valueMap(getValue(getOpParam(operatorId, aParam)), min, max);
}*/

float PolyFMDSP::opTimeValue(int operatorId, int aParam, bool applyTimeRatio, float min, float max) {
    float val = getValue(getOpParam(operatorId, aParam));
    val *= val;
    if (applyTimeRatio) {
        val += timeRatio * timeRatio;
    }
    return valueMap(val, min, max);
}

void PolyFMDSP::process(float** buf, int frameCount) {
    DSPKernel::process(buf, frameCount);
    
    timeRatio = getValue(TimeRatio);
    
    synth.setGlide(getValue(Glide));
    synth.setAlgorithm(valueMap(getValue(Algorithm), 0, SynthVoice::kAlgorithmCount - 1));
    
    uint8_t k = lfoCount;
    while (k--) {
        lfo[k].setRate(getValue(getOpParam(k, LfoRateA)));
        lfo[k].setAmount(getValue(getOpParam(k, LfoAmountA)));
        lfo[k].process(frameCount);
    }
    
    float envAttack = getValue(EnvAttack);
    float envDecay = getValue(EnvDecay);
    
    float attack = valueMap(envAttack*envAttack, 0.0001f, 2.f);
    float decay = valueMap(envDecay*envDecay, 0.0001f, 2.f);
    
    synth.setEnvParameters(attack, decay, getValue(EnvAmount));
    
    for (int i = 0; i < 4; i++) {
        synth.setOperatorADSR(i,
                              opTimeValue(i, AttackA, false),
                              opTimeValue(i, DecayA, true),
                              opTimeValue(i, SustainA, false, 0.001, 1),
                              opTimeValue(i, ReleaseA, true));
        
        float ratio = valueMap(getValue(getOpParam(i, CoarseA)), 0, 16);
        if (ratio == 0) {
            ratio = 0.5;
        }
        
        synth.setOperatorRatio(i, ratio * (getValue(getOpParam(i, FineA)) + 1));
        synth.setOperatorAmount(i, getValue(getOpParam(i, AmountA)));
    }
    
    float volume = getValue(Volume); // should be smoothed ?
     
    for (int i = 0; i < frameCount; i++) {
        updateParameters(); // useless only for smoothed parameters
        
        synth.setFeedback(getValue(Feedback));
        synth.setBrightness(getValue(Brightness));
        
        //If we apply lfo to pitch
        //synth.setTune(lfo[0].getBuffer(i) * 24);
        
        float out = synth.process() * volume;
       
        buf[0][i] = out * 0.2;
        for (int channel = 1; channel < channelCount; channel++) {
            buf[channel][i] = buf[0][i];
        }
        
    }
}
