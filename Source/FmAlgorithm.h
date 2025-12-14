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
    vector<int> outputOperators;
    int feedbackOp;
    
    vector<bool> isOutput = {false, false, false, false};

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
    }
    
    int getModulatorCount(int opId) {
        if (opId >= 3) {
            return 0;
        }
        return (int)modulators.at(opId).size();
    }
    
    int getModulator(int opId, int index) {
        return modulators.at(opId).at(index);
    }
};
