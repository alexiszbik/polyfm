/*
  ==============================================================================

    SynthVoice.cpp
    Created: 10 Jan 2024 2:48:00pm
    Author:  Alexis ZBIK

  ==============================================================================
*/

#include "SynthVoice.h"

void SynthVoice::init(double sampleRate) {
    phaseInc = (1/sampleRate);
    for (int i = 0; i < kOperatorCount; i++) {
        op[i].adsr.Init(sampleRate);
    }
    pitch.setImmediate(60);
    env.Init(sampleRate);
    this->sampleRate = sampleRate;
}

void SynthVoice::setPitch(int pitch) {
    this->pitch.setValue((float)pitch);
}

void SynthVoice::setGate(bool gate) {
    this->gate = gate;
    //env.SetGate(gate);
}

void SynthVoice::setNoteOn(Note note) {
    if (op[0].adsr.IsRunning() == false) { //In order to avoid clicks we should verify any output operators
        for (int i = kOperatorCount - 1; i >= 0; i--) {
            op[i].phase = 0;
        }
    }
    
    setPitch(note.pitch);
    for (int i = 0; i < kOperatorCount; i++) {
        op[i].adsr.Retrigger(false);
    }
    setGate(true);
    env.Retrig();
    noteTimeStamp = note.timeStamp;
}

void SynthVoice::setNoteOff() {
    setGate(false);
}

void SynthVoice::processPhase(Operator* op) {
    float f = op->useFixedFreq ? op->fixFreq : freq;
    op->phase += phaseInc * f * op->ratio;
    op->phase = fmod(op->phase, 1.0);
}

void SynthVoice::setGlide(float glide) {
    this->glide = glide;
}

void SynthVoice::setOperatorRatio(int operatorId, float ratio) {
    op[operatorId].ratio = ratio;
}

void SynthVoice::setOperatorAmount(int operatorId, float amount) {
    op[operatorId].amount = amount;
}

void SynthVoice::setOperatorMode(int operatorId, bool mode) {
    op[operatorId].useFixedFreq = mode;
}

void SynthVoice::setOperatorFixFrequency(int operatorId, float fixFreq) {
    op[operatorId].fixFreq = fixFreq;
}

void SynthVoice::setOperatorADSR(int operatorId, float attack, float decay, float sustain, float release) {
    op[operatorId].adsr.SetAttackTime(attack);
    op[operatorId].adsr.SetDecayTime(decay);
    op[operatorId].adsr.SetSustainLevel(sustain);
    op[operatorId].adsr.SetReleaseTime(release);
}

void SynthVoice::setEnvParameters(float attack, float decay, float amount) {
    env.SetAttack(attack);
    env.SetDecay(decay);
    envAmount = amount;
}

void SynthVoice::setFeedback(float feedbackAmount) {
    this->feedbackAmount = feedbackAmount;
}

void SynthVoice::setAlgorithm(int index) {
    this->selectedAlgorithm = index;
}

void SynthVoice::setBrightness(float brightness) {
    this->brigthness = brightness;
}

float SynthVoice::process() {
    
    float envPitch = env.Process() * (48 * envAmount - 24);
    
    pitch.dezipperCheck(sampleRate * glide);
    
    freq = mtof(pitch.getAndStep() + pitchMod + envPitch);
    
    FmAlgorithm* alg = &algorithms[selectedAlgorithm];
    
    for (int i = kOperatorCount - 1; i >= 0; i--) {
        Operator* o = &op[i];
        processPhase(o);
        
        float modulator = 0;
        
        if (i == alg->feedbackOp) {
            modulator = feedback * feedbackAmount;
        } else {
            if (alg->getModulatorCount(i)) {
                for (int index = 0; index < alg->getModulatorCount(i); index++) {
                    modulator += opOut[alg->getModulator(i, index)];
                }
            }
        }
        
        opOut[i] = cos((o->phase + modulator * 0.3333f) * PI_F * 2);
        
        float envOut = o->adsr.Process(gate);
        
        opOut[i] *= envOut*envOut;
        
        if (i == alg->feedbackOp) {
            feedback = opOut[i];
        }
        
        opOut[i] *= o->amount * (alg->isOutput[i] ? 1 : brigthness);
    }
    
    float out = 0;
    for (auto outputOpId : alg->outputOperators) {
        out += opOut[outputOpId];
    }
    
    return out;
}
