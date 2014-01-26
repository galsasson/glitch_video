//
//  VideoParticle.h
//  glitch_video
//
//  Created by Gal Sasson on 1/14/14.
//
//

#ifndef __glitch_video__VideoParticle__
#define __glitch_video__VideoParticle__

#include <iostream>
#include "ofMain.h"

class VideoParticle
{
public:
    VideoParticle(int _id, float x, float y, float z, float size);
    unsigned int fillVertices(vector<ofVec3f> &verts, float s, bool randomize = false);
    unsigned int fillIndices(vector<unsigned int> &indices);
    unsigned int fillUvs(vector<ofVec2f> &uvs, ofVec2f uv);
    void fillColors(vector<ofFloatColor> &colors, ofFloatColor color);
    
    void applyForce(ofVec3f force);
    ofVec3f getRestPosition();
    
    void update();
    
    unsigned int id;
    ofVec3f pos;
    ofVec3f vel;
    ofVec3f acc;
    ofVec3f restPos;
    float size;
    float halfSize;
    
private:
    void checkBounds();
    
    
};

#endif /* defined(__glitch_video__VideoParticle__) */
