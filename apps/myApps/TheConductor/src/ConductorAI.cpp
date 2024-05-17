#include "ConductorAI.h"
#include <iostream>
#include <vector>
#include <deque>
#include <numeric>
#include <cmath>
#include <algorithm>

void ConductorAI::loadModels(std::string modelPath, bool& areModelsLoaded) {
    std::cout << "Loading Models" << std::endl;
    hasPrintedSummary = false;

    std::cout << "EXISTS?? " << ofFile::doesFileExist(modelPath) << std::endl;

    hybridModel.load(modelPath);

    // Use the actual output names from the SavedModel
    std::vector<std::string> inputNames = {"serving_default_sensor_input:0"};
    std::vector<std::string> outputNames = {"StatefulPartitionedCall:0", "StatefulPartitionedCall:1"};

    hybridModel.setup(inputNames, outputNames);

    areModelsLoaded = hybridModel.isLoaded();
    if (!areModelsLoaded) {
        ofLogError("ConductorAI") << "Failed to load the model!";
    }
}

void ConductorAI::printOperations() {
    if (hasPrintedSummary) return;
    hybridModel.printOperations();
    hasPrintedSummary = true;
}

void ConductorAI::predict(double sensor0, double time_delta, float& predicted_depth, float& predicted_note) {
    // Ensure there is valid data before prediction
    if (!isValidData(sensor0, time_delta)) {
        ofLogError("ConductorAI") << "Invalid input data for prediction!";
        predicted_depth = -1;
        predicted_note = -1;
        return;
    }

    std::vector<cppflow::tensor> inputs = prepareInput(sensor0, time_delta);
    std::vector<cppflow::tensor> outputs = hybridModel.runMultiModel(inputs);

    // Ensure that the outputs vector has the correct number of elements
    if (outputs.size() != 2) {
        ofLogError("ConductorAI") << "Unexpected number of outputs!";
        predicted_depth = -1;
        predicted_note = -1;
        return;
    }

    getPredictedValues(outputs, predicted_note, predicted_depth);
}

std::vector<cppflow::tensor> ConductorAI::prepareInput(double sensor0, double time_delta) {
    // Update sensor history for rolling computations
    sensor0_history.push_back(sensor0);
    if (sensor0_history.size() > 10) { // Update to match the new sequence length
        sensor0_history.pop_front();
    }

    // Compute rolling features
    float rolling_mean = computeRollingMean();
    float rolling_std = computeRollingStd();
    float diff = computeDiff(sensor0);
    float variance = computeVariance();

    // Prepare input tensor with all features
    std::vector<cppflow::tensor> inputs;
    std::vector<float> input_values = {
        static_cast<float>(time_delta), static_cast<float>(sensor0), rolling_mean, rolling_std, diff, variance
    };
    for (int i = 1; i < 10; ++i) { // Duplicate to match the sequence length of 10
        input_values.insert(input_values.end(), {
            static_cast<float>(time_delta), static_cast<float>(sensor0), rolling_mean, rolling_std, diff, variance
        });
    }
    cppflow::tensor input(input_values, {1, 10, 6}); // input shape is (1, 10, 6) to match the model input
    inputs.push_back(input);
    return inputs;
}

void ConductorAI::getPredictedValues(const std::vector<cppflow::tensor>& outputs, float& predicted_note, float& predicted_depth) {
    std::vector<float> noteOutput_values = outputs[1].get_data<float>();
    std::vector<float> depthOutput_values = outputs[0].get_data<float>();
    predicted_note = static_cast<float>(noteOutput_values[0]);
    predicted_depth = static_cast<float>(depthOutput_values[0]);
}

bool ConductorAI::isValidData(double sensor0, double time_delta) {
    // Add any additional checks as necessary
    return (sensor0 >= 0 && time_delta >= 0);
}

// Implementations to compute rolling features
float ConductorAI::computeRollingMean() {
    float sum = std::accumulate(sensor0_history.begin(), sensor0_history.end(), 0.0f);
    return sum / sensor0_history.size();
}

float ConductorAI::computeRollingStd() {
    float mean = computeRollingMean();
    float sq_sum = std::inner_product(sensor0_history.begin(), sensor0_history.end(), sensor0_history.begin(), 0.0f, std::plus<float>(), [mean](float a, float b) { return (a - mean) * (b - mean); });
    return std::sqrt(sq_sum / sensor0_history.size());
}

float ConductorAI::computeDiff(double sensor0) {
    if (sensor0_history.size() < 2) return 0.0f;
    return sensor0 - sensor0_history[sensor0_history.size() - 2];
}

float ConductorAI::computeVariance() {
    float mean = computeRollingMean();
    float variance = 0.0f;
    for (float val : sensor0_history) {
        variance += (val - mean) * (val - mean);
    }
    return variance / sensor0_history.size();
}
