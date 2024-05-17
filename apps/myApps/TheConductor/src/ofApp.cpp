#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    modelsLoaded=false;
    prevD=prevN=-1;
    depthMinMax[0]=10000;
    depthMinMax[1]=-10000;
    noteMinMax[0]=10000;
    noteMinMax[1]=-10000;
}

//--------------------------------------------------------------
void ofApp::update(){
    if(modelsLoaded)conductorAI.printOperations();
    if(modelsLoaded)predict(dummySensorVal);
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(21);
    if(!modelsLoaded){
        ofSetColor(255);
        ofDrawBitmapString("Press L to load the models", 10, ofGetHeight()/2);
        return;
    }
    ofSetColor(244);
    ofDrawBitmapString(ofToString(ofGetFrameRate()),10,10);
    int yPos = ofMap(dummySensorVal,1024,0,0,ofGetHeight());
    ofDrawRectangle(0,yPos-10,ofGetWidth(),20);
    ofSetColor(0);
    ofDrawBitmapString(ofToString(dummySensorVal), 20, yPos);
    if(predicted_note>0.008){
        ofSetColor(77,255,77);
        ofDrawCircle(ofGetWidth()-50, 50, 50);
    }
    if(predicted_depth>447.449 || predicted_depth<447.449){
        ofSetColor(255,255,77);
        ofDrawCircle(50, 50, 50);
    }
    ofSetColor(255,77,77);
    ofDrawBitmapString(ofToString(predicted_depth),ofGetWidth()-150,ofGetHeight()-20);
    string minMaxD = "Depth Min:"+ofToString(depthMinMax[0])+" Max:"+ofToString(depthMinMax[1]);
    string minMaxN = "Note Min:"+ofToString(noteMinMax[0])+" Max:"+ofToString(noteMinMax[1]);
    
    ofDrawBitmapString(minMaxD,10,ofGetHeight()-60);
    ofDrawBitmapString(minMaxN,10,ofGetHeight()-20);
}
void ofApp::trainModels(string path){
    
}
void ofApp::loadModels(){
    /*
    ofFileDialogResult result = ofSystemLoadDialog("Load Model File");
    if(result.bSuccess) {
      string path = result.getPath();
        conductorAI.loadModel(path, modelsLoaded);
    }
    */
    
    conductorAI.loadModels(ofToDataPath("saved_hybrid_model"), modelsLoaded);
}
void ofApp::predict(double sensorVal){
    double curTime =ofGetSystemTimeMillis();
    double timeDelta = curTime-prevtVal;
    // Predict using the trained models
    
    conductorAI.predict(sensorVal, timeDelta, predicted_depth, predicted_note);
    prevtVal=curTime;
    
    /*
    if(predicted_depth!=prevD || predicted_note!=prevN){
        prevN=predicted_note;
        prevD=predicted_depth;
        cout<<"D"<<predicted_depth<<" N"<<predicted_note<<endl;
    }
     */
    cout<<"D"<<predicted_depth<<" N"<<predicted_note<<endl;
    if(predicted_depth<depthMinMax[0])depthMinMax[0]=predicted_depth;
    if(predicted_depth>depthMinMax[1])depthMinMax[1]=predicted_depth;
    if(predicted_note<noteMinMax[0])noteMinMax[0]=predicted_note;
    if(predicted_note>noteMinMax[1])noteMinMax[1]=predicted_note;
}

//--------------------------------------------------------------
void ofApp::exit(){

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    /*
    if((key=='T'|| key == 't') && !modelsLoaded){
        ofFileDialogResult result = ofSystemLoadDialog("Load file");
        if(result.bSuccess) {
          string path = result.getPath();
            trainModels(path);
        }
    }
    */
    if((key=='L' || key =='l') && !modelsLoaded){
        loadModels();
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    if(modelsLoaded){
        dummySensorVal = ofMap(y,0,ofGetHeight(),1024,0);
        if(dummySensorVal<0)dummySensorVal=0;
        if(dummySensorVal>1023)dummySensorVal=1023;
    }
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
