//
//  KinectInput.h
//  innovid_liquidvideo
//
//  Created by Gal Sasson on 3/13/14.
//
//

#ifndef __innovid_liquidvideo__KinectInput__
#define __innovid_liquidvideo__KinectInput__

#include <iostream>
#include "ofMain.h"
#include "ofxKinect.h"
#include "ofxOpenCv.h"
#include "ofxUI.h"
#include "ofxMPMFluid.h"
#include "ParticleSystem.h"

class KinectInput
{
public:
    KinectInput(ParticleSystem *ps, ofxMPMFluid *_fluid);
    
    void update();
    void draw();
    void loadSettings();
    void saveSettings();
    void toggleSettings();
    
    ofxKinect kinect;
    ofxCvGrayscaleImage depthImg;
    ofxCvGrayscaleImage oldDepthImg;
    ofxCvGrayscaleImage diffImg;
    
    ofxCvContourFinder contourFinder;
    
private:
    ParticleSystem *ps;
    ofxMPMFluid *fluid;
    vector<ofxMPMObstacle*> obstacles;
    void clearObstacles();
    
    float getAvgBrightness(ofxCvGrayscaleImage& img, ofRectangle rect);
    float getMaxBrightness(ofxCvGrayscaleImage& img, ofRectangle rect);
    
    float nearThreshold;
    float handThreshold;
    int blurAmount;
    ofVec2f pos;
    ofVec2f size;
    float contourMin;
    float contourMax;
    float obstacleSize;
    float motionThresh;
    
    float motionRectMinX;
    float motionRectMaxX;
    float motionRectMinY;
    float motionRectMaxY;
    
//    ofRange motionRectHor;
    ofRectangle motionRectVel;
    
    bool isMotion;
    
    // GUI
    ofxUICanvas *gui;
    void setupGui();
};
#endif /* defined(__innovid_liquidvideo__KinectInput__) */
