#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetWindowTitle("SyphonMixer");
    ofSetFrameRate(60);
    ofBackground(0, 0, 0, 0);
    
    drawWidth = ofGetWidth() / 2;
    drawHeight = ofGetHeight() / 2;
    
    opacityA = 0.5f;
    opacityB = 0.5f;
    fadeAB = 0.5f;
    fadeMaster = 1.0f;
    fadeWhite = 0.0f;
    
    settings.loadFile("settings.xml");
    outWidth = settings.getValue("settings:outWidth", 1280);
    outHeight = settings.getValue("settings:outHeight", 720);
    strobeInterval = settings.getValue("settings:strobeInterval", 3);
    drawInfo = settings.getValue("settings:drawInfo", true);
    scaleA = settings.getValue("settings:scaleA", 1.0f);
    scaleB = settings.getValue("settings:scaleB", 1.0f);
    centeringA = settings.getValue("settings:centeringA", false);
    centeringB = settings.getValue("settings:centeringB", false);

    
    //Setup Syphon
    mixedSyphonServer.setName("Mixed Output");
    dir.setup();
    
    dirIdxA = -1;
    dirIdxB = -1;

    mClientA.setup();
    if(dir.isValidIndex(dirIdxA))
        mClientA.set(dir.getDescription(dirIdxA));

    mClientB.setup();
    if(dir.isValidIndex(dirIdxB))
        mClientB.set(dir.getDescription(dirIdxB));
    
    ofAddListener(dir.events.serverAnnounced, this, &ofApp::serverAnnounced);
    ofAddListener(dir.events.serverRetired, this, &ofApp::serverRetired);
    
    mixedFbo.allocate(outWidth, outHeight, GL_RGB);
    mixedTex.allocate(outWidth, outHeight, GL_RGB);
    
    
    //Setup GUI
    gui = new ofxDatGui();
    gui->setPosition(drawWidth + 1, drawHeight + 1);
    gui2 = new ofxDatGui();
    gui2->setPosition(drawWidth + 221, drawHeight+ 1);
    
    auto theme = new ofxDatGuiTheme();
    theme->init();
    //theme->layout.upperCaseLabels = false;
    theme->layout.textInput.forceUpperCase = false;
    theme->layout.width = 220;
    theme->layout.height = 20;

    gui->setTheme(theme);
    gui2->setTheme(theme);

    gui->addFRM();
    
    //Fade
    fadeAB.set("Fade A/B", fadeAB, 0.0f, 1.0f);
    gui->addSlider(fadeAB);
    fadeMaster.set("Master Fade", fadeMaster, 0.0f, 1.0f);
    gui->addSlider(fadeMaster);
    
    //FX
    fadeWhite.set("White Fade", fadeWhite, 0.0f, 1.0f);
    gui->addSlider(fadeWhite);
    gui->addToggle("Black Strobe", false);
    gui->addToggle("White Strobe", false);
    strobeInterval.set("Strobe Interval", strobeInterval, 1, 6);
    gui->addSlider(strobeInterval);
    
    //Blend mode
    vector<string> options = {"Blend Alpha", "Blend Add", "Blend Screen", "Blend Multiply", "Blend Subtract"};
    gui->addDropdown("Blend Mode[WIP]", options);
    
    // GUI2
    // Input A
    ofxDatGuiFolder* inputA = gui2->addFolder("Input A", ofColor::white);
    scaleA.set("Scale", scaleA, 0.0f, 4.0f);
    inputA->addSlider(scaleA);
    inputA->addToggle("Centering", centeringA);
    inputA->expand();
    
    inputA->onToggleEvent(this, &ofApp::onToggleEventInputA);
    inputA->onSliderEvent(this, &ofApp::onSliderEventInputA);

    // Input B
    ofxDatGuiFolder* inputB = gui2->addFolder("Input B", ofColor::red);
    scaleB.set("Scale", scaleB, 0.0f, 4.0f);
    inputB->addSlider(scaleB);
    inputB->addToggle("Centering", centeringB);
    inputB->expand();
    
    inputB->onToggleEvent(this, &ofApp::onToggleEventInputB);
    inputB->onSliderEvent(this, &ofApp::onSliderEventInputB);
    
    //Output Resolution
    ofxDatGuiFolder* resCtrl = gui2->addFolder("Output Resolution", ofColor::yellow);
    outWidth.set("Width", outWidth, 1, 1920);
    resCtrl->addSlider(outWidth);
    outHeight.set("Height", outHeight, 1, 1920);
    resCtrl->addSlider(outHeight);
    resCtrl->expand();
    gui2->addToggle("Draw Info", drawInfo);

    
    // Source Selection
    sourceAHeader = new ofxDatGui();
    sourceAHeader->setPosition(drawWidth + 441, drawHeight + 1);
    sourceAHeader->addLabel("Source A");
    sourceAHeader->setTheme(theme);
    
    sourceBHeader = new ofxDatGui();
    sourceBHeader->setPosition(drawWidth + 441, drawHeight + 125);
    sourceBHeader->addLabel("Source B");
    sourceBHeader->setTheme(theme);
    
    sourceA = new ofxDatGuiScrollView("Source A", 4);
    sourceA->setWidth(220);
    sourceA->setPosition(drawWidth + 441, drawHeight + 20);
    
    sourceB = new ofxDatGuiScrollView("Source B", 4);
    sourceB->setWidth(220);
    sourceB->setPosition(drawWidth + 441, drawHeight + 145);
    
    for(int i = 0; i != dir.getServerList().size(); ++i) {
        ofxSyphonServerDescription description = dir.getDescription(i);
        sourceA->add(description.appName + " - " + description.serverName);
        sourceB->add(description.appName + " - " + description.serverName);
    }
    
    sourceA->onScrollViewEvent(this, &ofApp::onScrollViewEventA);
    sourceB->onScrollViewEvent(this, &ofApp::onScrollViewEventB);

    
    
    gui->onSliderEvent(this, &ofApp::onSliderEvent);
    gui->onToggleEvent(this, &ofApp::onToggleEvent);
    
    gui2->onSliderEvent(this, &ofApp::onSliderEvent);
    gui2->onToggleEvent(this, &ofApp::onToggleEvent);

}

//--------------------------------------------------------------
void ofApp::update(){
    mixedFbo.begin();
        ofBackground(0, 0, 0);

        ofEnableAlphaBlending();
        ofEnableBlendMode(OF_BLENDMODE_ADD);

        ofSetColor(255,255,255,255*opacityA);
        if(centeringA){
            int posX = (outWidth / 2) - ((mClientA.getWidth() / 2) + ((mClientA.getWidth() / 2) * (scaleA - 1.0)));
            int posY = (outHeight / 2) - (mClientA.getHeight() / 2) - ((mClientA.getHeight() / 2) * (scaleA - 1.0));
            mClientA.draw(posX, posY, mClientA.getWidth() * scaleA, mClientA.getHeight() * scaleA);
        }else{
            mClientA.draw(0, 0, mClientA.getWidth() * scaleA, mClientA.getHeight() * scaleA);
        }
    
        ofSetColor(255,255,255,255*opacityB);
        if(centeringB){
            int posX = (outWidth / 2) - ((mClientB.getWidth() / 2) + ((mClientB.getWidth() / 2) * (scaleB - 1.0)));
            int posY = (outHeight / 2) - (mClientB.getHeight() / 2) - ((mClientB.getHeight() / 2) * (scaleB - 1.0));
            mClientB.draw(posX, posY, mClientB.getWidth() * scaleB, mClientB.getHeight() * scaleB);
        }else{
            mClientB.draw(0, 0, mClientB.getWidth() * scaleB, mClientB.getHeight() * scaleB);
        }

        if(strobeBlack){
            if(strobe > strobeInterval){
                ofEnableBlendMode(OF_BLENDMODE_ALPHA);
                ofSetColor(0,0,0,255);
                ofDrawRectangle(0, 0, outWidth, outHeight);
                strobe = 0;
            }
            strobe++;
        }
    
        if(strobeWhite){
            if(strobe > strobeInterval){
                ofEnableBlendMode(OF_BLENDMODE_ALPHA);
                ofSetColor(255,255,255,255);
                ofDrawRectangle(0, 0, outWidth, outHeight);
                strobe = 0;
            }
            strobe++;
        }
    
        if(fadeWhite > 0.0f){
            ofEnableBlendMode(OF_BLENDMODE_ADD);
            ofSetColor(255,255,255,255*fadeWhite);
            ofDrawRectangle(0, 0, outWidth, outHeight);
        }
    
        if(fadeMaster < 1.0f){
            ofEnableBlendMode(OF_BLENDMODE_ALPHA);
            ofSetColor(0,0,0,255*(1.0f - fadeMaster));
            ofDrawRectangle(0, 0, outWidth, outHeight);
        }
    
        ofDisableAlphaBlending();
    
    mixedFbo.end();
    
    //Send Syphon
    mixedTex = mixedFbo.getTexture();
    mixedSyphonServer.publishTexture(&mixedTex);
    
    sourceA->update();
    sourceB->update();

}

//--------------------------------------------------------------
void ofApp::draw(){
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    ofSetColor(255);

    //Draw Preview1
    mClientA.draw(0, 0, drawWidth, drawHeight);
    
    //Draw Preview2
    mClientB.draw(drawWidth + 1, 0, drawWidth, drawHeight);
    
    //Draw mixed
    mixedFbo.draw(0, drawHeight + 1, drawWidth, drawHeight);
    
    //Draw Info
    ofSetColor(255);
    ofDrawLine(drawWidth, 0, drawWidth, drawHeight*2);
    ofDrawLine(0, drawHeight, drawWidth*2, drawHeight);
    
    if(drawInfo){
        string infoA = "Input A (" + appNameA + " - " + serverNameA + ", " + ofToString(mClientA.getWidth()) + "x" + ofToString(mClientA.getHeight()) + ")";
        string infoB = "Input B (" + appNameB + " - " + serverNameB + ", " + ofToString(mClientB.getWidth()) + "x" + ofToString(mClientB.getHeight()) + ")";

        string blendmode_str = "add";   // WIP
        string infoOut = "Output (" + ofToString(outWidth) + "x" + ofToString(outHeight) + ", " + blendmode_str + ")";
    
        ofSetColor(0,0,0,255);
        ofDrawBitmapString(infoA, 1, 11);
        ofDrawBitmapString(infoB, drawWidth + 2, 11);
        ofDrawBitmapString(infoOut, 1, drawHeight + 12);
    
        ofSetColor(255);
        ofDrawBitmapString(infoA, 0, 10);
        ofDrawBitmapString(infoB, drawWidth + 1, 10);
        ofDrawBitmapString(infoOut, 0, drawHeight + 11);
        
        sourceA->draw();
        sourceB->draw();

    }
}

void ofApp::onSliderEvent(ofxDatGuiSliderEvent e)
{
    //cout << e.target->getName() << " : " << e.value << endl;
    
    if(e.target->getName() == "Fade A/B"){
        float value = e.value;
        if(blendmode == 0){
            opacityA = 1.0f - value;
            opacityB = 0.0f + value;
        }
    }
    
    if(e.target->getName() == "Width" || e.target->getName() == "Height"){
        mixedFbo.allocate(outWidth, outHeight, GL_RGBA);
        mixedTex.allocate(outWidth, outHeight, GL_RGBA);
    }
}

void ofApp::onToggleEvent(ofxDatGuiToggleEvent e){
    if(e.target->getName() == "Black Strobe"){
        strobeBlack = e.checked;
    }
    
    if(e.target->getName() == "White Strobe"){
        strobeWhite = e.checked;
    }
    
    if(e.target->getName() == "Draw Info"){
        drawInfo = e.checked;
    }
}

void ofApp::onScrollViewEventA(ofxDatGuiScrollViewEvent e)
{
    ofxSyphonServerDescription description = dir.getDescription(e.index);
    if(dir.isValidIndex(e.index))
        mClientA.set(description);
    
    serverNameA = description.serverName;
    appNameA = description.appName;
}

void ofApp::onScrollViewEventB(ofxDatGuiScrollViewEvent e)
{
    ofxSyphonServerDescription description = dir.getDescription(e.index);
    if(dir.isValidIndex(e.index))
        mClientB.set(description);
    
    serverNameB = description.serverName;
    appNameB = description.appName;
}

void ofApp::onSliderEventInputA(ofxDatGuiSliderEvent e)
{
    scaleA = e.value;
}

void ofApp::onSliderEventInputB(ofxDatGuiSliderEvent e)
{
    scaleB = e.value;
}

void ofApp::onToggleEventInputA(ofxDatGuiToggleEvent e){
    centeringA = e.checked;
}

void ofApp::onToggleEventInputB(ofxDatGuiToggleEvent e){
    centeringB = e.checked;
}

void ofApp::serverAnnounced(ofxSyphonServerDirectoryEventArgs &arg)
{
    sourceA->clear();
    sourceB->clear();
    for(int i = 0; i != dir.getServerList().size(); ++i) {
        ofxSyphonServerDescription description = dir.getDescription(i);
        sourceA->add(description.appName + " - " + description.serverName);
        sourceB->add(description.appName + " - " + description.serverName);
    }
    dirIdxA = 0;
    dirIdxB = 0;
}

void ofApp::serverRetired(ofxSyphonServerDirectoryEventArgs &arg)
{
    sourceA->clear();
    sourceB->clear();
    for(int i = 0; i != dir.getServerList().size(); ++i) {
        ofxSyphonServerDescription description = dir.getDescription(i);
        sourceA->add(description.appName + " - " + description.serverName);
        sourceB->add(description.appName + " - " + description.serverName);
    }
    dirIdxA = 0;
    dirIdxB = 0;
}
//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    drawWidth = w/2;
    drawHeight = h/2;
    gui->setPosition(drawWidth+1, drawHeight+1);
    gui2->setPosition(drawWidth + 221, drawHeight+ 1);
    
    sourceAHeader->setPosition(drawWidth + 441, drawHeight + 1);
    sourceBHeader->setPosition(drawWidth + 441, drawHeight + 125);
    sourceA->setPosition(drawWidth + 441, drawHeight + 20);
    sourceB->setPosition(drawWidth + 441, drawHeight + 145);
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

void ofApp::exit()
{
    settings.setValue("settings:outWidth", outWidth);
    settings.setValue("settings:outHeight", outHeight);
    settings.setValue("settings:strobeInterval", strobeInterval);
    settings.setValue("settings:drawInfo", drawInfo);
    settings.setValue("settings:scaleA", scaleA);
    settings.setValue("settings:scaleB", scaleB);
    settings.setValue("settings:centeringA", centeringA);
    settings.setValue("settings:centeringB", centeringB);
    settings.saveFile("settings.xml");
}
