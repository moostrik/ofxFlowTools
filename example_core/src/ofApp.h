#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxFlowTools.h"
#include "ftCore.h"
#include "ftArea.h"

//#define USE_PROGRAMMABLE_GL

using namespace flowTools;

class ofApp : public ofBaseApp{
public:
	void	setup();
	void	update();
	void	draw();
	
	ftCore				flowCore;
	ofImage				flowToolsLogoImage;

	// GUI
	ofxPanel			gui;
	void				setupGui();
	void				switchGuiColor(bool _switch);
	void				keyPressed(int key);
	void				drawGui();
	ofParameter<bool>	toggleGuiDraw;
	ofParameter<float>	guiFPS;
	ofParameter<float>	guiMinFPS;
	deque<float>		deltaTimeDeque;
	ofParameter<bool>	doFullScreen;
	void				setFullScreen(bool& _value) { ofSetFullscreen(_value);}
	int					windowWidth, windowHeight;
	void windowResized(ofResizeEventArgs & _resize){ windowWidth = _resize.width; windowHeight = _resize.height; }
	
	// Camera
	int 				camWidth, camHeight;
	ofVideoGrabber		simpleCam;
	bool				didCamUpdate;
	ofFbo				cameraFbo;
	ofParameter<bool>	doFlipCamera;
	ofParameter<bool>	doDrawCamera;
	
	// Time
	float				lastTime;
	float				deltaTime;
};
