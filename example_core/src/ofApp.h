#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxFlowTools.h"
#include "ftCore.h"
#include "ftArea.h"

#define USE_PROGRAMMABLE_GL

using namespace flowTools;

class ofApp : public ofBaseApp{
public:
	void	setup();
	void	update();
	void	draw();
	
	int		densityWidth;
	int		densityHeight;
	int		flowWidth;
	int		flowHeight;
	int		fieldWidth;
	int		fieldHeight;
	
	// FLOWTOOLS
	vector< ftFlow* >	flows;
	ftOpticalFlow		opticalFlow;
	ftVelocityBridge	velocityBridge;
	ftDensityBridge		densityBridge;
	ftFluidSimulation	fluidSimulation;
	ofImage				flowToolsLogo;

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
	
	ofParameterGroup	visualizeParameters;
	ofParameter<int>	drawMode;
	ofParameter<string> drawName;
	ofParameter<bool>	showField;
	ofParameter<float>	visualizeScale;
	void showFieldListener(bool &_value)		{ for (auto flow : flows) { flow->setDrawField(_value); } }
	void visualizeScaleListener(float& _value)	{ for (auto flow : flows) { flow->setDrawScale(_value); } }
	void drawModeListener(int& _value) 			{ drawName.set(ftFlowNames[_value]); }
	
	// Camera
	ofVideoGrabber		simpleCam;
	ofFbo				cameraFbo;
	ofParameter<bool>	doFlipCamera;
	ofParameter<bool>	doDrawCamera;
	
	// Time
	float				lastTime;
	float				deltaTime;
};
