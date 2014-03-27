#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofSetVerticalSync(true);
    ofSetFrameRate(60);
    
    setupGui();
    
    ps.setup(0, 0, 0, 1920, 540, "assets/00_1920_video_logo_h264.mp4", "assets/alpha_3840_video_logo_final.svg", "assets/alpha_3840_video_logo_final.jpg");
    
    kInput = new KinectInput(&ps, ps.getFluid());
    
    bDrawFrameRate = false;
    gui->toggleVisible();
    ps.toggleSettings();
    kInput->toggleSettings();
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
    
//    ofScale((float)(ofGetWindowWidth()+20)/3840, (float)(ofGetWindowHeight()+20)/1080);
//    ofTranslate(0, -10);
    ofScale(scale.x, scale.y);
    ofTranslate(pos.x, pos.y);

    ofPushStyle();
    ps.draw();
    ofPopStyle();
    
    ofPushStyle();
    kInput->draw();
    ofPopStyle();
    
    ofPopStyle();
    ofPopMatrix();
    
    if (bDrawFrameRate)
    {
        ofSetColor(255, 255, 255, 255);
        char framerate[128];
        sprintf(framerate, "%f", ofGetFrameRate());
        ofDrawBitmapString(framerate, 20, ofGetHeight());
    }
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    switch (key)
    {
        case 'g':
            bDrawFrameRate = !bDrawFrameRate;
            gui->toggleVisible();
            ps.toggleSettings();
            kInput->toggleSettings();
//            flowInput.toggleSettings();
            break;
        case 'r':
            loadSettings();
            ps.loadSettings();
            kInput->loadSettings();
//            flowInput.loadSettings();
            break;
        case 's':
            saveSettings();
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
        case OF_KEY_UP:
            pos.y -= 1;
            break;
        case OF_KEY_DOWN:
            pos.y += 1;
            break;
        case OF_KEY_LEFT:
            pos.x -= 1;
            break;
        case OF_KEY_RIGHT:
            pos.x += 1;
            break;
        case 'z':
            scale.x -= 0.005;
            break;
        case 'x':
            scale.x += 0.005;
            break;
        case 'c':
            scale.y -= 0.005;
            break;
        case 'v':
            scale.y += 0.005;
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

void testApp::setupGui()
{
    gui = new ofxUICanvas(400, 0, 200, ofGetHeight());
    
    gui->addWidgetDown(new ofxUILabel("Position", OFX_UI_FONT_MEDIUM));
    
    pos.x = -10;
    gui->addSlider("X", -100, 100, &pos.x);
    pos.y = 0;
    gui->addSlider("Y", -100, 100, &pos.y);
    scale.x = 2;
    gui->addSlider("Scale width", 1.8, 2.2, &scale.x);
    scale.y = 2;
    gui->addSlider("Scale height", 1.8, 2.2, &scale.y);
}

void testApp::loadSettings()
{
    gui->loadSettings("GUI/Position.xml");
}

void testApp::saveSettings()
{
    gui->saveSettings("GUI/Position.xml");
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
