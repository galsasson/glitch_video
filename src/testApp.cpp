#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofSetVerticalSync(true);
    
    ps.setup(0, 0, 0, 3840, 1080, "new_assets/00_3840_video_logo_final.mp4", "new_assets/alpha_3840_video_logo_final.svg", "new_assets/alpha_3840_video_logo_final.jpg");
//    flowInput.listen(10000);
//    ps.setFluidForces(flowInput.getForcesRef());
    
    bDrawFrameRate = true;
    
    kInput = new KinectInput(&ps, ps.getFluid());
}

//--------------------------------------------------------------
void testApp::update(){
//    flowInput.update(ps.getFluidRef());
    kInput->update();
    ps.update();
}

//--------------------------------------------------------------
void testApp::draw()
{
    ofPushMatrix();
    
    ofScale((float)(ofGetWindowWidth()+20)/3840, (float)(ofGetWindowHeight()+20)/1080);
//    ofScale((float)(ofGetWindowWidth())/3840, (float)(ofGetWindowHeight())/1080);
    ofTranslate(0, -10);
//    ofClear(0);

    ofPushStyle();
    ps.draw();
    ofPopStyle();
    
    ofPushStyle();
    kInput->draw();
    ofPopStyle();
//    flowInput.draw();
    
    if (bDrawFrameRate)
    {
        ofSetColor(255);
        char framerate[128];
        sprintf(framerate, "%f", ofGetFrameRate());
        ofDrawBitmapString(framerate, 20, ofGetHeight());
    }
    
    ofPopStyle();
    ofPopMatrix();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    switch (key)
    {
        case 'g':
            bDrawFrameRate = !bDrawFrameRate;
            ps.toggleSettings();
            kInput->toggleSettings();
//            flowInput.toggleSettings();
            break;
        case 'r':
            ps.loadSettings();
            kInput->loadSettings();
//            flowInput.loadSettings();
            break;
        case 's':
            ps.saveSettings();
            kInput->saveSettings();
//            flowInput.saveSettings();
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
        case 'b':
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
