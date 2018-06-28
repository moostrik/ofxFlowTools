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
	void	keyPressed(int key);
	
	int		densityWidth, densityHeight;
	int		flowWidth, flowHeight;
	int		fieldWidth, fieldHeight;
	int		windowWidth, windowHeight;
	
	vector< ftFlow* >	flows;
	ftOpticalFlow		opticalFlow;
	ftVelocityBridge	velocityBridge;
	ftDensityBridge		densityBridge;
	ftFluidSimulation	fluidSimulation;
	ofImage				flowToolsLogo;
	
	ofVideoGrabber		simpleCam;
	ofFbo				cameraFbo;
	ofParameter<bool>	toggleCameraDraw;

	ofxPanel			gui;
	void				setupGui();
	void				switchGuiColor(bool _switch);
	
	ofParameter<float>	guiFPS;
	ofParameter<float>	guiMinFPS;
	deque<float>		deltaTimeDeque;
	
	ofParameter<bool>	toggleGuiDraw;
	void				drawGui();
	
	ofParameter<bool>	toggleFullScreen;
	void				toggleFullScreenListener(bool& _value) { ofSetFullscreen(_value);}
	void 				windowResized(ofResizeEventArgs & _resize){ windowWidth = _resize.width; windowHeight = _resize.height; }
	
	ofParameterGroup	visualizationParameters;
	ofParameter<int>	visualizationMode;
	ofParameter<string> visualizationName;
	ofParameter<float>	visualizationScale;
	ofParameter<bool>	toggleVisualizationField;
	void toggleVisualizationFieldListener(bool &_value)	{ for (auto flow : flows) { flow->setDrawField(_value); } }
	void visualizationScaleListener(float& _value)		{ for (auto flow : flows) { flow->setDrawScale(_value); } }
	void visualizationModeListener(int& _value) 		{ visualizationName.set(ftFlowNames[_value]); }
	
	// Time
	float				lastTime;
	float				deltaTime;
};
