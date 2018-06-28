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
	fieldWidth = flowWidth / 2;
	fieldHeight = flowWidth / 2;
	
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
	
	visualizeParameters.setName("flow visualisation");
	visualizeParameters.add(drawMode.set("draw mode", FT_FLUID_DENSITY, FT_NONE, FT_FLUID_DENSITY));
	visualizeParameters.add(drawName.set("MODE", "Fluid Density"));
	visualizeParameters.add(showField.set("show field", false));
	visualizeParameters.add(visualizeScale.set("scale", 0.3, 0.1, 3.0));
	drawMode.addListener(this, &ofApp::drawModeListener);
	showField.addListener(this, &ofApp::showFieldListener);
	visualizeScale.addListener(this, &ofApp::visualizeScaleListener);
	
	bool s = true;
	switchGuiColor(s = !s);
	gui.add(visualizeParameters);
	switchGuiColor(s = !s);
	gui.add(opticalFlow.getParameters());
	switchGuiColor(s = !s);
	gui.add(velocityBridge.getParameters());
	switchGuiColor(s = !s);
	gui.add(densityBridge.getParameters());
	switchGuiColor(s = !s);
	gui.add(fluidSimulation.getParameters());
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
		
		opticalFlow.setInput(cameraFbo.getTexture());
	}
	
//	flowMouse.update(dt);
//	for (int i=0; i<flowMouse.getNumForces(); i++) {
//		if (flowMouse.didChange(i)) {
//			flowCore.addFlow(flowMouse.getType(i), flowMouse.getTextureReference(i), flowMouse.getStrength(i));
//			if (flowMouse.getType(i) == FT_FLUID_VELOCITY) {
//				flowParticles.addFlowVelocity(flowMouse.getTextureReference(i), flowMouse.getStrength(i));
//			}
//		}
//	}
	
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
	
	flowParticles.setSpeed(fluidSimulation.getSpeed());
	flowParticles.setCellSize(fluidSimulation.getCellSize());
	flowParticles.addFlowVelocity(opticalFlow.getVelocity());
	flowParticles.addFluidVelocity(fluidSimulation.getVelocity());
	flowParticles.setObstacle(fluidSimulation.getObstacle());
	flowParticles.update(dt);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	
	switch (key) {
		default: break;
		case '1': drawMode.set(FT_INPUT_FOR_DENSITY); break;
		case '2': drawMode.set(FT_INPUT_FOR_VELOCITY); break;
		case '3': drawMode.set(FT_FLOW_VELOCITY); break;
		case '4': drawMode.set(FT_BRIDGE_VELOCITY); break;
		case '5': drawMode.set(FT_BRIDGE_DENSITY); break;
		case '6': drawMode.set(FT_FLUID_VORTICITY); break;
		case '7': drawMode.set(FT_FLUID_TEMPERATURE); break;
		case '8': drawMode.set(FT_FLUID_PRESSURE); break;
		case '9': drawMode.set(FT_FLUID_VELOCITY); break;
		case '0': drawMode.set(FT_FLUID_DENSITY); break;
		case 'G':
		case 'g': toggleGuiDraw = !toggleGuiDraw; break;
		case 'f':
		case 'F': doFullScreen.set(!doFullScreen.get()); break;
		case 'c':
		case 'C': doDrawCamera.set(!doDrawCamera.get()); break;
		case 'r':
		case 'R':
			for (auto flow : flows) { flow->reset(); }
			fluidSimulation.addObstacle(flowToolsLogo.getTexture());
			flowMouse.reset();
			flowParticles.reset();
			break;
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofClear(0,0);
	
	ofPushStyle();
	if (doDrawCamera.get()) {
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		cameraFbo.draw(0, 0, windowWidth, windowHeight);
	}
	
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	switch(drawMode.get()) {
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
	
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	if (flowParticles.isActive()) { flowParticles.draw(0, 0, windowWidth, windowHeight); }
	
	ofEnableBlendMode(OF_BLENDMODE_SUBTRACT);
	flowToolsLogo.draw(0, 0, windowWidth, windowHeight);
	ofPopStyle();
	
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

