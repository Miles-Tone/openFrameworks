#ifndef SensorData_h
#define SensorData_h

#include <fstream>
#include <sstream>
#include <vector>
#include <string>

struct SensorData {
    double sensor0;  // Using double for compatibility with dlib
    double depth;
    double note;
    double time;
    double time_delta; 
};
#endif /* SensorData_h */
