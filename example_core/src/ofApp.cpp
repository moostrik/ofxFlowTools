#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	
//	ofSetVerticalSync(false);
	ofSetLogLevel(OF_LOG_NOTICE);
	
	densityWidth = 1280;
	densityHeight = 720;
	// process all but the density on 16th resolution
	flowWidth = densityWidth / 4;
	flowHeight = densityHeight / 4;
	windowWidth = ofGetWindowWidth();
	windowHeight = ofGetWindowHeight();
	
	opticalFlow.setup(flowWidth, flowHeight);
	velocityBridge.setup(flowWidth, flowHeight);
	densityBridge.setup(flowWidth, flowHeight, densityWidth, densityHeight);
	fluidSimulation.setup(flowWidth, flowHeight, densityWidth, densityHeight);
	
	flows.push_back(&opticalFlow);
	flows.push_back(&velocityBridge);
	flows.push_back(&densityBridge);
	flows.push_back(&fluidSimulation);
	
	flowToolsLogo.load("flowtools.png");
	fluidSimulation.addObstacle(flowToolsLogo.getTexture());
	
	simpleCam.setup(densityWidth, densityHeight, true);
	cameraFbo.allocate(densityWidth, densityHeight);
	ftUtil::zero(cameraFbo);
	
	setupGui();
}

//--------------------------------------------------------------
void ofApp::setupGui() {
	
	gui.setup("settings");
	gui.setDefaultBackgroundColor(ofColor(0, 0, 0, 127));
	gui.setDefaultFillColor(ofColor(160, 160, 160, 160));
	gui.add(guiFPS.set("average FPS", 0, 0, 60));
	gui.add(guiMinFPS.set("minimum FPS", 0, 0, 60));
	gui.add(toggleFullScreen.set("fullscreen (F)", false));
	toggleFullScreen.addListener(this, &ofApp::toggleFullScreenListener);
	gui.add(toggleGuiDraw.set("show gui (G)", false));
	gui.add(toggleCameraDraw.set("draw camera (C)", true));
	
	visualizationParameters.setName("visualization");
	visualizationParameters.add(visualizationMode.set("mode", FT_FLUID_DENSITY, FT_NONE, FT_FLUID_DENSITY));
	visualizationParameters.add(visualizationName.set("MODE", "Fluid Density"));
	visualizationParameters.add(toggleVisualizationField.set("show field", false));
	visualizationParameters.add(visualizationScale.set("scale", 0.3, 0.1, 3.0));
	visualizationParameters.add(visualizationSize.set("size", flowWidth / 2, flowWidth / 4, flowWidth));
	visualizationMode.addListener(this, &ofApp::visualizationModeListener);
	toggleVisualizationField.addListener(this, &ofApp::toggleVisualizationFieldListener);
	visualizationScale.addListener(this, &ofApp::visualizationScaleListener);
	visualizationSize.addListener(this, &ofApp::visualizationSizeListener);
	
	bool s = true;
	switchGuiColor(s = !s);
	gui.add(visualizationParameters);
	switchGuiColor(s = !s);
	gui.add(opticalFlow.getParameters());
	switchGuiColor(s = !s);
	gui.add(velocityBridge.getParameters());
	switchGuiColor(s = !s);
	gui.add(densityBridge.getParameters());
	switchGuiColor(s = !s);
	gui.add(fluidSimulation.getParameters());

	// if the settings file is not present the parameters will not be set during this setup
	if (!ofFile("settings.xml"))
		gui.saveToFile("settings.xml");
	
	gui.loadFromFile("settings.xml");
	
	gui.minimizeAll();
	toggleGuiDraw = true;
}


void ofApp::switchGuiColor(bool _switch) {
	ofColor guiHeaderColor[2];
	guiHeaderColor[0].set(160, 160, 80, 200);
	guiHeaderColor[1].set(80, 160, 160, 200);
	ofColor guiFillColor[2];
	guiFillColor[0].set(160, 160, 80, 200);
	guiFillColor[1].set(80, 160, 160, 200);
	
	gui.setDefaultHeaderBackgroundColor(guiHeaderColor[_switch]);
	gui.setDefaultFillColor(guiFillColor[_switch]);
}

//--------------------------------------------------------------
void ofApp::update(){
	float dt = 1.0 / max(ofGetFrameRate(), 1.f); // more smooth as 'real' deltaTime.
	
	simpleCam.update();
	if (simpleCam.isFrameNew()) {
		cameraFbo.begin();
		simpleCam.draw(cameraFbo.getWidth(), 0, -cameraFbo.getWidth(), cameraFbo.getHeight());  // draw flipped
		cameraFbo.end();
		
		opticalFlow.setInput(cameraFbo.getTexture());
	}
	
	opticalFlow.update();
	velocityBridge.setInput(opticalFlow.getVelocity());
	velocityBridge.update(dt);
	densityBridge.setDensity(cameraFbo.getTexture());
	densityBridge.setVelocity(opticalFlow.getVelocity());
	densityBridge.update(dt);
	fluidSimulation.addVelocity(velocityBridge.getVelocity());
	fluidSimulation.addDensity(densityBridge.getDensity());
	fluidSimulation.addTemperature(densityBridge.getLuminance());
	fluidSimulation.update(dt);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	
	switch (key) {
		default: break;
		case '1': visualizationMode.set(FT_INPUT_FOR_DENSITY); break;
		case '2': visualizationMode.set(FT_INPUT_FOR_VELOCITY); break;
		case '3': visualizationMode.set(FT_FLOW_VELOCITY); break;
		case '4': visualizationMode.set(FT_BRIDGE_VELOCITY); break;
		case '5': visualizationMode.set(FT_BRIDGE_DENSITY); break;
		case '6': visualizationMode.set(FT_FLUID_VORTICITY); break;
		case '7': visualizationMode.set(FT_FLUID_TEMPERATURE); break;
		case '8': visualizationMode.set(FT_FLUID_PRESSURE); break;
		case '9': visualizationMode.set(FT_FLUID_VELOCITY); break;
		case '0': visualizationMode.set(FT_FLUID_DENSITY); break;
		case 'G':
		case 'g': toggleGuiDraw = !toggleGuiDraw; break;
		case 'f':
		case 'F': toggleFullScreen.set(!toggleFullScreen.get()); break;
		case 'c':
		case 'C': toggleCameraDraw.set(!toggleCameraDraw.get()); break;
		case 'r':
		case 'R':
			for (auto flow : flows) { flow->reset(); }
			fluidSimulation.addObstacle(flowToolsLogo.getTexture());
			break;
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	
	ofClear(0,0);
	
	ofPushStyle();
	if (toggleCameraDraw.get()) {
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		cameraFbo.draw(0, 0, windowWidth, windowHeight);
	}
	
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	switch(visualizationMode.get()) {
		case FT_INPUT_FOR_VELOCITY: opticalFlow.drawInput(0, 0, windowWidth, windowHeight); break;
		case FT_INPUT_FOR_DENSITY: 	densityBridge.drawInput(0, 0, windowWidth, windowHeight); break;
		case FT_FLOW_VELOCITY: 		opticalFlow.draw(0, 0, windowWidth, windowHeight); break;
		case FT_BRIDGE_VELOCITY: 	velocityBridge.draw(0, 0, windowWidth, windowHeight); break;
		case FT_BRIDGE_DENSITY: 	densityBridge.draw(0, 0, windowWidth, windowHeight); break;
		case FT_BRIDGE_TEMPERATURE: break;
		case FT_BRIDGE_PRESSURE: 	break;
		case FT_OBSTACLE_TEMPORARY: break;
		case FT_OBSTACLE_CONSTANT: 	fluidSimulation.drawObstacles(0, 0, windowWidth, windowHeight); break;
		case FT_FLUID_BUOYANCY: 	fluidSimulation.drawBuoyancy(0, 0, windowWidth, windowHeight); break;
		case FT_FLUID_VORTICITY: 	fluidSimulation.drawVorticityVelocity(0, 0, windowWidth, windowHeight); break;
		case FT_FLUID_DIVERGENCE: 	fluidSimulation.drawDivergence(0, 0, windowWidth, windowHeight); break;
		case FT_FLUID_TEMPERATURE: 	fluidSimulation.drawTemperature(0, 0, windowWidth, windowHeight); break;
		case FT_FLUID_PRESSURE: 	fluidSimulation.drawPressure(0, 0, windowWidth, windowHeight); break;
		case FT_FLUID_VELOCITY: 	fluidSimulation.drawVelocity(0, 0, windowWidth, windowHeight); break;
		case FT_FLUID_DENSITY: 		fluidSimulation.draw(0, 0, windowWidth, windowHeight); break;
		case FT_NONE:
		default: break;
	}
	
	ofEnableBlendMode(OF_BLENDMODE_SUBTRACT);
	flowToolsLogo.draw(0, 0, windowWidth, windowHeight);
	
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	if (toggleGuiDraw) {
		drawGui();
	}
	ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::drawGui() {
	guiFPS = (int)(ofGetFrameRate() + 0.5);
	
	// calculate minimum fps
	float deltaTime = ofGetElapsedTimef() - lastTime;
	lastTime = ofGetElapsedTimef();
	deltaTimeDeque.push_back(deltaTime);
	
	while (deltaTimeDeque.size() > guiFPS.get())
		deltaTimeDeque.pop_front();
	
	float longestTime = 0;
	for (int i=0; i<deltaTimeDeque.size(); i++){
		if (deltaTimeDeque[i] > longestTime)
			longestTime = deltaTimeDeque[i];
	}
	
	guiMinFPS.set(1.0 / longestTime);
	
	
	ofPushStyle();
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	gui.draw();
	ofPopStyle();
}

