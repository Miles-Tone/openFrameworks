#ifndef ConductorAI_h
#define ConductorAI_h

#include "ofMain.h"
#include "SensorData.h"
#include "ofxTensorFlow2.h"
#include "cppflow.h"

class ConductorAI {
public:
    void loadModels(std::string modelPath, bool& areModelsLoaded);
    void printOperations();
    void predict(double sensor0, double time_delta, float& predicted_depth, float& predicted_note);

private:
    ofxTF2::Model hybridModel;
    bool hasPrintedSummary = false;

    std::vector<cppflow::tensor> prepareInput(double sensor0, double time_delta);
    void getPredictedValues(const std::vector<cppflow::tensor>& outputs, float& predicted_note, float& predicted_depth);
    bool isValidData(double sensor0, double time_delta);

    // Implementations to compute rolling features
    std::deque<float> sensor0_history;
    float computeRollingMean();
    float computeRollingStd();
    float computeDiff(double sensor0);
    float computeVariance();
};
#endif /* ConductorAI_h */
