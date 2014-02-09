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

#include "VideoParticle.h"
#include "FluidMask.h"

class ParticleSystem
{
public:
    ParticleSystem();
    ~ParticleSystem();
    void setup(int id, float x, float y, float w, float h, string video);
    void applyBorders(string svgPath);
    void update();
    void draw();
    
    void loadSettings();
    void saveSettings();
    void toggleSettings();
    
    void backToPlace(bool b);
    void breakFluid();
    void resetParticles();
    
    void setFluidForces(vector<ofxMPMForce*>* forces);
    ofxMPMFluid* getFluidRef();
    
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

    bool hasContent;
    
    ofxUICanvas *gui;
    
    ofxAVFVideoPlayer videoPlayer;
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
    float blurAmount, blurAlpha, blurVideo, blurLiquid;
    ofShader shad_blurX, shad_blurY;
    ofFbo initialFbo, blurHor, blurVer;
    
    // fluid
    bool bDoFluid;
    bool isReshaping;
    float reshapeForce;
    int breakPoint;
    int breakTime, breakCounter;
    int cureTime, cureCounter;
    
    ofxMPMFluid fluid;
    float density;
    float stiffness;
    float bulkViscosity;
    float elasticity;
    float viscosity;
    float gravity;
    
    // obstacles
    bool drawObs;
    ofVec2f currObsCenter;
    float currObsRad;
    ofVec2f lineStart;
    ofVec2f lineEnd;
    
    // fluid mask
    FluidMask fluidMask;
};
#endif /* defined(__glitch_video__ParticleSystem__) */
