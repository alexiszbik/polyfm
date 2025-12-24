/*
  ==============================================================================

    PolyFMDSP.cpp
    Created: 8 Nov 2023 4:51:13pm
    Author:  Alexis ZBIK

  ==============================================================================
*/

#include "PolyFMDSP.h"

#define DECLARE_OPERATOR(_name, _label) \
{Coarse##_name,  _label " Coarse",   false}, \
{Fine##_name,    _label " Fine",     false}, \
{Mode##_name,    _label " Mode",     false}, \
{Attack##_name,  _label " Attack",   false}, \
{Decay##_name,   _label " Decay",    false}, \
{Sustain##_name, _label " Sustain",  false}, \
{Release##_name, _label " Release",  false}, \
{Amount##_name,  _label " Amount",   false}

#define DECLARE_LFO(_name, _label) \
{LfoType##_name,        _label " Lfo Type",        false}, \
{LfoDestination##_name, _label " Lfo Destination", false}, \
{LfoRate##_name,        _label " Lfo Rate",        false}, \
{LfoAmount##_name,      _label " Lfo Amount",      false}

PolyFMDSP::PolyFMDSP()
: DSPKernel({
    {PlayMode,      "Play Mode",     false},
    {Glide,         "Glide",        false},
    {Algorithm,     "Algorithm",    false},
    {Feedback,      "Feedback",     false},
    {TimeRatio,     "Time Ratio",    false},
    {Brightness,    "Brightness",   false},
    {Volume,        "Volume",       false},
    
    DECLARE_OPERATOR(A,"A"),
    DECLARE_OPERATOR(B,"B"),
    DECLARE_OPERATOR(C,"C"),
    DECLARE_OPERATOR(D,"D"),
    
    DECLARE_LFO(A,"A"),
    DECLARE_LFO(B,"B"),
    
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
        case LfoDestinationA:
            lfo[0].setDestinationValue(value);
            break;
        case LfoDestinationB:
            lfo[1].setDestinationValue(value);
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

bool PolyFMDSP::isOpParameter(int index) {
    return index >= CoarseA && index <= AmountD;
}

int PolyFMDSP::getOpParameterForA(int index) {
    int op = getOperatorForIndex(index);
    static int opParamCount = (AmountA - CoarseA) + 1;
    if (op >= 0) {
        return index - opParamCount*op;
    }
    return -1;
}

int PolyFMDSP::getOperatorForIndex(int index) {
    if (index > AmountD) {
        return -1;
    }
    uint8_t k = 3;
    for (int opLast : {CoarseD, CoarseC, CoarseB, CoarseA}) {
        if (index >= opLast) {
            return k;
        }
        k--;
    }
    return -1;
}

float PolyFMDSP::opTimeValue(int operatorId, int aParam, bool applyTimeRatio, float min, float max) {
    float val = getValue(getOpParam(operatorId, aParam));
    val *= val;
    if (applyTimeRatio) {
        val += timeRatio * timeRatio;
    }
    return valueMap(val, min, max);
}

const char* PolyFMDSP::getLfoDestName(int lfoIdx) {
    auto dest = lfo[lfoIdx].getDestination();
    return lfo[lfoIdx].destinationNames[dest];
}

float PolyFMDSP::getLfoBuffer(Lfo::LfoDest target, uint8_t frame, float multiplier) {
    return lfo[0].getBuffer(target, frame, multiplier) + lfo[1].getBuffer(target, frame, multiplier);
}

void PolyFMDSP::process(float** buf, int frameCount) {
    DSPKernel::process(buf, frameCount);
    
    synth.setGlide(getValue(Glide));
    synth.setAlgorithm(valueMap(getValue(Algorithm), 0, SynthVoice::kAlgorithmCount - 1));
    
    uint8_t k = lfoCount;
    while (k--) {
        lfo[k].setRate(getValue(getLfoParam(k, LfoRateA)));
        lfo[k].setAmount(getValue(getLfoParam(k, LfoAmountA)));
        lfo[k].process(frameCount);
    }
    
    timeRatio = getValue(TimeRatio) + getLfoBuffer(Lfo::LfoDest_TimeRatio, 0);
    timeRatio = clamp(timeRatio, 0.0f, 1.0f);
    
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
        
        bool useFixedFreq = getValue(getOpParam(i, ModeA)) > 0.5f;
        synth.setOperatorMode(i, useFixedFreq);
        
        if (useFixedFreq) {
            float fixFreq = valueMap(getValue(getOpParam(i, CoarseA)), 10.f, 2000.f);

            uint8_t idx = valueMap(getValue(getOpParam(i, FineA)), 0, 4);
            float multiplier = multipliers[idx];
            synth.setOperatorFixFreq(i, fixFreq * multiplier);
        } else {
            float ratio = valueMap(getValue(getOpParam(i, CoarseA)), 0, 16);
            if (ratio == 0) {
                ratio = 0.5;
            }
            
            synth.setOperatorRatio(i, ratio * (getValue(getOpParam(i, FineA)) + 1));
        }
        synth.setOperatorAmount(i, getValue(getOpParam(i, AmountA)));
    }
    
    for (int i = 0; i < frameCount; i++) {
        updateParameters(); // useless only for smoothed parameters
        
        float volume = getValue(Volume); 
        synth.setFeedback(getValue(Feedback) + getLfoBuffer(Lfo::LfoDest_Feedback, i));
        synth.setBrightness(getValue(Brightness) + getLfoBuffer(Lfo::LfoDest_Brightness, i));
        
        //If we apply lfo to pitch
        synth.setTune(getLfoBuffer(Lfo::LfoDest_Pitch, i, 24));
        
        float out = synth.process() * volume;
       
        buf[0][i] = out * 0.15;
        for (int channel = 1; channel < channelCount; channel++) {
            buf[channel][i] = buf[0][i];
        }
        
    }
}
