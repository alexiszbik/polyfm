/*
  ==============================================================================

    FmAlgorithm.h
    Created: 17 Jan 2024 8:58:03am
    Author:  Alexis ZBIK

  ==============================================================================
*/

#pragma once

using namespace std;

struct FmAlgorithm {
    
    vector<vector<int>> modulators;
    int modulatorCount[4];
    bool isOutput[4] = {false, false, false, false};
    vector<int> outputOperators;
    int feedbackOp;

    FmAlgorithm(vector<int> outputOperators,
                vector<int> modulatorsForOp1,
                vector<int> modulatorsForOp2,
                vector<int> modulatorsForOp3,
                int feedbackOp) :
    outputOperators(outputOperators),
    feedbackOp(feedbackOp) {
        
        modulators.push_back(modulatorsForOp1);
        modulators.push_back(modulatorsForOp2);
        modulators.push_back(modulatorsForOp3);
        
        for (auto outputId : outputOperators) {
            isOutput[outputId] = true;
        }
        
        for (int i = 0; i < 3; ++i) {
            modulatorCount[i] = (int)modulators[i].size();
        }
        modulatorCount[3] = 0;
    }
    
    int getModulator(int opId, int index) {
        return modulators[opId][index];
    }
};
