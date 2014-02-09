//
//  ParticleSystem.cpp
//  glitch_video
//
//  Created by Gal Sasson on 1/23/14.
//
//

#include "ParticleSystem.h"

ParticleSystem::ParticleSystem()
{
    bDoFluid = false;
}

ParticleSystem::~ParticleSystem()
{
    videoPlayer.stop();
    videoPlayer.close();
}

void ParticleSystem::setup(int _id, float x, float y, float w, float h, string video)
{
    id = _id;
    pos = ofVec2f(x, y);
    size = ofVec2f(w, h);
    
    setupGui();
    
    //    videoPlayer.loadMovie("videos/sunrise_static.mp4");
    videoPlayer.loadMovie(video);
    //    videoPlayer.loadMovie("videos/xx.mp4");
    videoPlayer.setLoopState(OF_LOOP_NORMAL);
    videoPlayer.play();
    setupDim = true;
    
    // blur
    shad_blurX.load("shaders/simpleBlurHorizontal");
    shad_blurY.load("shaders/simpleBlurVertical");
    
    hasContent = false;
    isReshaping = true;
    
}

void ParticleSystem::applyBorders(string svgPath)
{
    fluidMask.setup(&fluid, svgPath);
}

void ParticleSystem::update()
{
    
    videoPlayer.update();
    if (videoPlayer.isFrameNew()) {
        if (setupDim) {
            newVideo(ofVec2f(videoPlayer.getWidth(), videoPlayer.getHeight()));
            setupDim = false;
        }
    }    
    
    if (bDoFluid)
    {
        updateFluid();
        vector<ofxMPMParticle*> fParticles = fluid.getParticles();
        
        vector<ofVec3f> verts;
        for (int i=0; i<particles.size(); i++)
        {
            if (isReshaping)
            {
                ofVec3f force = particles[i]->getRestPosition() - (ofVec3f(fParticles[i]->x, fParticles[i]->y, 0) * ofVec3f(fluid.scaleFactor.x, fluid.scaleFactor.y, 0));
                fParticles[i]->u = force.x*reshapeForce;
                fParticles[i]->v = force.y*reshapeForce;
            }
            particles[i]->pos.set(fParticles[i]->x*fluid.scaleFactor.x, fParticles[i]->y*fluid.scaleFactor.y, 0);
            particles[i]->update();
            particles[i]->fillVertices(verts, parPhysSize);
    //        noiseT += 0.01;
        }
        
        vbo.setVertexData(&verts[0], verts.size(), GL_DYNAMIC_DRAW);
    }
}

void ParticleSystem::draw()
{
    if (!hasContent) {
        return;
    }
    
    ofPushMatrix();
    ofTranslate(pos);
    
    initialFbo.begin();
    drawMut.lock();
    if (videoPlayer.isFrameNew())
    {
        if (videoPlayer.getTexture()) {
            videoPlayer.getTexture()->bind();
        }
        ofEnableAlphaBlending();
        ofSetColor(0, 0, 0, 255 - 255*trailStrength);
        ofRect(0, 0, size.x, size.y);
        glDisable(GL_DEPTH_TEST);
        if (bUseAddMode) {
            ofEnableBlendMode(OF_BLENDMODE_ADD);
        }
        
        vbo.drawElements(GL_TRIANGLES, particles.size()*6);
        
        if (videoPlayer.getTexture()) {
            videoPlayer.getTexture()->unbind();
        }
    }
    drawMut.unlock();
    
    initialFbo.end();
    
    blurHor.begin();
    shad_blurX.begin();
    shad_blurX.setUniform1f("blurAmnt", blurAmount);
    shad_blurX.setUniform1f("blurAlpha", blurAlpha);
    
    ofClear(0);
    initialFbo.draw(0, 0);
    
    
    shad_blurX.end();
    blurHor.end();
    
    //        blurVer.begin();
    ofClear(0);
    //        initialFbo.draw(0, 0);
    shad_blurY.begin();
    shad_blurY.setUniform1f("blurAmnt", blurAmount);
    shad_blurX.setUniform1f("blurAlpha", blurAlpha);
    
    blurHor.draw(0, 0);
    
    shad_blurY.end();
    //        blurVer.end();
    
    //        blurVer.draw(0, 0);
    
    if (drawObs) {
        ofSetColor(0, 0, 255, 100);
        ofNoFill();
        ofLine(lineStart, lineEnd);
//        ofFill();
//        ofEllipse(currObsCenter.x, currObsCenter.y, currObsRad*2, currObsRad*2);
    }
    
    
    // draw fluid borders
    fluidMask.draw();
    
    ofPopMatrix();
}

void ParticleSystem::newVideo(ofVec2f dim)
{
    videoDim = dim;
    cout<<"dimensions["<<id<<"]: "<<videoDim<<endl;
    
    blurHor.allocate(size.x, size.y, GL_RGBA);
    blurHor.begin();
    ofClear(0);
    blurHor.end();
    blurVer.allocate(size.x, size.y, GL_RGBA);
    blurVer.begin();
    ofClear(0);
    blurVer.end();
    initialFbo.allocate(size.x, size.y, GL_RGBA);
    initialFbo.begin();
    ofClear(0);
    initialFbo.end();
    
    // init the particle system
    initVideoParticles();
    
    hasContent = true;
}


void ParticleSystem::initVideoParticles()
{
    // create particles and geometry vbo
    vector<ofVec3f> verts;
    vector<ofIndexType> indices;
    vector<ofVec2f> uvs;
    vector<ofFloatColor> colors;
    // create particles, and set texture coordinates
    for (int y=0; y<videoDim.y; y+=parChunk)
    {
        for (int x=0; x<videoDim.x; x+=parChunk)
        {
            // create a new particle on the grid
            float px = x / videoDim.x * size.x;
            float py = y / videoDim.y * size.y;
            VideoParticle *vp = new VideoParticle(particles.size(), px, py, 0, parChunk);
            vp->fillVertices(verts, parPhysSize);
            vp->fillIndices(indices);
            vp->fillUvs(uvs, ofVec2f(x, y));
            vp->fillColors(colors, ofFloatColor(1, 1, 1));
            particles.push_back(vp);
        }
    }
    
    vbo.setVertexData(&verts[0], verts.size(), GL_DYNAMIC_DRAW);
    vbo.setTexCoordData(&uvs[0], uvs.size(), GL_STATIC_DRAW);
    vbo.setColorData(&colors[0], colors.size(), GL_STATIC_DRAW);
    vbo.setIndexData(&indices[0], indices.size(), GL_STATIC_DRAW);
    
    cout<<"Initializing fluid["<<id<<"]: number of particles ("<<particles.size()<<")\n";
    fluid.setup(particles.size(), videoDim.x/10, videoDim.y/10);
    fluid.numParticles = particles.size();
    fluid.scaleFactor.x = size.x / fluid.getGridSizeX();
    fluid.scaleFactor.y = size.y / fluid.getGridSizeY();
    
    // init particle positions to rest
    resetParticles();
}

void ParticleSystem::setupGui()
{
    gui = new ofxUICanvas(0, 0, 200, ofGetHeight());
    
    char name[128];
    sprintf(name, "Particle_System_%d", id);
    gui->addWidgetDown(new ofxUILabel(name, OFX_UI_FONT_MEDIUM));
    
    parChunk = 10;
    gui->addIntSlider("Particle Chunk", 1, 50, &parChunk);
    parPhysSize = 10;
    gui->addSlider("Particle Size", 0, 50, &parPhysSize);
    blurAmount = 0;
    gui->addSlider("Blur Amount", 0, 10, &blurAmount);
    blurAlpha = 1;
    gui->addSlider("Blur Alpha", 0, 1, &blurAlpha);
    trailStrength = 0;
    gui->addSlider("Trail Strength", 0, 1, &trailStrength);
    bUseAddMode = false;
    gui->addToggle("Add Mode", &bUseAddMode);
    reshapeForce = 0.01;
    gui->addSlider("Reshape force", 0, 0.1, &reshapeForce);
    breakPoint = 20;
    gui->addIntSlider("Break point", 1, 100, &breakPoint);
    breakTime = 100;
    gui->addIntSlider("Break time (frames)", 1, 1000, &breakTime);
    cureTime = 120;
    gui->addIntSlider("Cure time (frames)", 1, 1100, &cureTime);
    
    gui->addSpacer();
    gui->addLabel("Fluid");
    density = 5;
    gui->addSlider("Density", 0, 3, &density);
    stiffness = 0.5;
    gui->addSlider("Stiffness", 0, 2, &stiffness);
    viscosity = 1;
    gui->addSlider("Viscosity", 0, 4, &viscosity);
    bulkViscosity = 3.0;
    gui->addSlider("Buld Viscosity", 0, 10, &bulkViscosity);
    elasticity = 1;
    gui->addSlider("Elasticity", 0, 4, &elasticity);
    gravity = 0.002;
    gui->addSlider("Gravity", 0, 0.2, &gravity);
    
    loadSettings();
}

void ParticleSystem::loadSettings()
{
    char name[128];
    sprintf(name, "GUI/psSettings-%d.xml", id);
    gui->loadSettings(name);
}

void ParticleSystem::saveSettings()
{
    char name[128];
    sprintf(name, "GUI/psSettings-%d.xml", id);
    gui->saveSettings(name);
}

void ParticleSystem::toggleSettings()
{
    gui->toggleVisible();
}

void ParticleSystem::updateFluid()
{
    fluid.densitySetting = density;
    fluid.stiffness = stiffness;
    fluid.bulkViscosity = bulkViscosity;
    fluid.viscosity = viscosity;
    fluid.elasticity = elasticity;
    fluid.gravity = gravity/10;
    
    fluid.scaleFactor.x = size.x / fluid.getGridSizeX();
    fluid.scaleFactor.y = size.y / fluid.getGridSizeY();
    
    fluid.update();
    
    if (fluid.forces->size() > breakPoint) {
        breakFluid();
    }
    
    // update break counters
    if (breakCounter >= 0) {
        breakCounter--;
        if (breakCounter==0) {
            isReshaping = true;
        }
    }
    if (cureCounter >= 0) {
        cureCounter--;
        if (cureCounter==0) {
            bDoFluid = false;
            resetParticles();
        }
    }
}

void ParticleSystem::backToPlace(bool b)
{
    isReshaping = b;
}

void ParticleSystem::breakFluid()
{
    bDoFluid = true;
    isReshaping = false;
    breakCounter = breakTime;
    cureCounter = cureTime;
}

void ParticleSystem::resetParticles()
{
    vector<ofxMPMParticle*> par = fluid.getParticles();
    for (int i=0; i<par.size(); i++)
    {
        par[i]->x = ofMap(particles[i]->restPos.x, 0, size.x, 2, fluid.getGridSizeX()-3);
        par[i]->y = ofMap(particles[i]->restPos.y, 0, size.y, 2, fluid.getGridSizeY()-3);
    }
}

void ParticleSystem::setFluidForces(vector<ofxMPMForce *> *forces)
{
    fluid.forces = forces;
}

ofxMPMFluid* ParticleSystem::getFluidRef()
{
    return &fluid;
}

void ParticleSystem::mousePressed(int x, int y, int button)
{
//    currObsCenter = ofVec2f(x-pos.x, y-pos.y);
    lineStart = lineEnd = ofVec2f(x-pos.x, y-pos.y);
//    currObsRad = 0;
    drawObs = true;
}

void ParticleSystem::mouseDragged(int x, int y, int button)
{
//    currObsRad = ofVec2f(x-pos.x, y-pos.y).distance(currObsCenter);
    lineEnd = ofVec2f(x-pos.x, y-pos.y);
}

void ParticleSystem::mouseReleased(int x, int y, int button)
{
//    currObsCenter /= fluid.scaleFactor;
//    currObsRad /= fluid.scaleFactor.x;
//    fluid.addObstacle(new ofxMPMObstacle(currObsCenter.x, currObsCenter.y, currObsRad));
    ofVec2f line = (lineEnd - lineStart);
    
    lineEnd = ofVec2f(x-pos.x, y-pos.y);
    for (float t=0; t<=1; t+=1/(line.length()/14))
    {
        ofVec2f op = (lineStart + line*t) / fluid.scaleFactor;
        fluid.addObstacle(new ofxMPMObstacle(op.x, op.y, 2));
    }
    drawObs = false;
}

