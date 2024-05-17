//
//  dataParser.h
//  EO-Conductor
//
//  Created by Miles Warren on 09/05/2024.
//
// DataParser.h
#pragma once
#include "SensorData.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

class DataParser {
    u_int64_t prevTime=0;
public:
    std::vector<SensorData> loadData(const std::string& filepath, int dummyRestrict=0) {//dummyRestrict reduces ingest HACKKK
        std::ifstream file(filepath);
        std::vector<SensorData> data;
        std::string line;
        
        while (getline(file, line)) {
            std::stringstream ss(line);
            SensorData entry;
            std::string part;

            while (getline(ss, part, ',')) {
                std::string key = part.substr(0, part.find(':'));
                int value = std::stoi(part.substr(part.find(':') + 1));

                if (key == "S0") entry.sensor0 = value;
                else if (key == "D") entry.depth = value;
                else if (key == "N") entry.note = value;
                else if (key == "T") {
                    entry.time = value;
                    entry.time_delta=value-prevTime;
                    prevTime=value;
                }
            }
            if(dummyRestrict==0 ||data.size()<dummyRestrict){
                data.push_back(entry);
            }
        }
        cout<<"Training Data Ingested, # Lines: "<<data.size()<<endl;
        return data;
    }
};
