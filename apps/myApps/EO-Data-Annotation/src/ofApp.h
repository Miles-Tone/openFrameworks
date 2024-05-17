#pragma once

#include "ofMain.h"
#include "dataWriter.h"

class ofApp : public ofBaseApp{

	public:
		void setup() override;
		void update() override;
		void draw() override;
		void exit() override;

		void keyPressed(int key) override;
		void keyReleased(int key) override;
		void mouseMoved(int x, int y ) override;
		void mouseDragged(int x, int y, int button) override;
		void mousePressed(int x, int y, int button) override;
		void mouseReleased(int x, int y, int button) override;
		void mouseScrolled(int x, int y, float scrollX, float scrollY) override;
		void mouseEntered(int x, int y) override;
		void mouseExited(int x, int y) override;
		void windowResized(int w, int h) override;
		void dragEvent(ofDragInfo dragInfo) override;
		void gotMessage(ofMessage msg) override;
        
    ofSerial serial;
    ofBuffer serialReadBuffer;
    // when did we last read?
    float readTime;
    // if the device is not connected, when is the last time we tried to connect to it
    float timeLastTryConnect = 0.;
    
    vector <ofSerialDeviceInfo> deviceList;
    // set this to the device path that you would like to connect to
    // the default will be the first device in the list
    string serialDevicePathToConnectTo = "";
    // keep a history of the bytes read from the serial device so we can split it based on a character
    string serialReadString = "";
    
    bool stopped=false;
    
    string giantDataString="";
    ofBuffer finalBuff;
    
    std::unique_ptr<DataWriter> dataWriter;
    int T, S0, D, N;
    double setupTime;
    double prevTime;
    double curTime;
    string currentLine;
    void extractValues(const std::string& line);
    
    bool writingAtFullSpeed;
    
    void startNewCSV();
    string CSVDataString="";
    void writeLineToCSVString();
    bool isRecording=false;
    void writeCSVtoFile();
};
