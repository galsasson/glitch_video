#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofSetVerticalSync(true);
    
//    ps.setup(0, 200, 0, 640/*ofGetWidth()-200*/, 320/*ofGetHeight()*/, "videos/different_pulses.mp4");
    ps.setup(0, 0, 0, 1920, 1080, "videos/innovid3.mp4");
    flowInput.listen(10000);
    ps.applyBorders("svg/test.svg");
    ps.setFluidForces(flowInput.getForcesRef());
    
//    fluidMask.setup(ps.getFluidRef());
}

//--------------------------------------------------------------
void testApp::update(){
    flowInput.update();
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
        case 'b':
            ps.breakFluid();
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
    ps.mouseDragged(x, y, button);
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    ps.mousePressed(x, y, button);
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    ps.mouseReleased(x, y, button);
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
