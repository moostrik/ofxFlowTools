#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxFlowTools.h"

#define USE_PROGRAMMABLE_GL					// Maybe there is a reason you would want to
//#define USE_FASTER_INTERNAL_FORMATS			// about 15% faster but gives errors from ofGLUtils

using namespace flowTools;

class flowToolsApp : public ofBaseApp{
public:
	void setup();
	void update();
	void draw();
	
	// Interaction
	void				keyPressed(int key);
	void				mouseMoved(int x, int y);
	void				mouseDragged(int x, int y, int button);
	ofVec2f				lastMouse;
	
	// GUI
	ofxPanel			gui;
	void				setupGui();
	ofParameter<float>	guiFPS;
	ofParameter<bool>	doFullScreen;
	void				setFullScreen(bool& _value) { ofSetFullscreen(_value);}
	ofParameter<bool>	toggleGuiDraw;
	ofParameter<bool>	doFlipCamera;
	ofParameter<int>	visualisationMode;
	ofParameter<string> visualisationName;
	int					numVisualisationModes;
	string				*visualisationModeTitles;
	ofParameterGroup	visualisationParameters;
	
	ofParameterGroup	drawForceParameters;
	ofParameter<bool>	doResetDrawForces;
	void				resetDrawForces(bool& _value) { if (_value) {for (int i=0; i<numDrawForces; i++) flexDrawForces[i].reset();} doResetDrawForces.set(false);}
	ofParameterGroup	leftButtonParameters;
	ofParameterGroup	rightButtonParameters;
	ofParameter<bool>	showScalar;
	ofParameter<bool>	showField;
	ofParameter<float>	displayScalarScale;
	void				setDisplayScalarScale(float& _value) { displayScalar.setScale(_value); }
	ofParameter<float>	velocityFieldArrowScale;
	void				setVelocityFieldArrowScale(float& _value) { velocityField.setVectorSize(_value); }
	ofParameter<float>	temperatureFieldBarScale;
	void				setTemperatureFieldBarScale(float& _value) { temperatureField.setVectorSize(_value); }
	ofParameter<bool>	visualisationLineSmooth;
	void				setVisualisationLineSmooth(bool& _value) { velocityField.setLineSmooth(_value); }
	
	
	// Camera
	ofVideoGrabber		simpleCam;
	bool				didCamUpdate;
	ofFbo				cameraFbo;
	
	// Time
	float				lastTime;
	float				deltaTime;
	
	// FlowTools
	int					flowWidth;
	int					flowHeight;
	int					drawWidth;
	int					drawHeight;
	
	ftOpticalFlow		opticalFlow;
	ftVelocityMask		velocityMask;
	ftFluidSimulation	fluid;
	ftParticleFlow		particleFlow;
	
	ftDisplayScalar		displayScalar;
	ftVelocityField		velocityField;
	ftTemperatureField	temperatureField;
	
	int					numDrawForces;
	ftDrawForce*		flexDrawForces;
	
	ofImage				flowToolsLogoImage;
	bool				showLogo;
	
};
