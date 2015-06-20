#include "flowToolsApp.h"

//--------------------------------------------------------------
void flowToolsApp::setup(){
	
	ofSetVerticalSync(false);
	
	drawWidth = 1280 / 2;
	drawHeight = 720 / 2;
	// process all but the density on 16th resolution
	flowWidth = drawWidth/4;
	flowHeight = drawHeight/4;
	
	// FLOW & MASK
	opticalFlow.setup(flowWidth, flowHeight);
	velocityMask.setup(drawWidth, drawHeight);
	
	// FLUID & PARTICLES
#ifdef USE_FASTER_INTERNAL_FORMATS
	fluidSimulation.setup(flowWidth, flowHeight, drawWidth, drawHeight, true);
	particleFlow.setup(flowWidth, flowHeight, drawWidth, drawHeight, true);
#else
	fluidSimulation.setup(flowWidth, flowHeight, drawWidth, drawHeight, false);
	particleFlow.setup(flowWidth, flowHeight, drawWidth, drawHeight, false);
#endif
	
	flowToolsLogoImage.loadImage("flowtools.png");
	fluidSimulation.addObstacle(flowToolsLogoImage.getTextureReference());
	showLogo = true;
	
	// VISUALIZATION
	displayScalar.setup(flowWidth, flowHeight);
	velocityField.setup(flowWidth / 4, flowHeight / 4);
	temperatureField.setup(flowWidth / 4, flowHeight / 4);
	velocityTemperatureField.setup(flowWidth / 4, flowHeight / 4);
	
	// MOUSE DRAW
	mouseDrawForces.setup(flowWidth, flowHeight, drawWidth, drawHeight);
	
	// CAMERA
	simpleCam.initGrabber(640, 480, true);
	didCamUpdate = false;
	cameraFbo.allocate(640, 480);
	cameraFbo.clear();
	
	// GUI
	setupGui();
	
	lastTime = ofGetElapsedTimef();
	
}

//--------------------------------------------------------------
void flowToolsApp::setupGui() {
	ofSetVerticalSync(false);
	
	gui.setup("settings");
	gui.setDefaultBackgroundColor(ofColor(0, 0, 0, 127));
	gui.setDefaultFillColor(ofColor(160, 160, 160, 160));
	gui.add(guiFPS.set("average FPS", 0, 0, 60));
	gui.add(guiMinFPS.set("minimum FPS", 0, 0, 60));
	gui.add(doFullScreen.set("fullscreen (F)", false));
	doFullScreen.addListener(this, &flowToolsApp::setFullScreen);
	gui.add(toggleGuiDraw.set("show gui (G)", false));
	gui.add(doFlipCamera.set("flip camera (C)", true));
	gui.add(drawMode.set("draw mode", DRAW_COMPOSITE, DRAW_COMPOSITE, DRAW_RIGHT_MOUSE_2));
	drawMode.addListener(this, &flowToolsApp::drawModeSetName);
	gui.add(drawName.set("MODE", "draw name"));
		
	
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
	gui.add(fluidSimulation.parameters);

	gui.setDefaultHeaderBackgroundColor(guiHeaderColor[guiColorSwitch]);
	gui.setDefaultFillColor(guiFillColor[guiColorSwitch]);
	guiColorSwitch = 1 - guiColorSwitch;
	gui.add(particleFlow.parameters);
	
	gui.setDefaultHeaderBackgroundColor(guiHeaderColor[guiColorSwitch]);
	gui.setDefaultFillColor(guiFillColor[guiColorSwitch]);
	guiColorSwitch = 1 - guiColorSwitch;
	gui.add(mouseDrawForces.leftButtonParameters);
	
	gui.setDefaultHeaderBackgroundColor(guiHeaderColor[guiColorSwitch]);
	gui.setDefaultFillColor(guiFillColor[guiColorSwitch]);
	guiColorSwitch = 1 - guiColorSwitch;
	gui.add(mouseDrawForces.rightButtonParameters);
	
	visualizeParameters.setName("visualizers");
	visualizeParameters.add(showScalar.set("show scalar", true));
	visualizeParameters.add(showField.set("show field", true));
	visualizeParameters.add(displayScalarScale.set("scalar scale", 0.15, 0.05, 0.5));
	displayScalarScale.addListener(this, &flowToolsApp::setDisplayScalarScale);
	visualizeParameters.add(velocityFieldScale.set("velocity scale", 0.1, 0.0, 0.5));
	velocityFieldScale.addListener(this, &flowToolsApp::setVelocityFieldArrowScale);
	visualizeParameters.add(velocityLineSmooth.set("arrow smooth", false));
	visualizeParameters.add(temperatureFieldScale.set("temperature scale", 0.25, 0.05, 0.5));
	temperatureFieldScale.addListener(this, &flowToolsApp::setTemperatureFieldBarScale);
	velocityLineSmooth.addListener(this, &flowToolsApp::setVelocityLineSmooth);
	
	gui.setDefaultHeaderBackgroundColor(guiHeaderColor[guiColorSwitch]);
	gui.setDefaultFillColor(guiFillColor[guiColorSwitch]);
	guiColorSwitch = 1 - guiColorSwitch;
	gui.add(visualizeParameters);

	gui.loadFromFile("settings.xml");
	gui.minimizeAll();
	
	toggleGuiDraw = true;
	
}

//--------------------------------------------------------------
void flowToolsApp::update(){
	
	deltaTime = ofGetElapsedTimef() - lastTime;
	lastTime = ofGetElapsedTimef();
	
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
		
		opticalFlow.setSource(cameraFbo.getTextureReference());
		opticalFlow.update(deltaTime);
		
		velocityMask.setDensity(cameraFbo.getTextureReference());
		velocityMask.setVelocity(opticalFlow.getOpticalFlow());
		velocityMask.update();
	}
	
	
	fluidSimulation.addVelocity(opticalFlow.getOpticalFlowDecay());
	fluidSimulation.addDensity(velocityMask.getColorMask());
	fluidSimulation.addTemperature(velocityMask.getLuminanceMask());
	
	mouseDrawForces.update(deltaTime);
	
	for (int i=0; i<mouseDrawForces.getNumForces(); i++) {
		if (mouseDrawForces.didChange(i)) {
			switch (mouseDrawForces.getType(i)) {
				case FT_DENSITY:
					fluidSimulation.addDensity(mouseDrawForces.getTextureReference(i), mouseDrawForces.getStrength(i));
					break;
				case FT_VELOCITY:
					fluidSimulation.addVelocity(mouseDrawForces.getTextureReference(i), mouseDrawForces.getStrength(i));
					particleFlow.addFlowVelocity(mouseDrawForces.getTextureReference(i), mouseDrawForces.getStrength(i));
					break;
				case FT_TEMPERATURE:
					fluidSimulation.addTemperature(mouseDrawForces.getTextureReference(i), mouseDrawForces.getStrength(i));
					break;
				case FT_PRESSURE:
					fluidSimulation.addPressure(mouseDrawForces.getTextureReference(i), mouseDrawForces.getStrength(i));
					break;
				case FT_OBSTACLE:
					fluidSimulation.addTempObstacle(mouseDrawForces.getTextureReference(i));
				default:
					break;
			}
		}
	}
	
	fluidSimulation.update();
	
	if (particleFlow.isActive()) {
		particleFlow.setSpeed(fluidSimulation.getSpeed());
		particleFlow.setCellSize(fluidSimulation.getCellSize());
		particleFlow.addFlowVelocity(opticalFlow.getOpticalFlow());
		particleFlow.addFluidVelocity(fluidSimulation.getVelocity());
//		particleFlow.addDensity(fluidSimulation.getDensity());
		particleFlow.setObstacle(fluidSimulation.getObstacle());
	}
	particleFlow.update();
	
}

//--------------------------------------------------------------
void flowToolsApp::keyPressed(int key){
	switch (key) {
		case 'G':
		case 'g': toggleGuiDraw = !toggleGuiDraw; break;
		case 'f':
		case 'F': ofToggleFullscreen(); break;
			
		case '1': drawMode.set(DRAW_COMPOSITE); break;
		case '2': drawMode.set(DRAW_PARTICLES); break;
		case '3': drawMode.set(DRAW_FLUID_DENSITY); break;
		case '4': drawMode.set(DRAW_FLUID_VELOCITY); break;
		case '5': drawMode.set(DRAW_FLUID_PRESSURE); break;
		case '6': drawMode.set(DRAW_FLUID_TEMPERATURE); break;
		case '7': drawMode.set(DRAW_FLUID_VORTICITY); break;
		case '8': drawMode.set(DRAW_FLUID_BUOYANCY); break;
		case '9': drawMode.set(DRAW_FLUID_OBSTACLE); break;
		case '0': drawMode.set(DRAW_FLOW_MASK); break;
		case '-': drawMode.set(DRAW_OPTICAL_FLOW); break;
		case '=': drawMode.set(DRAW_SOURCE); break;
		case '!': drawMode.set(DRAW_LEFT_MOUSE_0); break;
		case '@': drawMode.set(DRAW_LEFT_MOUSE_1); break;
		case '#': drawMode.set(DRAW_LEFT_MOUSE_2); break;
		case '$': drawMode.set(DRAW_RIGHT_MOUSE_0); break;
		case '%': drawMode.set(DRAW_RIGHT_MOUSE_1); break;
		case '^': drawMode.set(DRAW_RIGHT_MOUSE_2); break;
			
		case 'r':
		case 'R':
			fluidSimulation.reset();
			showLogo = false;
			break;
			
		default: break;
	}
}

//--------------------------------------------------------------
void flowToolsApp::drawModeSetName(int &_value) {
	switch(_value) {
		case DRAW_COMPOSITE:		drawName.set("Composite      (1)"); break;
		case DRAW_PARTICLES:		drawName.set("Particles      (2)"); break;
		case DRAW_FLUID_DENSITY:	drawName.set("Fluid Color    (3)"); break;
		case DRAW_FLUID_VELOCITY:	drawName.set("Fluid Velocity (4)"); break;
		case DRAW_FLUID_PRESSURE:	drawName.set("Fluid Pressure (5)"); break;
		case DRAW_FLUID_TEMPERATURE:drawName.set("Fld Temperature(6)"); break;
		//		case DRAW_FLUID_DIVERGENCE: drawName.set("Fld Divergence (6)"); break;
		case DRAW_FLUID_VORTICITY:	drawName.set("Fluid Vorticity(7)"); break;
		case DRAW_FLUID_BUOYANCY:	drawName.set("Fluid Buoyancy (8)"); break;
		case DRAW_FLUID_OBSTACLE:	drawName.set("Fluid Obstacle (9)"); break;
		case DRAW_FLOW_MASK:		drawName.set("Flow Mask      (0)"); break;
		case DRAW_OPTICAL_FLOW:		drawName.set("Optical Flow   (-)"); break;
		case DRAW_SOURCE:			drawName.set("Source         (=)"); break;
		case DRAW_LEFT_MOUSE_0:		drawName.set("Left Mouse 0   (!)"); break;
		case DRAW_LEFT_MOUSE_1:		drawName.set("Left Mouse 1   (@)"); break;
		case DRAW_LEFT_MOUSE_2:		drawName.set("Left Mouse 2   (#)"); break;
		case DRAW_RIGHT_MOUSE_0:	drawName.set("Right Mouse 0  ($)"); break;
		case DRAW_RIGHT_MOUSE_1:	drawName.set("Right Mouse 1  (%)"); break;
		case DRAW_RIGHT_MOUSE_2:	drawName.set("Right Mouse 2  (^)"); break;
	}
}

//--------------------------------------------------------------
void flowToolsApp::draw(){
	ofClear(0,0);

	if (!toggleGuiDraw) {
		ofHideCursor();
		drawComposite();
	}
	else {
		ofShowCursor();
		switch(drawMode.get()) {
			case DRAW_COMPOSITE: drawComposite(); break;
			case DRAW_PARTICLES: drawParticles(); break;
			case DRAW_FLUID_DENSITY: drawFluidDensity(); break;
			case DRAW_FLUID_VELOCITY: drawFluidVelocity(); break;
			case DRAW_FLUID_PRESSURE: drawFluidPressure(); break;
			case DRAW_FLUID_TEMPERATURE: drawFluidTemperature(); break;
			//	case DRAW_FLUID_DIVERGENCE: drawFluidDivergence(); break;
			case DRAW_FLUID_VORTICITY: drawFluidVorticity(); break;
			case DRAW_FLUID_BUOYANCY: drawFluidBuoyance(); break;
			case DRAW_FLUID_OBSTACLE: drawFluidObstacle(); break;
			case DRAW_FLOW_MASK: drawMask(); break;
			case DRAW_OPTICAL_FLOW: drawOpticalFlow(); break;
			case DRAW_SOURCE: drawSource(); break;
			case DRAW_LEFT_MOUSE_0: drawMouseDraw(0); break;
			case DRAW_LEFT_MOUSE_1: drawMouseDraw(1); break;
			case DRAW_LEFT_MOUSE_2: drawMouseDraw(2); break;
			case DRAW_RIGHT_MOUSE_0: drawMouseDraw(3); break;
			case DRAW_RIGHT_MOUSE_1: drawMouseDraw(4); break;
			case DRAW_RIGHT_MOUSE_2: drawMouseDraw(5); break;
		}
		drawGui();
	}
}

//--------------------------------------------------------------
void flowToolsApp::drawComposite(int _x, int _y, int _width, int _height) {
	ofPushStyle();
	
	ofEnableBlendMode(OF_BLENDMODE_DISABLED);
	cameraFbo.draw(_x, _y, _width, _height);
	
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	fluidSimulation.draw(_x, _y, _width, _height);
	
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	if (particleFlow.isActive())
		particleFlow.draw(_x, _y, _width, _height);
	
	if (showLogo) {
		flowToolsLogoImage.draw(_x, _y, _width, _height);
	}
	
	ofPopStyle();
}

//--------------------------------------------------------------
void flowToolsApp::drawParticles(int _x, int _y, int _width, int _height) {
	ofPushStyle();
	ofEnableBlendMode(OF_BLENDMODE_DISABLED);
	if (particleFlow.isActive())
		particleFlow.draw(_x, _y, _width, _height);
	ofPopStyle();
}

//--------------------------------------------------------------
void flowToolsApp::drawFluidDensity(int _x, int _y, int _width, int _height) {
	ofPushStyle();
	fluidSimulation.draw(_x, _y, _width, _height);
	
	
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	velocityTemperatureField.setVelocity(fluidSimulation.getVelocity());
	velocityTemperatureField.setTemperature(fluidSimulation.getPressure());
	velocityTemperatureField.draw(_x, _y, _width, _height);
	
	
	ofPopStyle();}

//--------------------------------------------------------------
void flowToolsApp::drawFluidVelocity(int _x, int _y, int _width, int _height) {
	ofPushStyle();
	if (showScalar.get()) {
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		displayScalar.setSource(fluidSimulation.getVelocity());
		displayScalar.draw(_x, _y, _width, _height);
	}
	if (showField.get()) {
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		velocityField.setVelocity(fluidSimulation.getVelocity());
		velocityField.draw(_x, _y, _width, _height);
	}
	ofPopStyle();
}

//--------------------------------------------------------------
void flowToolsApp::drawFluidPressure(int _x, int _y, int _width, int _height) {
	ofPushStyle();
	if (showScalar.get()) {
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		displayScalar.setSource(fluidSimulation.getPressure());
		displayScalar.draw(_x, _y, _width, _height);
	}
	if (showField.get()) {
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		temperatureField.setTemperature(fluidSimulation.getPressure());
		temperatureField.draw(_x, _y, _width, _height);
	}
	ofPopStyle();
}

//--------------------------------------------------------------
void flowToolsApp::drawFluidTemperature(int _x, int _y, int _width, int _height) {
	ofPushStyle();
	if (showScalar.get()) {
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		displayScalar.setSource(fluidSimulation.getTemperature());
		displayScalar.draw(_x, _y, _width, _height);
	}
	if (showField.get()) {
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		temperatureField.setTemperature(fluidSimulation.getTemperature());
		temperatureField.draw(_x, _y, _width, _height);
	}
	ofPopStyle();
}

//--------------------------------------------------------------
void flowToolsApp::drawFluidDivergence(int _x, int _y, int _width, int _height) {
	ofPushStyle();
	if (showScalar.get()) {
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		displayScalar.setSource(fluidSimulation.getDivergence());
		displayScalar.draw(_x, _y, _width, _height);
	}
	if (showField.get()) {
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		temperatureField.setTemperature(fluidSimulation.getDivergence());
		temperatureField.draw(_x, _y, _width, _height);
	}
	ofPopStyle();}

//--------------------------------------------------------------
void flowToolsApp::drawFluidVorticity(int _x, int _y, int _width, int _height) {
	ofPushStyle();
	if (showScalar.get()) {
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		displayScalar.setSource(fluidSimulation.getConfinement());
		displayScalar.draw(_x, _y, _width, _height);
	}
	if (showField.get()) {
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		ofSetColor(255, 255, 255, 255);
		velocityField.setVelocity(fluidSimulation.getConfinement());
		velocityField.draw(_x, _y, _width, _height);
	}
	ofPopStyle();}

//--------------------------------------------------------------
void flowToolsApp::drawFluidBuoyance(int _x, int _y, int _width, int _height) {
	ofPushStyle();
	if (showScalar.get()) {
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		displayScalar.setSource(fluidSimulation.getSmokeBuoyancy());
		displayScalar.draw(_x, _y, _width, _height);
	}
	if (showField.get()) {
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		velocityField.setVelocity(fluidSimulation.getSmokeBuoyancy());
		velocityField.draw(_x, _y, _width, _height);
	}
	ofPopStyle();
}

//--------------------------------------------------------------
void flowToolsApp::drawFluidObstacle(int _x, int _y, int _width, int _height) {
	ofPushStyle();
	ofEnableBlendMode(OF_BLENDMODE_DISABLED);
	fluidSimulation.getObstacle().draw(_x, _y, _width, _height);
	ofPopStyle();
}

//--------------------------------------------------------------
void flowToolsApp::drawMask(int _x, int _y, int _width, int _height) {
	ofPushStyle();
	ofEnableBlendMode(OF_BLENDMODE_DISABLED);
	velocityMask.draw(_x, _y, _width, _height);
	ofPopStyle();
}

//--------------------------------------------------------------
void flowToolsApp::drawOpticalFlow(int _x, int _y, int _width, int _height) {
	
	ofPushStyle();
	opticalFlow.getOpticalFlow().draw(_x, _y, _width, _height);
	
	if (showScalar.get()) {
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		displayScalar.setSource(opticalFlow.getOpticalFlowDecay());
		displayScalar.draw(0, 0, _width, _height);
	}
	if (showField.get()) {
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		velocityField.setVelocity(opticalFlow.getOpticalFlowDecay());
		velocityField.draw(0, 0, _width, _height);
	}
	ofPopStyle();
}

//--------------------------------------------------------------
void flowToolsApp::drawSource(int _x, int _y, int _width, int _height) {
	ofPushStyle();
	ofEnableBlendMode(OF_BLENDMODE_DISABLED);
	cameraFbo.draw(_x, _y, _width, _height);
	ofPopStyle();
}

//--------------------------------------------------------------
void flowToolsApp::drawMouseDraw(int _index, int _x, int _y, int _width, int _height) {
	ofPushStyle();

	if (mouseDrawForces.didChange(_index)) {
		ftDrawForceType dinges = mouseDrawForces.getType(_index);
		
		
		switch (mouseDrawForces.getType(_index)) {
			case FT_DENSITY:
				ofEnableBlendMode(OF_BLENDMODE_DISABLED);
				mouseDrawForces.getTextureReference(_index).draw(_x, _y, _width, _height);
				
				break;
			case FT_VELOCITY:
				if (showScalar.get()) {
					ofEnableBlendMode(OF_BLENDMODE_DISABLED);
					displayScalar.setSource(mouseDrawForces.getTextureReference(_index));
					displayScalar.draw(_x, _y, _width, _height);
				}
				if (showField.get()) {
					ofEnableBlendMode(OF_BLENDMODE_ADD);
					velocityField.setVelocity(mouseDrawForces.getTextureReference(_index));
					velocityField.draw(_x, _y, _width, _height);
				}
				break;
			case FT_TEMPERATURE:
				if (showScalar.get()) {
					ofEnableBlendMode(OF_BLENDMODE_DISABLED);
					displayScalar.setSource(mouseDrawForces.getTextureReference(_index));
					displayScalar.draw(_x, _y, _width, _height);
				}
				if (showField.get()) {
					ofEnableBlendMode(OF_BLENDMODE_ADD);
					temperatureField.setTemperature(mouseDrawForces.getTextureReference(_index));
					temperatureField.draw(_x, _y, _width, _height);
				}
				break;
			case FT_PRESSURE:
				if (showScalar.get()) {
					ofEnableBlendMode(OF_BLENDMODE_DISABLED);
					displayScalar.setSource(mouseDrawForces.getTextureReference(_index));
					displayScalar.draw(_x, _y, _width, _height);
				}
				if (showField.get()) {
					ofEnableBlendMode(OF_BLENDMODE_ADD);
					temperatureField.setTemperature(mouseDrawForces.getTextureReference(_index));
					temperatureField.draw(_x, _y, _width, _height);
				}
				break;
			case FT_OBSTACLE:
				ofEnableBlendMode(OF_BLENDMODE_DISABLED);
				mouseDrawForces.getTextureReference(_index).draw(_x, _y, _width, _height);
			default:
				break;
		}
	}

	ofPopStyle();
}

//--------------------------------------------------------------
void flowToolsApp::drawGui() {
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
	
	// HACK TO COMPENSATE FOR DISSAPEARING MOUSE
	ofEnableBlendMode(OF_BLENDMODE_SUBTRACT);
	ofCircle(ofGetMouseX(), ofGetMouseY(), ofGetWindowWidth() / 300.0);
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	ofCircle(ofGetMouseX(), ofGetMouseY(), ofGetWindowWidth() / 600.0);
	ofPopStyle;
}

