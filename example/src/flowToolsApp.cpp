#include "flowToolsApp.h"

//--------------------------------------------------------------
void flowToolsApp::setup(){
	
	ofSetVerticalSync(false);
	
	drawWidth = 1280;
	drawHeight = 720;
	// process all but the density on 16th resolution
	flowWidth = drawWidth/4;
	flowHeight = drawHeight/4;
	
	// Flow & Mask
	opticalFlow.setup(flowWidth, flowHeight);
	velocityMask.setup(drawWidth, drawHeight);
	
	// Fluid
#ifdef USE_FASTER_INTERNAL_FORMATS
	fluid.setup(flowWidth, flowHeight, drawWidth, drawHeight, true);
#else
	fluid.setup(flowWidth, flowHeight, drawWidth, drawHeight, false);
#endif
	
	flowToolsLogoImage.loadImage("flowtools.png");
	fluid.addObstacle(flowToolsLogoImage.getTextureReference());
	showLogo = true;
	
	// Particles
	particleFlow.setup(flowWidth, flowHeight, drawWidth, drawHeight);
	
	// Visualisation
	displayScalar.allocate(flowWidth, flowHeight);
	velocityField.allocate(flowWidth / 4, flowHeight / 4);
	temperatureField.allocate(flowWidth / 4, flowHeight / 4);
	
	// Draw Forces
	numDrawForces = 6;
	flexDrawForces = new ftDrawForce[numDrawForces];
	flexDrawForces[0].setup(drawWidth, drawHeight, FT_DENSITY, true);
	flexDrawForces[0].setName("draw full res");
	flexDrawForces[1].setup(flowWidth, flowHeight, FT_VELOCITY, true);
	flexDrawForces[1].setName("draw flow res 1");
	flexDrawForces[2].setup(flowWidth, flowHeight, FT_TEMPERATURE, true);
	flexDrawForces[2].setName("draw flow res 2");
	flexDrawForces[3].setup(drawWidth, drawHeight, FT_DENSITY, false);
	flexDrawForces[3].setName("draw full res");
	flexDrawForces[4].setup(flowWidth, flowHeight, FT_VELOCITY, false);
	flexDrawForces[4].setName("draw flow res 1");
	flexDrawForces[5].setup(flowWidth, flowHeight, FT_TEMPERATURE, false);
	flexDrawForces[5].setName("draw flow res 2");
	
	// Camera
	simpleCam.initGrabber(640, 480, true);
	didCamUpdate = false;
	cameraFbo.allocate(drawWidth, drawHeight);
	cameraFbo.begin(); ofClear(0); cameraFbo.end();
	
	
	setupGui();
	
	lastTime = ofGetElapsedTimef();
	lastMouse.set(0,0);
	
}

//--------------------------------------------------------------
void flowToolsApp::setupGui() {
	
	gui.setup("settings");
	gui.setDefaultBackgroundColor(ofColor(0, 0, 0, 127));
	gui.setDefaultFillColor(ofColor(160, 160, 160, 160));
	gui.add(guiFPS.set("FPS", 0, 0, 60));
	gui.add(doFullScreen.set("fullscreen (F)", false));
	doFullScreen.addListener(this, &flowToolsApp::setFullScreen);
	gui.add(toggleGuiDraw.set("show gui (G)", false));
	gui.add(doFlipCamera.set("flip camera (C)", true));
	numVisualisationModes = 12;
	gui.add(visualisationMode.set("visualisation mode", 0, 0, numVisualisationModes - 1));
	gui.add(visualisationName.set("MODE", "draw name"));
	
	visualisationModeTitles = new string[numVisualisationModes];
	visualisationModeTitles[0] = "Source         (0)";
	visualisationModeTitles[1] = "Optical Flow   (1)";
	visualisationModeTitles[2] = "Flow Mask      (2)";
	visualisationModeTitles[3] = "Fluid Velocity (3)";
	visualisationModeTitles[4] = "Fluid Pressure (4)";
	visualisationModeTitles[5] = "Fld Temperature(5)";
	visualisationModeTitles[6] = "Fld Divergence (6)";
	visualisationModeTitles[7] = "Fluid Vorticity(7)";
	visualisationModeTitles[8] = "Fluid Buoyancy (8)";
	visualisationModeTitles[9] = "Fluid Obstacle (9)";
	visualisationModeTitles[10] = "Fluid Color    (-)";
	visualisationModeTitles[11] = "Fld Composite  (=)";
	
	int guiColorSwitch = 0;
	ofColor guiHeaderColor[2];
	guiHeaderColor[0].set(160, 160, 80, 200);
	guiHeaderColor[1].set(80, 160, 160, 200);
	ofColor guiFillColor[2];
	guiFillColor[0].set(160, 160, 80, 200);
	guiFillColor[1].set(80, 160, 160, 200);
	
	gui.setDefaultHeaderBackgroundColor(guiHeaderColor[guiColorSwitch]);
	gui.setDefaultFillColor(guiFillColor[guiColorSwitch]);
	guiColorSwitch = 1 - guiColorSwitch;
	gui.add(opticalFlow.parameters);
	
	gui.setDefaultHeaderBackgroundColor(guiHeaderColor[guiColorSwitch]);
	gui.setDefaultFillColor(guiFillColor[guiColorSwitch]);
	guiColorSwitch = 1 - guiColorSwitch;
	gui.add(velocityMask.parameters);
	
	gui.setDefaultHeaderBackgroundColor(guiHeaderColor[guiColorSwitch]);
	gui.setDefaultFillColor(guiFillColor[guiColorSwitch]);
	guiColorSwitch = 1 - guiColorSwitch;
	gui.add(fluid.parameters);

	gui.setDefaultHeaderBackgroundColor(guiHeaderColor[guiColorSwitch]);
	gui.setDefaultFillColor(guiFillColor[guiColorSwitch]);
	guiColorSwitch = 1 - guiColorSwitch;
	gui.add(particleFlow.parameters);
	
	visualisationParameters.setName("visualisation");
	visualisationParameters.add(showScalar.set("show scalar", true));
	visualisationParameters.add(showField.set("show field", true));
	visualisationParameters.add(displayScalarScale.set("display scalar scale", 0.25, 0.05, 0.5));
	displayScalarScale.addListener(this, &flowToolsApp::setDisplayScalarScale);
	visualisationParameters.add(velocityFieldArrowScale.set("arrow scale", 0.6, 0.2, 1));
	velocityFieldArrowScale.addListener(this, &flowToolsApp::setVelocityFieldArrowScale);
	visualisationParameters.add(temperatureFieldBarScale.set("temperature scale", 0.25, 0.05, 0.5));
	temperatureFieldBarScale.addListener(this, &flowToolsApp::setTemperatureFieldBarScale);
	visualisationParameters.add(visualisationLineSmooth.set("line smooth", false));
	visualisationLineSmooth.addListener(this, &flowToolsApp::setVisualisationLineSmooth);

	gui.setDefaultHeaderBackgroundColor(guiHeaderColor[guiColorSwitch]);
	gui.setDefaultFillColor(guiFillColor[guiColorSwitch]);
	guiColorSwitch = 1 - guiColorSwitch;
	gui.add(visualisationParameters);
	
	leftButtonParameters.setName("mouse left button");
	for (int i=0; i<3; i++) {
		leftButtonParameters.add(flexDrawForces[i].parameters);
	}
	gui.setDefaultHeaderBackgroundColor(guiHeaderColor[guiColorSwitch]);
	gui.setDefaultFillColor(guiFillColor[guiColorSwitch]);
	guiColorSwitch = 1 - guiColorSwitch;
	gui.add(leftButtonParameters);
	
	rightButtonParameters.setName("mouse right button");
	for (int i=3; i<6; i++) {
		rightButtonParameters.add(flexDrawForces[i].parameters);
	}
	gui.setDefaultHeaderBackgroundColor(guiHeaderColor[guiColorSwitch]);
	gui.setDefaultFillColor(guiFillColor[guiColorSwitch]);
	guiColorSwitch = 1 - guiColorSwitch;
	gui.add(rightButtonParameters);
	
	gui.setDefaultHeaderBackgroundColor(guiHeaderColor[guiColorSwitch]);
	gui.setDefaultFillColor(guiFillColor[guiColorSwitch]);
	guiColorSwitch = 1 - guiColorSwitch;
	gui.add(doResetDrawForces.set("reset draw forces (D)", false));
	doResetDrawForces.addListener(this,  &flowToolsApp::resetDrawForces);

	gui.loadFromFile("settings.xml");
	gui.minimizeAll();
	
	toggleGuiDraw = true;
	
}

//--------------------------------------------------------------
void flowToolsApp::update(){
	didCamUpdate = false;
	simpleCam.update();
	if (simpleCam.isFrameNew())
		didCamUpdate = true;
	
	deltaTime = ofGetElapsedTimef() - lastTime;
	lastTime = ofGetElapsedTimef();
	
}

//--------------------------------------------------------------
void flowToolsApp::draw(){
	ofClear(0);
	
	if (didCamUpdate) {
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		cameraFbo.begin();
		if (doFlipCamera)
			simpleCam.draw(cameraFbo.getWidth(), 0, -cameraFbo.getWidth(), cameraFbo.getHeight());  // Flip Horizontal
		else
			simpleCam.draw(0, 0, cameraFbo.getWidth(), cameraFbo.getHeight());
		cameraFbo.end();
		ofPopStyle();
		
		opticalFlow.setSource(cameraFbo.getTextureReference());
		opticalFlow.update(deltaTime);
		
		velocityMask.setDensity(cameraFbo.getTextureReference());
		velocityMask.setVelocity(opticalFlow.getOpticalFlow());
		velocityMask.update();
	}


	fluid.addVelocity(opticalFlow.getOpticalFlowDecay());
	fluid.addDensity(velocityMask.getColorMask());
	fluid.addTemperature(velocityMask.getLuminanceMask());

	for (int i=0; i<numDrawForces; i++) {
		flexDrawForces[i].update();
		if (flexDrawForces[i].didChange()) {
			// if a force is constant multiply by deltaTime
			float strength = flexDrawForces[i].getStrength();
			if (!flexDrawForces[i].getIsTemporary())
				strength *=deltaTime;
			switch (flexDrawForces[i].getType()) {
				case FT_DENSITY:
					fluid.addDensity(flexDrawForces[i].getTextureReference(), strength);
					break;
				case FT_VELOCITY:
					fluid.addVelocity(flexDrawForces[i].getTextureReference(), strength);
					particleFlow.addFlowVelocity(flexDrawForces[i].getTextureReference(), strength);
					break;
				case FT_TEMPERATURE:
					fluid.addTemperature(flexDrawForces[i].getTextureReference(), strength);
					break;
				case FT_PRESSURE:
					fluid.addPressure(flexDrawForces[i].getTextureReference(), strength);
					break;
				case FT_OBSTACLE:
					fluid.addTempObstacle(flexDrawForces[i].getTextureReference());
				default:
					break;
			}
		}
	}
	
	fluid.update();

	if (particleFlow.isActive()) {
		particleFlow.setSpeed(fluid.getSpeed());
		particleFlow.setCellSize(fluid.getCellSize());
		particleFlow.addFlowVelocity(opticalFlow.getOpticalFlow());
		particleFlow.addFluidVelocity(fluid.getVelocity());
		particleFlow.setObstacle(fluid.getObstacle());
	}
	particleFlow.update();

	
	int windowWidth = ofGetWindowWidth();
	int windowHeight = ofGetWindowHeight();
	ofClear(0,0);

	switch(visualisationMode.get()) {
		case 0: // Source
			cameraFbo.draw(0,0, windowWidth, windowHeight);
			break;
		case 1: // Optical Flow
			ofPushStyle();
			opticalFlow.getOpticalFlow().draw(0, 0, windowWidth, windowHeight);
			
			if (showScalar.get()) {
				ofEnableBlendMode(OF_BLENDMODE_DISABLED);
				displayScalar.setSource(opticalFlow.getOpticalFlowDecay());
				displayScalar.draw(0, 0, windowWidth, windowHeight);
			}
				if (showField.get()) {
				ofEnableBlendMode(OF_BLENDMODE_ADD);
				velocityField.setSource(opticalFlow.getOpticalFlowDecay());
				velocityField.draw(0, 0, windowWidth, windowHeight);
			}
			ofPopStyle();
			break;
		case 2: // Optical Flow Mask
			velocityMask.draw(0, 0, windowWidth, windowHeight);
			break;
		case 3: // Fluid Velocities
			ofPushStyle();
			if (showScalar.get()) {
				ofEnableBlendMode(OF_BLENDMODE_DISABLED);
				displayScalar.setSource(fluid.getVelocity());
				displayScalar.draw(0, 0, windowWidth, windowHeight);
			}
			if (showField.get()) {
				ofEnableBlendMode(OF_BLENDMODE_ADD);
				velocityField.setSource(fluid.getVelocity());
				velocityField.draw(0, 0, windowWidth, windowHeight);
			}
			ofPopStyle();
			break;
		case 4: // Fluid Pressure
			ofPushStyle();
			if (showScalar.get()) {
				ofEnableBlendMode(OF_BLENDMODE_DISABLED);
				displayScalar.setSource(fluid.getPressure());
				displayScalar.draw(0, 0, windowWidth, windowHeight);
			}
			if (showField.get()) {
				ofEnableBlendMode(OF_BLENDMODE_ADD);
				temperatureField.setSource(fluid.getPressure());
				temperatureField.draw(0, 0, windowWidth, windowHeight);
			}
			ofPopStyle();
			break;
		case 5: // Fluid Temperature
			ofPushStyle();
			if (showScalar.get()) {
				ofEnableBlendMode(OF_BLENDMODE_DISABLED);
				displayScalar.setSource(fluid.getTemperature());
				displayScalar.draw(0, 0, windowWidth, windowHeight);
			}
			if (showField.get()) {
				ofEnableBlendMode(OF_BLENDMODE_ADD);
				temperatureField.setSource(fluid.getTemperature());
				temperatureField.draw(0, 0, windowWidth, windowHeight);
			}
			ofPopStyle();
			break;
		case 6: // Fluid Divergence
			ofPushStyle();
			if (showScalar.get()) {
				ofEnableBlendMode(OF_BLENDMODE_DISABLED);
				displayScalar.setSource(fluid.getDivergence());
				displayScalar.draw(0, 0, windowWidth, windowHeight);
			}
			if (showField.get()) {
				ofEnableBlendMode(OF_BLENDMODE_ADD);
				temperatureField.setSource(fluid.getDivergence());
				temperatureField.draw(0, 0, windowWidth, windowHeight);
			}
			ofPopStyle();
			break;
		case 7: // Fluid Vorticity
			ofPushStyle();
			if (showScalar.get()) {
				ofEnableBlendMode(OF_BLENDMODE_DISABLED);
				displayScalar.setSource(fluid.getConfinement());
				displayScalar.draw(0, 0, windowWidth, windowHeight);
			}
			if (showField.get()) {
				ofEnableBlendMode(OF_BLENDMODE_ADD);
				ofSetColor(255, 255, 255, 255);
				velocityField.setSource(fluid.getConfinement());
				velocityField.draw(0, 0, windowWidth, windowHeight);
			}
			ofPopStyle();
			break;
		case 8: // Fluid Buoyancy
			ofPushStyle();
			if (showScalar.get()) {
				ofEnableBlendMode(OF_BLENDMODE_DISABLED);
				displayScalar.setSource(fluid.getSmokeBuoyancy());
				displayScalar.draw(0, 0, windowWidth, windowHeight);
			}
			if (showField.get()) {
				ofEnableBlendMode(OF_BLENDMODE_ADD);
				velocityField.setSource(fluid.getSmokeBuoyancy());
				velocityField.draw(0, 0, windowWidth, windowHeight);
			}
			ofPopStyle();
			break;
		case 9: // Fluid Obstacle
			fluid.getObstacle().draw(0, 0, windowWidth, windowHeight);
			break;
		case 10: // Fluid Color
			ofPushStyle();
			fluid.draw(0, 0, windowWidth, windowHeight);
			if (particleFlow.isActive())
				particleFlow.draw(0, 0, windowWidth, windowHeight);
			if (showLogo) {
				ofEnableBlendMode(OF_BLENDMODE_SUBTRACT);
				ofSetColor(255,255,255,255);
				flowToolsLogoImage.draw(0, 0, windowWidth, windowHeight);
			}
			ofPopStyle();
			break;
		case 11: // Fluid Composite
			ofPushStyle();
			ofEnableBlendMode(OF_BLENDMODE_DISABLED);
			cameraFbo.draw(0,0, windowWidth, windowHeight);
			
			ofEnableBlendMode(OF_BLENDMODE_ALPHA);
			ofSetColor(255,255,255,64);
			fluid.draw(0, 0, windowWidth, windowHeight);
			ofSetColor(255,255,255,255);
			
			ofEnableBlendMode(OF_BLENDMODE_ADD);
			if (particleFlow.isActive())
				particleFlow.draw(0, 0, windowWidth, windowHeight);
			
			if (showLogo) {
				ofSetColor(255,255,255,255);
				flowToolsLogoImage.draw(0, 0, windowWidth, windowHeight);
			}
			ofPopStyle();
			break;
	}
	
	if (toggleGuiDraw) {
		guiFPS = ofGetFrameRate();
		if (visualisationMode.get() >= numVisualisationModes)
			visualisationMode.set(numVisualisationModes-1);
		visualisationName.set(visualisationModeTitles[visualisationMode.get()]);
		gui.draw();
	}
}

//--------------------------------------------------------------
void flowToolsApp::keyPressed(int key){
	switch (key) {
		case 'G' :
			toggleGuiDraw = !toggleGuiDraw;
			break;
		case 'F':
			ofToggleFullscreen();
			break;
		case 'R':
			fluid.reset();
			showLogo = false;
			for (int i=0; i<numDrawForces; i++)
				flexDrawForces[i].reset();
			break;
		case 'D':
			doResetDrawForces.set(true);
			break;
		case '0':
			visualisationMode.set(0);
			break;
		case '1':
			visualisationMode.set(1);
			break;
		case '2':
			visualisationMode.set(2);
			break;
		case '3':
			visualisationMode.set(3);
			break;
		case '4':
			visualisationMode.set(4);
			break;
		case '5':
			visualisationMode.set(5);
			break;
		case '6':
			visualisationMode.set(6);
			break;
		case '7':
			visualisationMode.set(7);
			break;
		case '8':
			visualisationMode.set(8);
			break;
		case '9':
			visualisationMode.set(9);
			break;
		case '-':
			visualisationMode.set(10);
			break;
		case '=':
			visualisationMode.set(11);
			break;
		default:
			break;
	}
}

//--------------------------------------------------------------
void flowToolsApp::mouseDragged(int x, int y, int button) {
	ofVec2f mouse;
	
	mouse.set(x / (float)ofGetWindowWidth(), y / (float)ofGetWindowHeight());
	ofVec2f velocity = mouse - lastMouse;
	if (button == 0) {
		
		for (int i=0; i<3; i++) {
			if (flexDrawForces[i].getType() == FT_VELOCITY)
				flexDrawForces[i].setForce(velocity);
			flexDrawForces[i].applyForce(mouse);
		}
	}
	else {
		
		for (int i=3; i<numDrawForces; i++) {
			if (flexDrawForces[i].getType() == FT_VELOCITY)
				flexDrawForces[i].setForce(velocity);
			flexDrawForces[i].applyForce(mouse);
		}
	}
	lastMouse.set(mouse.x, mouse.y);

}

//--------------------------------------------------------------
void flowToolsApp::mouseMoved(int x, int y){
	ofVec2f mouse;
	mouse.set(x / (float)ofGetWindowWidth(), y / (float)ofGetWindowHeight());
	lastMouse.set(mouse.x, mouse.y);
	
}
