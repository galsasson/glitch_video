//
//  KinectInput.cpp
//  innovid_liquidvideo
//
//  Created by Gal Sasson on 3/13/14.
//
//

#include "KinectInput.h"

KinectInput::KinectInput(ofxMPMFluid* _fluid)
{
    fluid = _fluid;
    
    kinect.setRegistration(true);
    kinect.init();
    kinect.open();
    kinect.setCameraTiltAngle(15);
    
    depthImg.allocate(kinect.width, kinect.height);
    oldDepthImg.allocate(kinect.width, kinect.height);
    
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
        depthImg.blur(blurAmount);
        depthImg.mirror(false, true);
        
        contourFinder.findContours(depthImg, contourMin, contourMax, 4, false);
        // now for every blob that we find we will try to locate smaller contours (hands)
        if (contourFinder.nBlobs > 0)
        {
            for (int i=0; i<contourFinder.nBlobs; i++)
            {
                ofRectangle rect = contourFinder.blobs.at(i).boundingRect;
                
                float maxVal = getMaxBrightness(depthImg, rect);
                depthImg.threshold(maxVal - handThreshold, CV_THRESH_TOZERO);
            }
        
            // run contour finder again on the new image
            contourFinder.findContours(depthImg, contourMin, contourMax, 4, false);
            
            // put obstacle on center of contours
            for (int i=0; i<contourFinder.nBlobs; i++)
            {
                ofVec2f bPos = contourFinder.blobs.at(i).centroid;
                bPos /= ofVec2f(640, 480);
                bPos += pos / size;
                // normalize values
                ofxMPMObstacle *obs = new ofxMPMObstacle(bPos.x * fluid->gridSizeX, bPos.y * fluid->gridSizeY, 4);
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
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    ofSetColor(255);
//    contourFinder.draw(0, 0);
    depthImg.blurHeavily();
    depthImg.draw(0, 0);
    
    ofEnableAlphaBlending();
    ofPopMatrix();

    ofSetColor(255);
    for (int i=0; i<obstacles.size(); i++)
    {
        ofEllipse(obstacles[i]->cx * fluid->scaleFactor.x, obstacles[i]->cy * fluid->scaleFactor.y, 20, 20);
    }
    
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
