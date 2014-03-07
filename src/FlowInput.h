//
//  FlowInput.h
//  innovid_liquidvideo
//
//  Created by Gal Sasson on 1/26/14.
//
//

#ifndef __innovid_liquidvideo__FlowInput__
#define __innovid_liquidvideo__FlowInput__

#include <iostream>
#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxMPMFluid.h"
#include "ofxMPMForce.h"
#include "ofxUI.h"

class FlowInput
{
public:
    FlowInput();
    void listen(int port);
    void update(ofxMPMFluid& fluid);
    void draw();
    vector<ofxMPMForce*>* getForcesRef();
    void loadSettings();
    void saveSettings();
    void toggleSettings();
    
    ofxOscReceiver oscRecv;
    vector<ofxMPMForce*> forces;
    
private:
    void setupGui();
    
    int maxForces;
    float forceStrength;
    float forceDecay;
    int forceLife;
    
    ofxUICanvas *gui;
};

#endif /* defined(__innovid_liquidvideo__FlowInput__) */
