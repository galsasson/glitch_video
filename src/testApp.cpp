#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofSetVerticalSync(true);
    
    setupGui();
    
//    videoPlayer.loadMovie("sunrise_static.mp4");
    videoPlayer.loadMovie("videos/different_pulses.mp4");
    videoPlayer.play();
    setupDim = true;
    
    // blur
    shad_blurX.load("shaders/simpleBlurHorizontal");
    shad_blurY.load("shaders/simpleBlurVertical");
    
    hasContent = false;
    isReshaping = false;
}

//--------------------------------------------------------------
void testApp::update(){
    drawMut.lock();
    if (videoPlayer.isPlaying()) {
        if (setupDim) {
            newDimensions(ofVec2f(videoPlayer.getWidth(), videoPlayer.getHeight()));
            setupDim = false;
        }
        videoPlayer.update();
    }
    drawMut.unlock();
    
    
    updateFluid();
    vector<ofxMPMParticle*> fParticles = fluid.getParticles();

    vector<ofVec3f> verts;
    for (int i=0; i<particles.size(); i++)
    {
        if (isReshaping)
        {
            ofVec3f force = particles[i]->getRestPosition() - (ofVec3f(fParticles[i]->x, fParticles[i]->y, 0) * ofVec3f(scaleFactor, scaleFactor, 0));
            fParticles[i]->u = force.x/25;
            fParticles[i]->v = force.y/25;
        }
        particles[i]->pos.set(fParticles[i]->x*scaleFactor, fParticles[i]->y*scaleFactor, 0);
        particles[i]->update();
        particles[i]->fillVertices(verts, parPhysSize);
    }
    
    vbo.setVertexData(&verts[0], verts.size(), GL_STATIC_DRAW);
}

//--------------------------------------------------------------
void testApp::draw()
{
    ofPushStyle();
    ofClear(0);
    
    if (hasContent)
    {
        initialFbo.begin();
        drawMut.lock();
        if (videoPlayer.isPlaying())
        {
            if (videoPlayer.getTexture()) {
                videoPlayer.getTexture()->bind();
            }
            
            ofClear(0);
//            ofEnableAlphaBlending();
            glDisable(GL_DEPTH_TEST);
            //            ofBackground(0, 0, 0, 10);
            //            ofSetColor(255, 255, 255, 255);
            ofEnableBlendMode(OF_BLENDMODE_ADD);
            ofSetColor(255, 255, 255, parAlpha);

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
        
        ofClear(0);
        initialFbo.draw(0, 0);
        
        shad_blurX.end();
        blurHor.end();
        
//        blurVer.begin();
        shad_blurY.begin();
        shad_blurY.setUniform1f("blurAmnt", blurAmount);
        
        ofClear(0);
        blurHor.draw(0, 0);
        
        shad_blurY.end();
//        blurVer.end();
    
//        ofEnableBlendMode(OF_BLENDMODE_ADD);
//        initialFbo.draw(0, 0);
//        blurVer.draw(0, 0);
    }
    
    
    ofPopStyle();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    switch (key)
    {
        case 'g':
            gui->toggleVisible();
            break;
        case 's':
            gui->saveSettings("GUI/guiSettings.xml");
            break;
        case 'p':
        {
            if (!isReshaping) {
                prevBlurAmount = blurAmount;
                blurAmount = 0;
                isReshaping = true;
            }
            break;
        }
        default:
            break;
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
    switch(key)
    {
        case 'p':
            isReshaping = false;
            blurAmount = prevBlurAmount;
            break;
        default:
            break;
    }
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    for (int i=0; i<particles.size(); i++)
    {
//        if (ofRandom(1) < 0.5) {
            particles[i]->applyForce(ofVec3f(ofRandom(-15, 15), ofRandom(-15, 15), ofRandom(-1, 1)));
            //            ofVec3f f = ofVec3f(particles[i]->pos.x - ofGetMouseX(),
            //                            particles[i]->pos.y - ofGetMouseY(),
            //                            0);
            //            particles[i]->applyForce(f/10000);
//        }
    }
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}

void testApp::exit()
{
    drawMut.lock();
    
    delete gui;
//    videoPlayer.stop();
//    videoPlayer.close();
    
    drawMut.unlock();
}

void testApp::newDimensions(ofVec2f newDim)
{
    videoDim = newDim;
    cout<<"dimensions: "<<videoDim<<endl;
    
    blurHor.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
    blurVer.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
    initialFbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
    initVideoParticles();
    
    hasContent = true;
}

void testApp::initVideoParticles()
{
    // create particles and geometry vbo
    vector<ofVec3f> verts;
    vector<ofIndexType> indices;
    vector<ofVec2f> uvs;
    vector<ofFloatColor> colors;
    for (int y=0; y<videoDim.y; y+=parChunk)
    {
        for (int x=0; x<videoDim.x; x+=parChunk)
        {
            VideoParticle *vp = new VideoParticle(particles.size(), x+parPhysSize/2, y+parPhysSize/2, 0, parChunk);
            vp->fillVertices(verts, parPhysSize);
            vp->fillIndices(indices);
            vp->fillUvs(uvs, ofVec2f(x, y));
            vp->fillColors(colors, ofFloatColor(1, 1, 1));
            particles.push_back(vp);
        }
    }
    
    vbo.setVertexData(&verts[0], verts.size(), GL_STATIC_DRAW);
    vbo.setTexCoordData(&uvs[0], uvs.size(), GL_STATIC_DRAW);
    vbo.setColorData(&colors[0], colors.size(), GL_STATIC_DRAW);
    vbo.setIndexData(&indices[0], indices.size(), GL_STATIC_DRAW);
    
    cout<<"Initializing fluid: number of particles ("<<particles.size()<<")\n";
    fluid.setup(particles.size());
    fluid.scaleFactor = scaleFactor = (float)ofGetWidth() / fluid.getGridSizeX();
    fluid.numParticles = particles.size();
}

void testApp::setupGui()
{
    gui = new ofxUICanvas(0, 0, 200, ofGetHeight());
    
    gui->addWidgetDown(new ofxUILabel("Video Glitch", OFX_UI_FONT_MEDIUM));
    
    parChunk = 10;
    gui->addIntSlider("Particle Chunk", 1, 50, &parChunk);
    parPhysSize = 10;
    gui->addSlider("Particle Size", 0, 50, &parPhysSize);
    blurAmount = 0;
    gui->addSlider("Blur Amount", 0, 10, &blurAmount);
    parAlpha = 255;
    gui->addSlider("Particle Alpha", 0, 255, &parAlpha);
    
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

    gui->loadSettings("GUI/guiSettings.xml");
}

void testApp::updateFluid()
{
    fluid.densitySetting = density;
    fluid.stiffness = stiffness;
    fluid.bulkViscosity = bulkViscosity;
    fluid.viscosity = viscosity;
    fluid.elasticity = elasticity;
    fluid.gravity = gravity/10;
    
    fluid.scaleFactor = scaleFactor = (float)ofGetWidth() / fluid.getGridSizeX();

    fluid.update();
}

