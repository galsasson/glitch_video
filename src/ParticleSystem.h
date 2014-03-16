//
//  ParticleSystem.h
//  glitch_video
//
//  Created by Gal Sasson on 1/23/14.
//
//

#ifndef __glitch_video__ParticleSystem__
#define __glitch_video__ParticleSystem__

#include <iostream>
#include "ofMain.h"
#include "ofxMPMFluid.h"
#include "ofxUI.h"
#include "ofxAVFVideoPlayer.h"
#include "ofxTween.h"

#include "VideoParticle.h"
#include "FluidMask.h"

class ParticleSystem
{
public:
    ParticleSystem();
    ~ParticleSystem();
    void setup(int id, float x, float y, float w, float h, string video, string svg, string alpha);
    void update();
    void draw();
    void drawParticles();
    void drawForces();
    
    void loadSettings();
    void saveSettings();
    void toggleSettings();
    
    void backToPlace(bool b);
    void breakFluid();
    void resetFluidParticles();
    void resetParticles();
    
    void setFluidForces(vector<ofxMPMForce*>* forces);
    ofxMPMFluid* getFluid();
    
    void mousePressed(int x, int y, int button);
    void mouseDragged(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    
    ofVec2f pos;
    ofVec2f size;
    int id;
    
private:
    void newVideo(ofVec2f newDim);
    void initVideoParticles();
    void setupGui();
    void updateFluid();
    void startFluidReshape();
    float getRegionValue(int x, int y, int size);

    bool hasContent;
    
    ofxUICanvas *gui;
    
    ofxAVFVideoPlayer videoPlayer;
//    ofVideoPlayer videoPlayer;
    ofVec2f videoDim;
    bool setupDim;
    
    int parChunk;
    float parPhysSize;
    vector<VideoParticle*> particles;
    ofVbo vbo;
    float noiseT = 0;
    float trailStrength;
    bool bUseAddMode;
    
    
    ofMutex drawMut;
    
    // blur
    float liquidBlur, blurAlpha;
    ofxTween tweenBlur;
    ofxEasingCubic tweenBlurEasing;
    
    ofShader shad_blurX, shad_blurY;
    ofFbo initialFbo, blurHor, blurVer;
    
    // draw parameters
    bool drawVideoBackground;
    float backgroundAlpha;
    bool isDrawShadow;
    float shadowIntensity;
    
    // behaviour parameters
    bool addSVGBorders;
    
    // fluid
    bool bDoFluid;
    bool isReshaping;
    float maxReshapeForce;
    ofxTween tweenReshapeForce;
    ofxEasingCubic tweenReshapeEasing;
    int breakTime, breakCounter;
    int cureTime, cureCounter;
    
    ofxMPMFluid fluid;
    float density;
    float stiffness;
    float bulkViscosity;
    float elasticity;
    float viscosity;
    float gravity;
        
    // fluid mask
    string svgPath;
    FluidMask fluidMask;
    string alphaFile;
    ofImage alphaImg;
};
#endif /* defined(__glitch_video__ParticleSystem__) */
