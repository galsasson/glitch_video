#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofSetVerticalSync(true);
    
    ps.setup(0, 200, 0, 640/*ofGetWidth()-200*/, 360/*ofGetHeight()*/, "videos/different_pulses.mp4");
    flowInput.listen(10000);
    ps.setFluidForces(flowInput.getForcesRef());
//    ps2.setup(1, 300, 300, 100, 100, "videos/xx.mp4");
    
}

//--------------------------------------------------------------
void testApp::update(){
    flowInput.update();
    ps.update();
//    ps2.update();
}

//--------------------------------------------------------------
void testApp::draw()
{
    ofPushStyle();
    ofClear(0);

    ps.draw();
//    ps2.draw();
    
    ofPopStyle();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    switch (key)
    {
        case 'g':
            ps.toggleSettings();
            flowInput.toggleSettings();
            break;
        case 'r':
            ps.loadSettings();
            flowInput.loadSettings();
            break;
        case 's':
            ps.saveSettings();
            flowInput.saveSettings();
            break;
        case 'p':
            ps.backToPlace(false);
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
            ps.backToPlace(true);
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
