#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetWindowTitle("SyphonMixer");
    ofSetFrameRate(60);

    outWidth = 1280;
    outHeight = 720;
    
    drawWidth = ofGetWidth() / 2;
    drawHeight = ofGetHeight() / 2;
    
    opacityA = 0.5f;
    opacityB = 0.5f;
    fadeAB = 0.5f;
    masterFade = 1.0f;
    
    fadeWhite = 0.0f;
    strobeInterval = 3;
    
    drawInfo = true;
    
    serverNameA = "Layer 1";
    appNameA = "VDMX5";
    scaleA = 1.0f;
    centeringA = false;
    
    serverNameB = "Layer 2";
    appNameB = "VDMX5";
    scaleB = 1.0f;
    centeringB = false;


    //Setup Syphon
    mixedSyphonServer.setName("Mixed Output");
    
    mClient0.setup();
    //mClient0.set("","Simple Server");
    mClient0.set(serverNameA,appNameA);

    mClient1.setup();
    //mClient1.set("", "Simple Server");
    mClient1.set(serverNameB,appNameB);
    
    mixedFbo.allocate(outWidth, outHeight, GL_RGBA);
    mixedTex.allocate(outWidth, outHeight, GL_RGBA);
    
    
    //Setup GUI
    gui = new ofxDatGui();
    gui->setPosition(drawWidth + 1, drawHeight + 1);
    
    //gui->addHeader("SyphonMixer Control");
    gui->addFRM();
    gui->addBreak()->setHeight(10.0f);
    
    //Fade
    fadeAB.set("Fade A/B", fadeAB, 0.0f, 1.0f);
    gui->addSlider(fadeAB);
    masterFade.set("Master Fade", masterFade, 0.0f, 1.0f);
    gui->addSlider(masterFade);
    
    //FX
    fadeWhite.set("White Fade", fadeWhite, 0.0f, 1.0f);
    gui->addSlider(fadeWhite);
    gui->addToggle("Black Strobe", false);
    gui->addToggle("White Strobe", false);
    strobeInterval.set("Strobe Interval", strobeInterval, 1, 6);
    gui->addSlider(strobeInterval);
    gui->addBreak()->setHeight(10.0f);
    
    //Blend mode
    vector<string> options = {"Blend Alpha", "Blend Add", "Blend Screen", "Blend Multiply", "Blend Subtract"};
    gui->addDropdown("Blend Mode[WIP]", options);
    
    gui2 = new ofxDatGui();
    gui2->setPosition(drawWidth + 276, drawHeight+ 1);
    
    // Input A
    ofxDatGuiFolder* inputA = gui2->addFolder("Input A", ofColor::white);
    inputA->addTextInput("App Name", appNameA);
    inputA->addTextInput("Server Name", serverNameA);
    scaleA.set("Scale[WIP]", scaleA, 0.0f, 4.0f);
    inputA->addSlider(scaleA);
    inputA->addToggle("Centering[WIP]", false);
    //inputA->expand();
    
    inputA->onTextInputEvent(this, &ofApp::onTextInputEventInputA);
    inputA->onToggleEvent(this, &ofApp::onToggleEventInputA);
    inputA->onSliderEvent(this, &ofApp::onSliderEventInputA);

    // Input B
    ofxDatGuiFolder* inputB = gui2->addFolder("Input B", ofColor::red);
    inputB->addTextInput("App Name", appNameB);
    inputB->addTextInput("Server Name", serverNameB);
    scaleB.set("Scale[WIP]", scaleB, 0.0f, 4.0f);
    inputB->addSlider(scaleB);
    inputB->addToggle("Centering[WIP]", false);
    //inputB->expand();
    
    inputB->onTextInputEvent(this, &ofApp::onTextInputEventInputB);
    inputB->onToggleEvent(this, &ofApp::onToggleEventInputB);
    inputB->onSliderEvent(this, &ofApp::onSliderEventInputB);
    
    
    //Output Resolution
    ofxDatGuiFolder* resCtrl = gui2->addFolder("Output Resolution", ofColor::yellow);
    outWidth.set("Width", outWidth, 1, 1920);
    resCtrl->addSlider(outWidth);
    outHeight.set("Height", outHeight, 1, 1920);
    resCtrl->addSlider(outHeight);
    resCtrl->expand();
    
    gui2->addToggle("Draw Info", true);

    gui->onSliderEvent(this, &ofApp::onSliderEvent);
    gui2->onSliderEvent(this, &ofApp::onSliderEvent);
    gui->onToggleEvent(this, &ofApp::onToggleEvent);
    gui2->onToggleEvent(this, &ofApp::onToggleEvent);




}

//--------------------------------------------------------------
void ofApp::update(){
    mixedFbo.begin();
        ofBackground(0, 0, 0);
    
        ofEnableAlphaBlending();
        ofEnableBlendMode(OF_BLENDMODE_ADD);

        ofSetColor(255,255,255,255*opacityA);
        mClient0.draw(0, 0);

        ofSetColor(255,255,255,255*opacityB);
        mClient1.draw(0, 0);
    
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
    
        if(masterFade < 1.0f){
            ofEnableBlendMode(OF_BLENDMODE_ALPHA);
            ofSetColor(0,0,0,255*(1.0f - masterFade));
            ofDrawRectangle(0, 0, outWidth, outHeight);
        }
    
        ofDisableAlphaBlending();
    
    mixedFbo.end();
    
    //Send Syphon
    mixedTex = mixedFbo.getTexture();
    mixedSyphonServer.publishTexture(&mixedTex);
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    ofSetColor(255);

    //Draw Preview1
    mClient0.draw(0, 0, drawWidth, drawHeight);
    
    //Draw Preview2
    mClient1.draw(drawWidth + 1, 0, drawWidth, drawHeight);
    
    //Draw mixed
    mixedFbo.draw(0, drawHeight + 1, drawWidth, drawHeight);
    
    //Draw Info
    ofSetColor(255);
    ofDrawLine(drawWidth, 0, drawWidth, drawHeight*2);
    ofDrawLine(0, drawHeight, drawWidth*2, drawHeight);
    
    if(drawInfo){
        string infoA = "Input A (" + mClient0.getApplicationName() + " - " + mClient0.getServerName() + ", " + ofToString(mClient0.getWidth()) + "x" + ofToString(mClient0.getHeight()) + ")";
        string infoB = "Input B (" + mClient1.getApplicationName() + " - " + mClient1.getServerName() + ", " + ofToString(mClient1.getWidth()) + "x" + ofToString(mClient1.getHeight()) + ")";

        string blendmode_str = "add";   // WIP
        string infoOut = "Output (SyphonMixerDebug - " + mixedSyphonServer.getName() + ", " +ofToString(outWidth) + "x" + ofToString(outHeight) + ", " + blendmode_str + ")";
    
        ofSetColor(0,0,0,255);
        ofDrawBitmapString(infoA, 1, 11);
        ofDrawBitmapString(infoB, drawWidth + 2, 11);
        ofDrawBitmapString(infoOut, 1, drawHeight + 12);
    
        ofSetColor(255);
        ofDrawBitmapString(infoA, 0, 10);
        ofDrawBitmapString(infoB, drawWidth + 1, 10);
        ofDrawBitmapString(infoOut, 0, drawHeight + 11);
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

void ofApp::onTextInputEventInputA(ofxDatGuiTextInputEvent e){
    if(e.target->getName() == "Server Name"){
        serverNameA = e.text;
    }
    
    if(e.target->getName() == "App Name"){
        appNameA = e.text;
    }
    mClient0.set(serverNameA,appNameA);

}

void ofApp::onTextInputEventInputB(ofxDatGuiTextInputEvent e){
    if(e.target->getName() == "Server Name"){
        serverNameB = e.text;
    }
    
    if(e.target->getName() == "App Name"){
        appNameB = e.text;
    }
    mClient1.set(serverNameB,appNameB);

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


//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    drawWidth = w/2;
    drawHeight = h/2;
    gui->setPosition(drawWidth+1, drawHeight+1);
    gui2->setPosition(drawWidth + 271, drawHeight+ 1);
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
