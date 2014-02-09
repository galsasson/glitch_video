//
//  FlowInput.cpp
//  innovid_liquidvideo
//
//  Created by Gal Sasson on 1/26/14.
//
//

#include <strings.h>
#include "FlowInput.h"

FlowInput::FlowInput()
{
    setupGui();
}

void FlowInput::listen(int port)
{
    oscRecv.setup(port);
}

void FlowInput::update()
{
    while (oscRecv.hasWaitingMessages())
    {
        ofxOscMessage m;
        oscRecv.getNextMessage(&m);
        if (std::strcmp(m.getAddress().data(), "/iv/force") == 0)
        {
            if (forces.size() < maxForces) {
                ofVec2f origin = ofVec2f(m.getArgAsFloat(0), m.getArgAsFloat(1));
                ofVec2f force = ofVec2f(m.getArgAsFloat(2)*forceStrength, m.getArgAsFloat(3)*forceStrength);
                forces.push_back(new ofxMPMForce(origin, force, forceLife));
            }
        }
    }
    
    // update and kill dead forces
    for (int i=0; i<forces.size(); i++)
    {
        forces[i]->update();
        if ( !forces[i]->alive() )
        {
            ofxMPMForce *f = forces[i];
            forces.erase(forces.begin() + i);
            i--;
            delete f;
        }
    }
    
//    cout<<"forces = "<<forces.size()<<endl;
}

vector<ofxMPMForce*>* FlowInput::getForcesRef()
{
    return &forces;
}

void FlowInput::setupGui()
{
    gui = new ofxUICanvas(200, 0, 200, 200);
    
    gui->addWidgetDown(new ofxUILabel("FlowField Input", OFX_UI_FONT_MEDIUM));
    
    maxForces = 100;
    gui->addIntSlider("Max forces", 1, 500, &maxForces);
    forceStrength = 1;
    gui->addSlider("Force strength", 0, 2, &forceStrength);
    forceLife = 1;
    gui->addIntSlider("Force life", 1, 50, &forceLife);
    forceDecay = 1;
    gui->addSlider("Force decay", 0, 1, &forceDecay);
    
    loadSettings();
}

void FlowInput::loadSettings()
{
    gui->loadSettings("FlowInput.xml");
}

void FlowInput::saveSettings()
{
    gui->saveSettings("FlowInput.xml");
}

void FlowInput::toggleSettings()
{
    gui->toggleVisible();
}
