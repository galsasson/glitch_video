//
//  KinectInput.cpp
//  innovid_liquidvideo
//
//  Created by Gal Sasson on 3/13/14.
//
//

#include "KinectInput.h"

KinectInput::KinectInput(ParticleSystem* _ps, ofxMPMFluid* _fluid)
{
    ps = _ps;
    fluid = _fluid;
    
    kinect.setRegistration(true);
    kinect.init();
    kinect.open();
    kinect.setCameraTiltAngle(-15);
    
    depthImg.allocate(kinect.width, kinect.height);
    oldDepthImg.allocate(kinect.width, kinect.height);
    diffImg.allocate(kinect.width, kinect.height);
    
    isMotion = false;
    
    setupGui();
}

void KinectInput::update()
{
    kinect.update();
    if (kinect.isFrameNew())
    {
        clearObstacles();
        
        depthImg.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
        depthImg.threshold(nearThreshold, CV_THRESH_TOZERO);
        
        // check for motion
        diffImg.absDiff(depthImg, oldDepthImg);
        oldDepthImg = depthImg;
        if (getAvgBrightness(diffImg, ofRectangle(motionRectMinX, motionRectMinY, motionRectMaxX, motionRectMaxY)) > motionThresh) {
            isMotion = true;
//            cout<<"detected motion"<<endl;
            ps->breakFluid();
        }
        else {
            isMotion = false;
//            return;
        }
        
        
        // find current contours
        depthImg.blur(blurAmount);
        depthImg.mirror(false, true);
        contourFinder.findContours(depthImg, contourMin, contourMax, 4, false);
        // now for every blob that we find we will try to locate smaller contours (hands)
        if (contourFinder.nBlobs > 0)
        {
//            for (int i=0; i<contourFinder.nBlobs; i++)
//            {
                ofRectangle rect = contourFinder.blobs.at(0).boundingRect;
                
                float maxVal = getMaxBrightness(depthImg, rect);
                depthImg.threshold(maxVal - handThreshold, CV_THRESH_TOZERO);
//            }
        
            // run contour finder again on the new image
            contourFinder.findContours(depthImg, contourMin, contourMax, 4, false);
            
            // put obstacle on center of contours
            for (int i=0; i<contourFinder.nBlobs; i++)
            {
                ofVec2f bPos = contourFinder.blobs.at(i).centroid;
                bPos /= ofVec2f(640, 480);
                bPos += pos / size;
                // normalize values
                ofxMPMObstacle *obs = new ofxMPMObstacle(bPos.x * fluid->gridSizeX, bPos.y * fluid->gridSizeY, obstacleSize);
                obstacles.push_back(obs);
                fluid->addObstacle(obs);
            }
        }
    }
}

void KinectInput::draw()
{
    ofPushMatrix();
    ofTranslate(pos.x, pos.y);
    ofScale(size.x/640, size.y/480);
    glEnable(GL_COLOR_LOGIC_OP);
    glLogicOp(GL_XOR);
    ofSetColor(100, 100, 255, 125);
//    if (isMotion)
//    {
        depthImg.blur(7);
        depthImg.draw(0, 0);
//    }
    glDisable(GL_COLOR_LOGIC_OP);
    
    ofEnableAlphaBlending();
    
    if (bDisplayMotionRect)
    {
        ofRect(motionRectMinX, motionRectMinY, motionRectMaxX, motionRectMaxY);
    }
    
    ofPopMatrix();

//    ofSetColor(255);
//    for (int i=0; i<obstacles.size(); i++)
//    {
//        ofEllipse(obstacles[i]->cx * fluid->scaleFactor.x, obstacles[i]->cy * fluid->scaleFactor.y, 20, 20);
//    }
    
}


float KinectInput::getAvgBrightness(ofxCvGrayscaleImage& img, ofRectangle rect)
{
    float avg=0;
    float count=0;
    
    for (int y=rect.getTop(); y<rect.getBottom(); y++)
    {
        for (int x=rect.getLeft(); x<rect.getRight(); x++)
        {
            int val = img.getPixels()[(int)(y*img.getWidth()+x)];
            if (val > 0)
            {
                avg += val;
                count++;
            }
        }
    }
    
    return avg/count;
}

float KinectInput::getMaxBrightness(ofxCvGrayscaleImage& img, ofRectangle rect)
{
    float max=0;
    
    for (int y=rect.getTop(); y<rect.getBottom(); y++)
    {
        for (int x=rect.getLeft(); x<rect.getRight(); x++)
        {
            int val = img.getPixels()[(int)(y*img.getWidth()+x)];
            if (val > max)
            {
                max = val;
            }
        }
    }
    
    return max;
}

void KinectInput::clearObstacles()
{
    for (int i=0; i<obstacles.size(); i++)
    {
        fluid->removeObstacle(obstacles[i]);
        delete obstacles[i];
    }
    
    obstacles.clear();
}


void KinectInput::setupGui()
{
    gui = new ofxUICanvas(200, 0, 200, 500);
    
    gui->addWidgetDown(new ofxUILabel("Kinect Input", OFX_UI_FONT_MEDIUM));
    
    nearThreshold = 100;
    gui->addSlider("Near Threshold", 0, 255, &nearThreshold);
    handThreshold = 10;
    gui->addSlider("Hand Threshold", 0, 255, &handThreshold);
    blurAmount = 19;
    gui->addIntSlider("Blur", 0, 33, &blurAmount);
    pos.x = 0;
    gui->addSlider("pos.x", 0, 800, &pos.x);
    pos.y = 0;
    gui->addSlider("pos.y", 0, 800, &pos.y);
    size.x = 0;
    gui->addSlider("Width", 1000, 3840, &size.x);
    size.y = 0;
    gui->addSlider("Height", 200, 2000, &size.y);
    contourMin = 50;
    gui->addSlider("Countour Min", 0, 10000, &contourMin);
    contourMax = 50;
    gui->addSlider("Countour Max", 0, 640*480, &contourMax);
    motionThresh = 30;
    gui->addSlider("Motion min", 0, 100, &motionThresh);
    obstacleSize = 4;
    gui->addSlider("Obstacle Size", 2, 20, &obstacleSize);
    bDisplayMotionRect = false;
    gui->addToggle("Display motion rect", &bDisplayMotionRect);
    motionRectMinX = 0;
    motionRectMaxX = 640;
    motionRectMinY = 0;
    motionRectMaxY = 480;
    gui->addRangeSlider("Motion width", 0, 640, &motionRectMinX , &motionRectMaxX);
    gui->addRangeSlider("Motion height", 0, 480, &motionRectMinY, &motionRectMaxY);
    
    loadSettings();
}

void KinectInput::loadSettings()
{
    gui->loadSettings("KinectInput.xml");
}

void KinectInput::saveSettings()
{
    gui->saveSettings("KinectInput.xml");
}

void KinectInput::toggleSettings()
{
    gui->toggleVisible();
}
