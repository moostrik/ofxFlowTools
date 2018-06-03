#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	
//	ofSetVerticalSync(false);
	ofSetLogLevel(OF_LOG_NOTICE);
	
	densityWidth = 1280;
	densityHeight = 720;
	flowWidth = densityWidth / 4;
	flowHeight = densityHeight / 4;
	
	flowCore.setup(densityWidth, densityHeight, flowWidth, flowHeight);
	
	flowToolsLogo.load("flowtools.png");
	flowCore.addFlow(FT_CORE_OBSTACLE_CONSTANT, flowToolsLogo.getTexture());
	
	flowMouse.setup(flowWidth, flowHeight, densityWidth, densityHeight);
	
	flowParticles.setup(flowWidth, flowHeight, densityWidth, densityHeight);
	flowParticles.setObstacle(flowToolsLogo.getTexture());
	
	// CAMERA
	simpleCam.setup(densityWidth, densityHeight, true);
	cameraFbo.allocate(densityWidth, densityHeight);
	ftUtil::zero(cameraFbo);
	
	// GUI
	setupGui();
	
	// DRAW
	windowWidth = ofGetWindowWidth();
	windowHeight = ofGetWindowHeight();
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
	gui.add(flowCore.getCoreParameters());
	switchGuiColor(s = !s);
	gui.add(flowCore.getOpticalFlowParameters());
	switchGuiColor(s = !s);
	gui.add(flowCore.getVelocityBridgeParameters());
	switchGuiColor(s = !s);
	gui.add(flowCore.getDensityBridgeParameters());
	switchGuiColor(s = !s);
	gui.add(flowCore.getFluidSimulationParameters());
	switchGuiColor(s = !s);
	gui.add(flowMouse.getParameters());
	switchGuiColor(s = !s);
	gui.add(flowParticles.getParameters());

	// if the settings file is not present the parameters will not be set during this setup
	if (!ofFile("settings.xml"))
		gui.saveToFile("settings.xml");
	
	gui.loadFromFile("settings.xml");
	
	gui.minimizeAll();
	toggleGuiDraw = true;
	
	lastTime = ofGetElapsedTimef();
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
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		cameraFbo.begin();
		if (doFlipCamera)
			simpleCam.draw(cameraFbo.getWidth(), 0, -cameraFbo.getWidth(), cameraFbo.getHeight());  // Flip Horizontal
		else
			simpleCam.draw(0, 0, cameraFbo.getWidth(), cameraFbo.getHeight());
		cameraFbo.end();
		ofPopStyle();
		
		flowCore.setInput(cameraFbo.getTexture());
	}
	
	flowMouse.update(dt);
	for (int i=0; i<flowMouse.getNumForces(); i++) {
		if (flowMouse.didChange(i)) {
			flowCore.addFlow(flowMouse.getType(i), flowMouse.getTextureReference(i), flowMouse.getStrength(i));
			if (flowMouse.getType(i) == FT_CORE_FLUID_VELOCITY) {
				flowParticles.addFlowVelocity(flowMouse.getTextureReference(i), flowMouse.getStrength(i));
			}
		}
	}
	
	flowCore.update(dt);
	
//	flowParticles.setSpeed(flowCore.getSpeed());
//	flowParticles.setCellSize(flowCore.getCellSize());
	flowParticles.addFlowVelocity(flowCore.getOpticalFlowVelocity());
	flowParticles.addFluidVelocity(flowCore.getFluidVelocity());
//	flowParticles.setObstacle(flowCore.getObstacle());
	flowParticles.update(dt);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	
	switch (key) {
		case 'G':
		case 'g': toggleGuiDraw = !toggleGuiDraw; break;
		case 'f':
		case 'F': doFullScreen.set(!doFullScreen.get()); break;
		case 'c':
		case 'C': doDrawCamera.set(!doDrawCamera.get()); break;
		case 'r':
		case 'R':
			flowCore.reset();
			flowMouse.reset();
			flowCore.addFlow(FT_CORE_OBSTACLE_CONSTANT, flowToolsLogo.getTexture());
			flowParticles.reset();
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
	
	flowCore.draw(0, 0, windowWidth, windowHeight, OF_BLENDMODE_ALPHA);
	
	if (flowParticles.isActive()) { flowParticles.draw(0, 0, windowWidth, windowHeight, OF_BLENDMODE_ADD); }
	
	ofPushStyle();
	ofEnableBlendMode(OF_BLENDMODE_SUBTRACT);
	flowToolsLogo.draw(0, 0, windowWidth, windowHeight);
	ofPopStyle();
	
	if (toggleGuiDraw) {
		drawGui();
	}
}

//--------------------------------------------------------------
void ofApp::drawGui() {
	guiFPS = (int)(ofGetFrameRate() + 0.5);
	
	// calculate minimum fps
	deltaTime = ofGetElapsedTimef() - lastTime;
	lastTime = ofGetElapsedTimef();
	deltaTimeDeque.push_back(deltaTime);
	while (deltaTimeDeque.size() > guiFPS.get()) { deltaTimeDeque.pop_front(); }
	
	float longestTime = 0;
	for (int i=0; i<deltaTimeDeque.size(); i++){
		if (deltaTimeDeque[i] > longestTime) { longestTime = deltaTimeDeque[i]; }
	}
	guiMinFPS.set(1.0 / longestTime);
	
	ofPushStyle();
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	gui.draw();
	ofPopStyle();
}

