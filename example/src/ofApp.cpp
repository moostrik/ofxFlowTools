#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	
//	ofSetVerticalSync(false);
	ofSetLogLevel(OF_LOG_NOTICE);
	
//	velocityDots.setup(fieldWidth, fieldHeight);
	
	// AREA
//	area.setup(32, 32, "flow");
	
	flowTools.setup();
	
	
	// CAMERA
	camWidth = 1280;
	camWidth = 720;
	simpleCam.setup(camWidth, camWidth, true);
	didCamUpdate = false;
	cameraFbo.allocate(camWidth, camWidth);
	cameraFbo.black();
	
	// GUI
	setupGui();
	windowWidth = ofGetWindowWidth();
	windowHeight = ofGetWindowHeight();
	
	lastTime = ofGetElapsedTimef();
}

//--------------------------------------------------------------
void ofApp::setupGui() {
	
	gui.setup("settings");
	gui.setDefaultBackgroundColor(ofColor(0, 0, 0, 127));
	gui.setDefaultFillColor(ofColor(160, 160, 160, 160));
	gui.add(guiFPS.set("average FPS", 0, 0, 60));
	gui.add(guiMinFPS.set("minimum FPS", 0, 0, 60));
	gui.add(doFullScreen.set("fullscreen (F)", false));
	doFullScreen.addListener(this, &ofApp::setFullScreen);
	gui.add(toggleGuiDraw.set("show gui (G)", false));
	gui.add(doFlipCamera.set("flip camera", true));
	gui.add(doDrawCamera.set("draw camera (C)", true));
	
	bool s = true;
	switchGuiColor(s = !s);
	gui.add(flowTools.getCoreParameters());
	switchGuiColor(s = !s);
	gui.add(flowTools.getOpticalFlowParameters());
	switchGuiColor(s = !s);
	gui.add(flowTools.getVelocityBridgeParameters());
	switchGuiColor(s = !s);
	gui.add(flowTools.getDensityBridgeParameters());
	switchGuiColor(s = !s);
	gui.add(flowTools.getFluidSimulationParameters());
	
//	gui.add(area.parameters);
//	gui.add(velocityDots.parameters);

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
	
	deltaTime = ofGetElapsedTimef() - lastTime;
	lastTime = ofGetElapsedTimef();
	
	float dt = min(deltaTime, 1.f / 30.f);
	
	simpleCam.update();
	
	if (simpleCam.isFrameNew()) {
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		cameraFbo.begin();
		
		if (doFlipCamera)
			simpleCam.draw(cameraFbo.getWidth(), 0, -cameraFbo.getWidth(), cameraFbo.getHeight());  // Flip Horizontal
		else
			simpleCam.draw(0, 0, cameraFbo.getWidth(), cameraFbo.getHeight());
		cameraFbo.end();
		ofPopStyle();
		
		flowTools.setInput(cameraFbo.getTexture());
	}
	
	flowTools.update(dt);
	
	
	
	
	
//	if (particleFlow.isActive()) {
//		particleFlow.setSpeed(fluidSimulation.getSpeed());
//		particleFlow.setCellSize(fluidSimulation.getCellSize());
//		particleFlow.addFlowVelocity(opticalFlow.getOpticalFlow());
//		particleFlow.addFluidVelocity(fluidSimulation.getVelocity());
////		particleFlow.addDensity(fluidSimulation.getDensity());
//		particleFlow.setObstacle(fluidSimulation.getObstacle());
//	}
//	particleFlow.update(dt);
	
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	flowTools.keyPressed(key);
	
	switch (key) {
		case 'G':
		case 'g': toggleGuiDraw = !toggleGuiDraw; break;
		case 'f':
		case 'F': doFullScreen.set(!doFullScreen.get()); break;
		case 'c':
		case 'C': doDrawCamera.set(!doDrawCamera.get()); break;
		case 'r':
		case 'R':
			
			break;
			
		default: break;
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofClear(0,0);
	
	if (doDrawCamera.get()) {
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		cameraFbo.draw(0, 0, windowWidth, windowHeight);
		ofPopStyle();
	}
	
	flowTools.draw(0, 0, windowWidth, windowHeight);
	
	if (toggleGuiDraw) {
		drawGui();
	}
}

//--------------------------------------------------------------
//void ofApp::drawVelocityDots(int _x, int _y, int _width, int _height) {
//	ofPushStyle();
//	ofEnableBlendMode(OF_BLENDMODE_ADD);
//	velocityDots.setVelocity(fluidSimulation.getVelocity());
//	velocityDots.draw(_x, _y, _width, _height);
//	ofPopStyle();
//}

//--------------------------------------------------------------
void ofApp::drawGui() {
	guiFPS = (int)(ofGetFrameRate() + 0.5);
	
	// calculate minimum fps
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

