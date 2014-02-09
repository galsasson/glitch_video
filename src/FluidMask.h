//
//  FluidMask.h
//  innovid_liquidvideo
//
//  Created by Gal Sasson on 1/29/14.
//
//

#ifndef __innovid_liquidvideo__FluidMask__
#define __innovid_liquidvideo__FluidMask__

#include <iostream>
#include "ofMain.h"
#include "ofxMPMFluid.h"
#include "ofxSvg.h"
#include "ofxTween.h"

class FluidMask
{
public:
    FluidMask();
    void setup(ofxMPMFluid *f, string svgPath);
    void addObstacle(float x, float y, float rad);
    void update();
    void draw();
    
    void easeIn();
    void easeOut();
    
    
private:
    void createObstacles(float spacing=10);
    
    float obstacleSize;
    float prevSize;
    ofxMPMFluid *fluid;
    ofxSVG svg;
    ofxTween tweenSize;
    ofxEasingQuad easeQuadSize;
//    vector<ofxMPMObstacle*> obstacles;
};

#endif /* defined(__innovid_liquidvideo__FluidMask__) */
