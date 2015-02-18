#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetFrameRate(60);
    client.connect("192.168.0.240",9910,5320);
  
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){

    if (client.hasWaitingMessages()) {
        inComing ="";
        uint8_t incomingSize = client.parseMessage();
        cout<<"Message is " + ofToString(incomingSize) + " long"<<endl;
        
         uint8_t incomingMessage[96];
        ofBuffer tempBuffer;
        client.getNextMessage(incomingMessage);
        for (int i = 0; i < 96; i++) printf("%x", incomingMessage[i]);
        
        
        for (int i =0; i<incomingSize; i++) {
            inComing+=incomingMessage[i];
        }
        cout<<inComing<<endl;
            }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    if (key == ' ') {
        const char connectHello[] = {
            0x10, 0x14, 0x53, 0xAB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3A, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
        ofBuffer buffer;
        buffer.append(connectHello, 20);
       
        client.sendMessage(buffer);
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

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
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}