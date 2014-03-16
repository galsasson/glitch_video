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

void ParticleSystem::setup(int _id, float x, float y, float w, float h, string video, string svg, string alpha)
{
    id = _id;
    pos = ofVec2f(x, y);
    size = ofVec2f(w, h);
    svgPath = svg;
    alphaFile = alpha;
    
    alphaImg.loadImage(alphaFile);
    
    setupGui();
    
    //    videoPlayer.loadMovie("videos/sunrise_static.mp4");
    videoPlayer.loadMovie(video);
    //    videoPlayer.loadMovie("videos/xx.mp4");
    videoPlayer.setLoopState(OF_LOOP_NORMAL);
    videoPlayer.setVolume(0);
    videoPlayer.play();
    setupDim = true;
    
    // blur
    shad_blurX.load("shaders/simpleBlurHorizontal");
    shad_blurY.load("shaders/simpleBlurVertical");
    tweenBlur.setParameters(0, tweenBlurEasing, ofxTween::easeOut, 0, 0, 0, 0);
    
    hasContent = false;
    isReshaping = true;
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
    
    
    // update particle positions
    vector<ofVec3f> verts;
    verts.reserve(particles.size()*4);
    if (bDoFluid)
    {
        updateFluid();
        vector<ofxMPMParticle*> fParticles = fluid.getParticles();
        
        for (int i=0; i<particles.size(); i++)
        {
            particles[i]->pos.set(fParticles[i]->x*fluid.scaleFactor.x, fParticles[i]->y*fluid.scaleFactor.y, 0);
            particles[i]->update();
            particles[i]->fillVertices(verts, parPhysSize);
        }
    }
    else if (isReshaping)
    {
        float reshapeForce = tweenReshapeForce.update();
        
        for (int i=0; i<particles.size(); i++)
        {
            ofVec3f offset = particles[i]->getRestPosition() - particles[i]->pos;
            particles[i]->vel = offset*reshapeForce;
            particles[i]->update();
            particles[i]->fillVertices(verts, parPhysSize);
        }
    }
    else {
        for (int i=0; i<particles.size(); i++)
        {
            ofVec3f offset = particles[i]->getRestPosition() - particles[i]->pos;
            particles[i]->update();
            particles[i]->fillVertices(verts, parPhysSize);
        }
    }
    vbo.setVertexData(&verts[0], verts.size(), GL_DYNAMIC_DRAW);
    
    // update break counters
    if (breakCounter >= 0) {
        breakCounter--;
        if (breakCounter==0) {
            startFluidReshape();
        }
    }
    if (cureCounter >= 0) {
        cureCounter--;
        if (cureCounter==0) {
            resetParticles();
        }
    }
    
}

void ParticleSystem::drawParticles()
{
    // draw shadow
    if (isDrawShadow)
    {
        ofPushMatrix();
        ofTranslate(5, 5);
        ofSetColor(0, shadowIntensity);
        glDisable(GL_DEPTH_TEST);
        vbo.drawElements(GL_TRIANGLES, particles.size()*6);
        ofPopMatrix();
    }
    
    videoPlayer.getTexture()->bind();
    ofEnableAlphaBlending();
    ofSetColor(0, 0, 0, 255 - 255*trailStrength);
    ofRect(0, 0, size.x, size.y);
    ofSetColor(255);
    glDisable(GL_DEPTH_TEST);
    if (bUseAddMode) {
        ofEnableBlendMode(OF_BLENDMODE_ADD);
    }
    vbo.drawElements(GL_TRIANGLES, particles.size()*6);
    videoPlayer.getTexture()->unbind();
}

#define DO_BLUR

void ParticleSystem::draw()
{
    if (!hasContent) {
        return;
    }
    
    ofPushMatrix();
    ofTranslate(pos);
    
#ifdef DO_BLUR
    float blur = tweenBlur.update();
    
    initialFbo.begin();
    if (videoPlayer.isFrameNew())
    {
        drawParticles();
    }
    initialFbo.end();
    

    blurHor.begin();
    shad_blurX.begin();
    shad_blurX.setUniform1f("blurAmnt", blur);
    initialFbo.draw(0, 0);
    shad_blurX.end();
    blurHor.end();

    // draw background
    if (drawVideoBackground)
    {
        ofSetColor(255, 255);
        videoPlayer.draw(0, 0);
        ofEnableAlphaBlending();
        ofSetColor(0, 255 - backgroundAlpha);             // background alpha here
        ofRect(0, 0, size.x, size.y);
    }

    shad_blurY.begin();
    shad_blurY.setUniform1f("blurAmnt", blur);
    blurHor.draw(0, 0);
    shad_blurY.end();

#else
//    if (videoPlayer.isFrameNew())
//    {
    ofEnableAlphaBlending();
    
    videoPlayer.draw(0, 0);
    ofSetColor(0, 0, 0, 100);
    ofFill();
    ofRect(0, 0, size.x, size.y);
    drawParticles();
//    }
#endif
    //        blurVer.end();
    
    //        blurVer.draw(0, 0);
    
    // draw fluid borders
    //fluidMask.draw();
    
    ofPopMatrix();
}

void ParticleSystem::drawForces()
{
    if (!fluid.forces) {
        return;
    }
    
    ofSetColor(255, 255, 255, 100);
    ofNoFill();
    ofSetLineWidth(20);
    vector<ofxMPMForce*>* forces = fluid.forces;
    for (int i=0; i<forces->size(); i++)
    {
        ofVec2f origin = (*forces)[i]->origin * fluid.scaleFactor;
        ofVec2f force = (*forces)[i]->force * fluid.scaleFactor;
        ofLine(origin, origin+force);
    }
    
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
    ofSetColor(0, 0, 0);
    ofRect(0, 0, size.x, size.y);
//    ofClear(0);
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
    
    // make sure that video and alpha dimensions match
    if (videoDim.x != alphaImg.width ||
        videoDim.y != alphaImg.height) {
        cout<<"warning! video and alpha dimensions do not match"<<endl;
    }
    
    // create particles, and set texture coordinates
    for (int y=0; y<videoDim.y; y+=parChunk)
    {
        for (int x=0; x<videoDim.x; x+=parChunk)
        {
//            cout << getRegionValue(x, y, parChunk);
            if (getRegionValue(x, y, parChunk) < 255)
            {
                // create a new particle on the grid
                float px = x / videoDim.x * size.x;
                float py = y / videoDim.y * size.y;
                VideoParticle *vp = new VideoParticle(particles.size(), px, py, 0, parChunk, size);
                vp->fillVertices(verts, parPhysSize);
                vp->fillIndices(indices);
                vp->fillUvs(uvs, ofVec2f(x, y));
//                vp->fillColors(colors, ofFloatColor(1, 1, 1));
                particles.push_back(vp);
            }
        }
    }
    
    vbo.setVertexData(&verts[0], verts.size(), GL_DYNAMIC_DRAW);
    vbo.setTexCoordData(&uvs[0], uvs.size(), GL_STATIC_DRAW);
//    vbo.setColorData(&colors[0], colors.size(), GL_STATIC_DRAW);
    vbo.setIndexData(&indices[0], indices.size(), GL_STATIC_DRAW);
    
    cout<<"Initializing fluid["<<id<<"]: number of particles ("<<particles.size()<<")\n";
    fluid.setup(particles.size(), videoDim.x/14, videoDim.y/14);
    fluid.numParticles = particles.size();
    fluid.scaleFactor.x = size.x / fluid.getGridSizeX();
    fluid.scaleFactor.y = size.y / fluid.getGridSizeY();
    fluid.mouseForce = 12;
    fluid.bDoMouse = false;
    
    // init particle positions to rest
    resetParticles();

    // create fluid obstacles according to svgPath
    if (addSVGBorders) {
        if (svgPath != "") {
            fluidMask.setup(&fluid, svgPath);
        }
    }
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
    liquidBlur = 0;
    gui->addSlider("Liquid Blur", 0, 10, &liquidBlur);
    trailStrength = 0.8;
    gui->addSlider("Trail Strength", 0.5, 1, &trailStrength);
    bUseAddMode = false;
    gui->addToggle("Add Mode", &bUseAddMode);
    maxReshapeForce = 0.01;
    gui->addSlider("Max Reshape Force", 0, 0.2, &maxReshapeForce);
    breakTime = 2;
    gui->addIntSlider("Break time (frames)", 1, 30, &breakTime);
    cureTime = 120;
    gui->addIntSlider("Cure time (frames)", 1, 1100, &cureTime);

    isDrawShadow = false;
    gui->addToggle("Draw Shadow", &isDrawShadow);
    shadowIntensity = 0;
    gui->addSlider("Shadow Intensity", 0, 255, &shadowIntensity);
    drawVideoBackground = true;
    gui->addToggle("Video Background", &drawVideoBackground);
    backgroundAlpha = 0;
    gui->addSlider("Background Alpha", 0, 255, &backgroundAlpha);
    addSVGBorders = true;
    gui->addToggle("Add SVG Borders", &addSVGBorders);
    
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
    fluidMask.update();
    
    fluid.densitySetting = density;
    fluid.stiffness = stiffness;
    fluid.bulkViscosity = bulkViscosity;
    fluid.viscosity = viscosity;
    fluid.elasticity = elasticity;
    fluid.gravity = gravity/10;
    
    fluid.scaleFactor.x = size.x / fluid.getGridSizeX();
    fluid.scaleFactor.y = size.y / fluid.getGridSizeY();
    
    
    fluid.bDoMouse = false;
    fluid.update();
//    fluid.bDoMouse = true;
//    fluid.update(ofGetMouseX(), ofGetMouseY());
    
}

void ParticleSystem::backToPlace(bool b)
{
    breakCounter = 1;
//    isReshaping = b;
}

void ParticleSystem::breakFluid()
{
    // only if we are in plain video mode
    if (!bDoFluid)
    {
        bDoFluid = true;
        tweenBlur.setParameters(0, tweenBlurEasing, ofxTween::easeOut, 0, liquidBlur, 300, 0);
    }
    
    fluidMask.easeIn();
    
    breakCounter = breakTime;
    cureCounter = cureTime;
}

void ParticleSystem::resetFluidParticles()
{
    vector<ofxMPMParticle*> fParticles = fluid.getParticles();
    for (int i=0; i<fParticles.size(); i++)
    {
        fParticles[i]->x = ofMap(particles[i]->restPos.x, 0, size.x, 2, fluid.getGridSizeX()-3);
        fParticles[i]->y = ofMap(particles[i]->restPos.y, 0, size.y, 2, fluid.getGridSizeY()-3);
        fParticles[i]->u = 0;
        fParticles[i]->v = 0;
    }
}

void ParticleSystem::resetParticles()
{
    isReshaping = false;
//    cout<<"inside resetParticles\n";

    resetFluidParticles();
    for (int i=0; i<particles.size(); i++)
    {
        particles[i]->reset();
    }
}

void ParticleSystem::startFluidReshape()
{
    // update particles velocity from fluid simulation velocities
    vector<ofxMPMParticle*> fParticles = fluid.getParticles();

    for (int i=0; i<particles.size(); i++)
    {
        particles[i]->vel.x = fParticles[i]->u;
        particles[i]->vel.y = fParticles[i]->v;
    }
    
    
    bDoFluid = false;
    resetFluidParticles();
    isReshaping = true;
    fluidMask.easeOut();
    tweenReshapeForce.setParameters(0, tweenReshapeEasing, ofxTween::easeIn, maxReshapeForce, 0, 3000, 0);
    tweenBlur.setParameters(0, tweenBlurEasing, ofxTween::easeIn, liquidBlur, 0, 300, 0);
}

float ParticleSystem::getRegionValue(int sx, int sy, int size)
{
    int colorVal=0;
    
    for (int y=sy; y<sy+size; y++)
    {
        for (int x=sx; x<sx+size; x++)
        {
            colorVal += alphaImg.getPixels()[y*alphaImg.width*3 + x*3];
        }
    }
    
    return colorVal / (size*size);
}


void ParticleSystem::setFluidForces(vector<ofxMPMForce *> *forces)
{
    fluid.forces = forces;
}

ofxMPMFluid* ParticleSystem::getFluid()
{
    return &fluid;
}

void ParticleSystem::mousePressed(int x, int y, int button)
{
//    lineStart = lineEnd = ofVec2f(x-pos.x, y-pos.y);
//    drawObs = true;
}

void ParticleSystem::mouseDragged(int x, int y, int button)
{
//    lineEnd = ofVec2f(x-pos.x, y-pos.y);
}

void ParticleSystem::mouseReleased(int x, int y, int button)
{
//    ofVec2f line = (lineEnd - lineStart);
//    
//    lineEnd = ofVec2f(x-pos.x, y-pos.y);
//    for (float t=0; t<=1; t+=1/(line.length()/14))
//    {
//        ofVec2f op = (lineStart + line*t) / fluid.scaleFactor;
//        fluid.addObstacle(new ofxMPMObstacle(op.x, op.y, 2));
//    }
//    drawObs = false;
}

