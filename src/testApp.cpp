#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofSetVerticalSync(true);
    
    ps.setup(0, 200, 0, ofGetWidth()-200, ofGetHeight(), "videos/different_pulses.mp4");
    
}

//--------------------------------------------------------------
void testApp::update(){
    ps.update();
}

//--------------------------------------------------------------
void testApp::draw()
{
    ofPushStyle();
    ofClear(0);

    ps.draw();
    
    ofPopStyle();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    switch (key)
    {
        case 'g':
            ps.toggleSettings();
            break;
        case 'r':
            ps.loadSettings();
            break;
        case 's':
            ps.saveSettings();
            break;
        case 'p':
            ps.backToPlace(true);
            break;
        default:
            break;
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
    switch(key)
    {
        case 'p':
            ps.backToPlace(false);
            break;
        default:
            break;
    }
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
//    for (int i=0; i<particles.size(); i++)
//    {
//        particles[i]->applyForce(ofVec3f(ofRandom(-15, 15), ofRandom(-15, 15), ofRandom(-1, 1)));
//    }
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}

void testApp::exit()
{
    
}
