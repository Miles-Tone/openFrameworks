#include <string>
#include <fstream>
#include <mutex>
#include <atomic>
#include "ofFileUtils.h"
#include "ofUtils.h"

class DataWriter {
public:
    DataWriter() : threadReady_(true) {
        // Get the directory of the executable
        std::string exeDirectory = ofFilePath::getCurrentWorkingDirectory();
        std::string enclDirectory = ofFilePath::getEnclosingDirectory(exeDirectory);
        // Append "saves" to the executable directory to get the output directory
        outputDirectory_ = ofFilePath::join(enclDirectory, "saves");
    }

    void setData(std::string data) {
        std::lock_guard<std::mutex> lock(mutex_);
        data_ = std::move(data);
    }

    void writeFileAsync() {
        if (!threadReady_) return; // If the thread is not ready, exit

        std::lock_guard<std::mutex> lock(mutex_);
        if (!data_.empty()) {
            // Create the "saves" directory if it doesn't exist
            ofDirectory::createDirectory(outputDirectory_, true);

            // Create a new file with a timestamp
            std::string fileName = ofFilePath::join(outputDirectory_, ofGetTimestampString("%Y-%m-%d-%H-%M-%S-%i") + ".txt");
            std::ofstream file(fileName, std::ios::binary);
            if (file.is_open()) {
                file << data_;
                file.close();
                std::cout << "File written asynchronously: " << fileName << std::endl;
            } else {
                std::cerr << "Failed to write file: " << fileName << std::endl;
            }
            // Clear the data after writing
            data_.clear();
        }
        threadReady_ = true; // Set the thread ready flag to true
    }

    bool isThreadReady() const {
        return threadReady_;
    }

    void setThreadReady(bool ready) {
        threadReady_ = ready;
    }

private:
    std::string data_;
    std::string outputDirectory_;
    std::mutex mutex_;
    std::atomic<bool> threadReady_;
};
