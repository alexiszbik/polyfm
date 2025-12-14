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
{Attack##_name,  xstr(Attack##_name),   false}, \
{Decay##_name,   xstr(Decay##_name),    false}, \
{Sustain##_name, xstr(Sustain##_name),  false}, \
{Release##_name, xstr(Release##_name),  false}, \
{Amount##_name,  xstr(Amount##_name),   false}

PolyFMDSP::PolyFMDSP()
: DSPKernel({
    {PlayMode,      "PlayMode",     false},
    {Glide,         "Glide",        false},
    {Algorithm,     "Algorithm",    false},
    {Feedback,      "Feedback",     false},
    {TimeRatio,     "TimeRatio",    false},
    {Brightness,    "Brightness",   false},
    {ChorusState,   "ChorusState",  false},
    DECLARE_OPERATOR(A),
    DECLARE_OPERATOR(B),
    DECLARE_OPERATOR(C),
    DECLARE_OPERATOR(D),
    
}){
    loadPreset(&preset);
}

PolyFMDSP::~PolyFMDSP() {
}

void PolyFMDSP::init(int channelCount, double sampleRate) {
    DSPKernel::init(channelCount, sampleRate);

    synth.init(sampleRate);
    
    chorus.Init(sampleRate);
    
    chorus.SetPan(0.2, 0.8);
    chorus.SetLfoDepth(0.9, -0.9);
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

int PolyFMDSP::getOpParam(int operatorId, int aParam) {
    static int opParamCount = (AmountA - CoarseA) + 1;
    return (operatorId * opParamCount) + aParam;
}

float PolyFMDSP::opIimeValue(int operatorId, int aParam, float min, float max) {
    return valueMap(getValue(getOpParam(operatorId, aParam)), min, max);
}

void PolyFMDSP::process(float** buf, int frameCount) {
    DSPKernel::process(buf, frameCount);
    
    bool chorusState = getValue(ChorusState);
    
    synth.setGlide(getValue(Glide));
    synth.setAlgorithm(valueMap(getValue(Algorithm), 0, SynthVoice::kAlgorithmCount - 1));
    
    for (int i = 0; i < 4; i++) {
        synth.setOperatorADSR(i,
                              opIimeValue(i, AttackA),
                              opIimeValue(i, DecayA),
                              opIimeValue(i, SustainA, 0.001, 1),
                              opIimeValue(i, ReleaseA));
        
        synth.setOperatorRatio(i, valueMap(getValue(getOpParam(i, CoarseA)), 1, 16) * (getValue(getOpParam(i, FineA)) + 1));
        synth.setOperatorAmount(i, getValue(getOpParam(i, AmountA)));
    }
     
    for (int i = 0; i < frameCount; i++) {
        updateParameters(); // useless only for smoothed parameters
        
        synth.setFeedback(getValue(Feedback));
        synth.setBrightness(getValue(Brightness));
        
        float out = synth.process();
        
        if (chorusState) {
            chorus.Process(out * 0.707);
            buf[0][i] = chorus.GetLeft();
            for (int channel = 1; channel < channelCount; channel++) {
                buf[channel][i] = chorus.GetRight();
            }
        } else {
            buf[0][i] = out * 0.2;
            for (int channel = 1; channel < channelCount; channel++) {
                buf[channel][i] = buf[0][i];
            }
        }
    }
}
