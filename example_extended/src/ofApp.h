#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxFlowTools.h"

#define USE_PROGRAMMABLE_GL

using namespace flowTools;

enum visualizationTypes{ INPUT_FOR_DEN = 0, INPUT_FOR_VEL, FLOW_VEL, BRIDGE_VEL, BRIDGE_DEN, BRIDGE_TMP, BRIDGE_PRS, OBSTACLE, FLUID_BUOY, FLUID_VORT, FLUID_DIVE, FLUID_TMP, FLUID_PRS, FLUID_VEL, FLUID_DEN };

const vector<string> visualizationNames({"input for density", "input for velocity", "optical flow", "bridge velocity", "bridge density", "bridge temperature", "bridge pressure", "obstacle", "fluid buoyancy", "fluid vorticity", "fluid divergence", "fluid temperature", "fluid pressure", "fluid velocity", "fluid density"});

class ofApp : public ofBaseApp{
public:
	void	setup();
	void	update();
	void	draw();
	void	keyPressed(int key);
	
	int		densityWidth, densityHeight, flowWidth, flowHeight, windowWidth, windowHeight;
	
	vector< ftFlow* >		flows;
	ftOpticalFlow			opticalFlow;
	ftVelocityBridgeFlow	velocityBridgeFlow;
	ftDensityBridgeFlow		densityBridgeFlow;
	ftFluidFlow				fluidFlow;
	ftMouseFlow				mouseFlow;
	ftParticleFlow			particleFlow;
	
	ofImage					flowToolsLogo;
	
	ofParameterGroup		visualizationParameters;
	ofParameter<int>		visualizationMode;
	ofParameter<string>		visualizationName;
	ofParameter<float>		visualizationScale;
	ofParameter<int>		visualizationSize;
	ofParameter<bool>		toggleVisualizationField;
	void toggleVisualizationFieldListener(bool &_value)	{ for (auto flow : flows) { flow->setDrawField(_value); } }
	void visualizationScaleListener(float& _value)		{ for (auto flow : flows) { flow->setDrawScale(_value); } }
	void visualizationSizeListener(int& _value)			{ for (auto flow : flows) { flow->setFieldSize(_value); } }
	void visualizationModeListener(int& _value) 		{ visualizationName.set(visualizationNames[_value]); }
	
	ofVideoGrabber		simpleCam;
	ofFbo				cameraFbo;
	ofParameter<bool>	toggleCameraDraw;
	
	ofxPanel			gui;
	void				setupGui();
	void				switchGuiColor(bool _switch);
	ofParameter<float>	guiFPS;
	ofParameter<float>	guiMinFPS;
	ofParameter<bool>	toggleGuiDraw;
	void				drawGui();
	deque<float>		deltaTimeDeque;
	float				lastTime;
	
	ofParameter<bool>	toggleFullScreen;
	void				toggleFullScreenListener(bool& _value) { ofSetFullscreen(_value);}
	void 				windowResized(ofResizeEventArgs & _resize){ windowWidth = _resize.width; windowHeight = _resize.height; }
};
