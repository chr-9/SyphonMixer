#pragma once

#include "ofMain.h"
#include "ofxSyphon.h"
#include "ofxDatGui.h"
#include "ofxXmlSettings.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
        void exit();

		void keyPressed(int key);
		void keyReleased(int key);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    void onSliderEvent(ofxDatGuiSliderEvent e);
    void onToggleEvent(ofxDatGuiToggleEvent e);

    void onSliderEventInputA(ofxDatGuiSliderEvent e);
    void onToggleEventInputA(ofxDatGuiToggleEvent e);
    void onSliderEventInputB(ofxDatGuiSliderEvent e);
    void onToggleEventInputB(ofxDatGuiToggleEvent e);
    
    void onScrollViewEventA(ofxDatGuiScrollViewEvent e);
    void onScrollViewEventB(ofxDatGuiScrollViewEvent e);

    void serverAnnounced(ofxSyphonServerDirectoryEventArgs &arg);
    void serverRetired(ofxSyphonServerDirectoryEventArgs &arg);
    
    ofxXmlSettings settings;
    ofxDatGui* gui;
    
    // Fade
    ofParameter<float> fadeAB;
    ofParameter<float> fadeMaster;
    
    // FX
    ofParameter<float> fadeWhite;
    bool strobeBlack;
    bool strobeWhite;
    ofParameter<int> strobeInterval;

    ofxDatGui* gui2;

    // Input A
    string serverNameA;
    string appNameA;
    ofParameter<float> scaleA;
    bool centeringA;
    
    // Input B
    string serverNameB;
    string appNameB;
    ofParameter<float> scaleB;
    bool centeringB;
    
    // Output Resolution
    ofParameter<int> outWidth;
    ofParameter<int> outHeight;
    
    ofxDatGui* sourceAHeader;
    ofxDatGui* sourceBHeader;

    ofxDatGuiScrollView* sourceA;
    ofxDatGuiScrollView* sourceB;
    
    int drawWidth;
    int drawHeight;
    float opacityA;
    float opacityB;
    int blendmode = 0;
    int strobe;
    
    bool drawInfo;
    
    ofFbo mixedFbo;
    ofTexture mixedTex;
    
    ofxSyphonServer mixedSyphonServer;
    ofxSyphonServerDirectory dir;
    ofxSyphonClient mClientA;
    ofxSyphonClient mClientB;
    
    int dirIdxA;
    int dirIdxB;

};
