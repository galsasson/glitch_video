//
//  FluidMask.cpp
//  innovid_liquidvideo
//
//  Created by Gal Sasson on 1/29/14.
//
//

#include "FluidMask.h"

FluidMask::FluidMask()
{
    prevSize = 0;
}

void FluidMask::setup(ofxMPMFluid *f, string svgPath)
{
    fluid = f;
    svg.load(svgPath);
    
    obstacleSize = 25 / fluid->scaleFactor.x;
    createObstacles(30);
    tweenSize.setParameters(0, easeQuadSize, ofxTween::easeOut, obstacleSize, obstacleSize, 0, 0);
}

void FluidMask::addObstacle(float x, float y, float rad)
{
    ofVec2f pos(x, y);
    pos /= fluid->scaleFactor;
    fluid->addObstacle(new ofxMPMObstacle(pos.x, pos.y, rad));
}

void FluidMask::update()
{
    float s = tweenSize.update();
    if (s != prevSize)
    {
        float s2 = s*s;
        vector<ofxMPMObstacle*> obstacles = fluid->getObstacles();
        for (int i=0; i<obstacles.size(); i++)
        {
            obstacles[i]->radius = s;
            obstacles[i]->radius2 = s2;
        }
    }
    prevSize = s;
}

void FluidMask::draw()
{
    svg.draw();
}


void FluidMask::easeIn()
{
    tweenSize.setParameters(0, easeQuadSize, ofxTween::easeOut, 0, obstacleSize, 0, 0);
}

void FluidMask::easeOut()
{
    tweenSize.setParameters(0, easeQuadSize, ofxTween::easeIn, obstacleSize, 0, 5000, 0);
}



void FluidMask::createObstacles(float spacing)
{
    cout<<"Parsing svg paths\n";
    
    int pathNum = svg.getNumPath();
    
    for (int i=0; i<pathNum; i++)
    {
        // for each path get the outline
        vector<ofPolyline> outline = svg.getPathAt(i).getOutline();
        for (int l=0; l<outline.size(); l++)
        {
            ofPolyline resampledLine = outline[l].getResampledBySpacing(spacing);
            vector<ofPoint> points = resampledLine.getVertices();
            for (int v=0; v<points.size(); v++)
            {
//                cout<<"vertex at "<<points[v].x<<"x"<<points[v].y<<endl;
                addObstacle(points[v].x, points[v].y, obstacleSize);
            }
        }
    }
}


