//
//  VideoParticle.cpp
//  glitch_video
//
//  Created by Gal Sasson on 1/14/14.
//
//

#include "VideoParticle.h"

VideoParticle::VideoParticle(int _id, float x, float y, float z, float s)
{
    id = _id;
    pos = ofVec3f(x, y, z);
    restPos = pos;
    size = s;
    halfSize = s/2;
}

unsigned int VideoParticle::fillVertices(vector<ofVec3f> &verts, float s, bool randomize)
{
    float sdiv2 = s/2;
    if (randomize)
    {
        verts.push_back(pos + ofVec3f(-sdiv2+ofRandom(s/4), -sdiv2+ofRandom(s/4), 0));
        verts.push_back(pos + ofVec3f(sdiv2+ofRandom(s/4), -sdiv2+ofRandom(s/4), 0));
        verts.push_back(pos + ofVec3f(sdiv2+ofRandom(s/4), sdiv2+ofRandom(s/4), 0));
        verts.push_back(pos + ofVec3f(-sdiv2+ofRandom(s/4), sdiv2+ofRandom(s/4), 0));
    }
    else {
        verts.push_back(pos + ofVec3f(-sdiv2, -sdiv2, 0));
        verts.push_back(pos + ofVec3f(sdiv2, -sdiv2, 0));
        verts.push_back(pos + ofVec3f(sdiv2, sdiv2, 0));
        verts.push_back(pos + ofVec3f(-sdiv2, sdiv2, 0));
    }
    return 4;
}

unsigned int VideoParticle::fillIndices(vector<unsigned int> &indices)
{
    unsigned int start = id*4;
    
    indices.push_back(start+0);
    indices.push_back(start+1);
    indices.push_back(start+2);
    indices.push_back(start+2);
    indices.push_back(start+3);
    indices.push_back(start+0);
    
    return 6;
}

unsigned int VideoParticle::fillUvs(vector<ofVec2f> &uvs, ofVec2f uv)
{
    uvs.push_back(ofVec2f(uv.x, uv.y));
    uvs.push_back(ofVec2f(uv.x + size, uv.y));
    uvs.push_back(ofVec2f(uv.x+ size, uv.y + size));
    uvs.push_back(ofVec2f(uv.x, uv.y + size));
    
    return 4;
}

void VideoParticle::fillColors(vector<ofFloatColor> &colors, ofFloatColor color)
{
    colors.push_back(color);
    colors.push_back(color);
    colors.push_back(color);
    colors.push_back(color);
}

void VideoParticle::applyForce(ofVec3f force)
{
    acc += force;
}

ofVec3f VideoParticle::getRestPosition()
{
    return restPos;
}

void VideoParticle::update()
{
    vel += acc;
    pos += vel;
    
    acc = acc * 0;
    
    // friction
    vel *= 0.98;
    
    checkBounds();
}

void VideoParticle::checkBounds()
{
    if (pos.x < 0) {
        pos.x = 0;
        vel.x *= -1;
    }
    else if (pos.x > ofGetWidth()) {
        pos.x = ofGetWidth();
        vel.x *= -1;
    }
    if (pos.y < 0) {
        pos.y = 0;
        vel.y *= -1;
    }
    else if (pos.y > ofGetHeight()) {
        pos.y = ofGetHeight();
        vel.y *= -1;
    }
}
