#pragma once

#include "ofMain.h"
#include "ofxSyphon.h"
#include "ofxDatGui.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    void onSliderEvent(ofxDatGuiSliderEvent e);
    void onToggleEvent(ofxDatGuiToggleEvent e);

    void onSliderEventInputA(ofxDatGuiSliderEvent e);
    void onTextInputEventInputA(ofxDatGuiTextInputEvent e);
    void onToggleEventInputA(ofxDatGuiToggleEvent e);

    void onSliderEventInputB(ofxDatGuiSliderEvent e);
    void onTextInputEventInputB(ofxDatGuiTextInputEvent e);
    void onToggleEventInputB(ofxDatGuiToggleEvent e);

    
    ofxDatGui* gui;
    
    // Fade
    ofParameter<float> fadeAB;
    ofParameter<float> masterFade;
    
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
    
    ofxSyphonClient mClient0;
    ofxSyphonClient mClient1;
};
