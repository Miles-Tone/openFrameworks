#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(255);
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    serial.listDevices();
    deviceList = serial.getDeviceList();
    
    dataWriter = std::make_unique<DataWriter>();
    
    int baud = 9600;
    // if the serialDevicePathToConnectTo variable is set, try connecting there first
    // the serial device will try to reconnect in the update app if no device is connected
    if( serialDevicePathToConnectTo != "" ) {
        serial.setup(serialDevicePathToConnectTo, baud);
    } else {
        serial.setup(0, baud); //open the first device
    }
    // we just tried to connect, so store the time
    timeLastTryConnect = ofGetElapsedTimef();
    readTime = 0;
    
    T=S0=D=N=0;
    writingAtFullSpeed=false;
    if(writingAtFullSpeed)setupTime=ofGetSystemTimeMicros();
    else setupTime=ofGetSystemTimeMillis();
    prevTime=setupTime;
    //cout<<setupTime<<endl;
}

//--------------------------------------------------------------
void ofApp::update(){
    
    if( serial.isInitialized()) {
        
        // the number of bytes that the serial has available for reading
        int numBytesToRead = serial.available();
        
        // cap the amount of bytes to read at 512
        //if( numBytesToRead > 512 ) {
        //    numBytesToRead = 512;
        //}
        if( numBytesToRead > 0 ) {
            serialReadBuffer.clear();
            // if there are bytes to read, then read it into the serialReadBuffer
            serial.readBytes(serialReadBuffer, numBytesToRead );
            // append the string with the new buffer of data that has arrived
            serialReadString += serialReadBuffer.getText();
            
            // create a temporary string that will populate from the read string
            string fullMessage = "";
            int eraseIndex = -1;
            // go through each character in the string to search for a return character
            bool prev=false;
            for (int i = 0; i < serialReadString.length(); i++) {
                unsigned char character = serialReadString[i];

                // If the character is a newline or carriage return, process the line
                if (!prev && (character == '\n' || character == '\r')) {
                    // Add T component to giantDataString
                    
                    if(writingAtFullSpeed)fullMessage += ",T:" + ofToString(ofGetSystemTimeMicros()-setupTime);
                    else fullMessage += ",T:" + ofToString(ofGetSystemTimeMillis()-setupTime);
//                    // Display values if the line is not empty
//                    if (!fullMessage.empty()) {
//                        extractValues(fullMessage); // Extract and display values
//                    }
                    // Reset giantDataString for the next line
                    if(writingAtFullSpeed)giantDataString+=(fullMessage+"\n");//hack to free up memory
                    currentLine=fullMessage;
                    fullMessage.clear();
                } else {
                    fullMessage += character;
                    prev = (character == '\n' || character == '\r'); // Update prev flag
                }
            }
            if( eraseIndex > -1 ) {
                // a new message has been received and must be erased
                serialReadString = serialReadString.substr(eraseIndex);
            }
            readTime = ofGetElapsedTimef();
        }
    } else {
        // serial device is not initialized
        // lets try to connect again
        float etimef = ofGetElapsedTimef();
        if( etimef - timeLastTryConnect > 10.0 ) {
            // refresh device list
            deviceList = serial.getDeviceList();
            timeLastTryConnect = etimef;
            int baud = 9600;
            if( serialDevicePathToConnectTo != "" ) {
                ofLogNotice("Attempting to connect to serial device: ") << serialDevicePathToConnectTo;
                serial.setup(serialDevicePathToConnectTo, baud);
            } else {
                ofLogNotice("Attempting to connect to serial device: ") << 0;
                serial.setup(0, baud); //open the first device
            }
        }
    }
    
    extractValues(currentLine);
    
    if(writingAtFullSpeed){
        //File Saving
        if(ofGetElapsedTimef()>30 && dataWriter->isThreadReady() && giantDataString.size()>0){
            // Create a new copy of the data
            std::unique_ptr<std::string> dataCopy = std::make_unique<std::string>(giantDataString);
            // Set the data and trigger the file writing thread
            dataWriter->setData(std::move(*dataCopy));
            std::thread t(&DataWriter::writeFileAsync, dataWriter.get());
            t.detach(); // Detach the thread to allow it to run independently
            giantDataString.clear(); // Reset giantDataString after detaching the thread
            ofResetElapsedTimeCounter();
            cout<<"New File Thread"<<endl;
        }
    }
    
    if(currentLine.size()>0 && isRecording)writeLineToCSVString();
}
// Function to extract and display values
void ofApp::extractValues(const std::string& line) {
    std::istringstream iss(line);
    //int T = -1, S0 = -1, D = -1, N = -1; // Initialize values to -1 to detect if any component is missing
    std::string component;
    
    while (std::getline(iss, component, ',')) {
        // Check if the component contains T, S0, D, or N
        if (component.find("T:") != std::string::npos) {
            std::istringstream(component.substr(2)) >> curTime;
        } else if (component.find("S0:") != std::string::npos) {
            std::istringstream(component.substr(3)) >> S0;
        } else if (component.find("D:") != std::string::npos) {
            std::istringstream(component.substr(2)) >> D;
        } else if (component.find("N:") != std::string::npos) {
            std::istringstream(component.substr(2)) >> N;
        }
    }
    /*
    // Display values
    std::cout << "T: " << T << std::endl;
    std::cout << "S0: " << S0 << std::endl;
    std::cout << "D: " << D << std::endl;
    std::cout << "N: " << N << std::endl;
     */
    
}
//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor( 0 );
    
    ofDrawBitmapString("Serial connected: "+ofToString(serial.isInitialized()), 50, 10);
    /*
    string deviceString = "Devices:\n";
    for( int i = 0; i < deviceList.size(); i++ ) {
        deviceString += ofToString(deviceList[i].getDeviceID())+": " +deviceList[i].getDevicePath() + " \n";
    }
    ofDrawBitmapString(deviceString, 50, 20);
    */
    ofDrawBitmapString("Time Delta:"+ofToString(T), 50, 30);
    ofDrawBitmapString("Sensor:"+ofToString(S0), 50, 50);
    ofDrawBitmapString("Depth:"+ofToString(D), 50, 70);
    ofDrawBitmapString("Note:"+ofToString(N), 50, 90);
    
    if(isRecording){
        ofSetColor(255,77,77);
        ofFill();
        ofDrawCircle(200, 100, 30);
        ofSetColor(0);
        ofDrawBitmapString("RECORDING", 200, 100);
    }
    else{
        ofSetColor(255,77,77);
        ofNoFill();
        ofDrawCircle(200, 100, 30);
        ofSetColor(0);
        ofDrawBitmapString("hit R to record", 200, 100);
    }
    
}
void ofApp::startNewCSV(){
    CSVDataString="Time_Delta, Depth, Note, Sensor0\n";
    prevTime=curTime;
}
void ofApp::writeLineToCSVString(){
    T=curTime-prevTime;
    prevTime=curTime;
    CSVDataString+=ofToString(T)+","+ofToString(D)+","+ofToString(N)+","+ofToString(S0);
    CSVDataString+="\n";
}
void ofApp::writeCSVtoFile(){
    if(dataWriter->isThreadReady() && CSVDataString.size()>0){
        // Create a new copy of the data
        std::unique_ptr<std::string> dataCopy = std::make_unique<std::string>(CSVDataString);
        // Set the data and trigger the file writing thread
        dataWriter->setData(std::move(*dataCopy));
        std::thread t(&DataWriter::writeFileAsync, dataWriter.get());
        t.detach(); // Detach the thread to allow it to run independently
        CSVDataString.clear(); // Reset giantDataString after detaching the thread
        //ofResetElapsedTimeCounter();
        cout<<"New File Created"<<endl;
    }
}

//--------------------------------------------------------------
void ofApp::exit(){
   
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key==' '){
        if(!stopped){
            cout<<"STOPPED"<<endl;
            stopped=true;
            
            ofBuffer buff(giantDataString.c_str(), giantDataString.size());
            string fileName = ofGetTimestampString("%Y-%m-%d-%H-%M-%S-%i")+".txt";
            bool fileWritten = ofBufferToFile(fileName, buff,false);
            if(fileWritten)cout<<"success"<<endl;
            else cout<<"fail"<<endl;
        }
    }
    if(key=='r' || key=='R'){
        if(isRecording){
            writeCSVtoFile();
            isRecording=false;
        }
        else{
            isRecording=true;
            startNewCSV();
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

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
