#pragma once

#include "ofMain.h"
#include "ofxAVFVideoPlayer.h"
#include "VideoParticle.h"
#include "ofxUI.h"
#include "ofxMPMFluid.h"

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    void exit();

    bool hasContent;

private:

    ofxUICanvas *gui;
    void setupGui();
    
    ofxAVFVideoPlayer videoPlayer;
    ofVec2f videoDim;
    bool setupDim;
    void newDimensions(ofVec2f newDim);
    
    int parChunk;
    float parPhysSize;
    float parAlpha;
    void initVideoParticles();
    vector<VideoParticle*> particles;
    ofVbo vbo;
    float noiseT = 0;
    float trailStrength;
    bool bUseAddMode;
    
    ofMutex drawMut;
    
    // blur
    float blurAmount, blurAlpha;
    ofShader shad_blurX, shad_blurY;
    ofFbo initialFbo, blurHor, blurVer;

    // fluid
    bool isReshaping;
    void updateFluid();
    ofxMPMFluid fluid;
    float density;
    float stiffness;
    float bulkViscosity;
    float elasticity;
    float viscosity;
    float gravity;
    float scaleFactor;

};
