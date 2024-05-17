#pragma once

#include "ofMain.h"
#include "ConductorAI.h"
#include "SensorData.h"
#include "DataParser.h"

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
    
        void trainModels(string path);
        void loadModels();
        void predict(double sensorVal);
    
        DataParser dataParser;
		
        ConductorAI conductorAI;
        double prevtVal;
        int dummySensorVal=0;
        
        bool modelsLoaded;
        float predicted_depth, predicted_note;
        float prevD, prevN;
        float depthMinMax[2];
        float noteMinMax[2];
};
