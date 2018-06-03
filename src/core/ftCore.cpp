#include "ftCore.h"


ftCore::ftCore() {
	ofAddListener(ofEvents().keyPressed, this, &ftCore::keyPressed);
}

ftCore::~ftCore() {
	ofRemoveListener(ofEvents().keyPressed, this, &ftCore::keyPressed);
}

//--------------------------------------------------------------
void ftCore::setup(int _densityWidth, int _densityHeight, int _flowWidth, int _flowHeight){
	
	densityWidth = _densityWidth;
	densityHeight = _densityHeight;
	// process all but the density on 16th resolution
	flowWidth = (_flowWidth == 0)? densityWidth / 4: _flowWidth;
	flowHeight = (_flowHeight == 0)? densityHeight / 4: _flowHeight;
	fieldWidth = flowWidth / 4;
	fieldHeight = flowWidth / 4;
		
	// CORE FLOW
	opticalFlow.setup(flowWidth, flowHeight);
	velocityBridge.setup(flowWidth, flowHeight);
	densityBridge.setup(flowWidth, flowHeight, densityWidth, densityHeight);
	fluidSimulation.setup(flowWidth, flowHeight, densityWidth, densityHeight);
	
	// VISUALIZATION
	displayScalar.setup(flowWidth, flowHeight);
	velocityField.setup(fieldWidth, fieldHeight);
	temperatureField.setup(fieldWidth, fieldHeight);
	pressureField.setup(fieldWidth, fieldHeight);
	velocityTemperatureField.setup(fieldWidth, fieldHeight);
	
	setupParameters();
	lastTime = ofGetElapsedTimef();
	
}

//--------------------------------------------------------------
void ftCore::setupParameters() {
	
	parameters.setName("ofxFlowTools");
	parameters.add(drawMode.set("draw mode", FT_CORE_DRAW_FLUID_DENSITY, FT_CORE_DRAW_FLUID_DENSITY, FT_CORE_DRAW_OPTICAL_FLOW_VELOCITY));
	parameters.add(drawName.set("MODE", "Fluid Density"));
	
	flowCoreParameters.setName("flow core");
	flowCoreParameters.add(drawMode);
	flowCoreParameters.add(drawName);
	
	visualizeParameters.setName("flow visualisation");
	visualizeParameters.add(showScalar.set("show scalar", true));
	visualizeParameters.add(showField.set("show field", true));
	visualizeParameters.add(visualizeScale.set("scale", 0.3, 0.1, 1.0));
	flowCoreParameters.add(visualizeParameters);
	parameters.add(visualizeParameters);
	
	drawMode.addListener(this, &ftCore::drawModeSetName);
	visualizeScale.addListener(this, &ftCore::setScale);
	
	parameters.add(opticalFlow.getParameters());
	parameters.add(velocityBridge.getParameters());
	parameters.add(densityBridge.getParameters());
	parameters.add(fluidSimulation.getParameters());
}

//--------------------------------------------------------------
void ftCore::setInput(ofTexture &_forVelocity, ofTexture &_forDensity) {
	opticalFlow.setInput(_forVelocity);
	densityBridge.setDensity(_forDensity);
}

//--------------------------------------------------------------
void ftCore::addFlow(flowTools::ftFlowType _type, ofTexture &_tex, float _strength) {
	switch (_type) {
		case FT_CORE_FLUID_DENSITY:
			addFluidDensity(_tex, _strength);
			break;
		case FT_CORE_FLUID_VELOCITY:
			addFluidVelocity(_tex, _strength);
			break;
		case FT_CORE_FLUID_TEMPERATURE:
			addFluidTemperature(_tex, _strength);
			break;
		case FT_CORE_FLUID_PRESSURE:
			addFluidPressure(_tex, _strength);
			break;
		case FT_CORE_OBSTACLE_TEMPORARY:
			addObstacleTemp(_tex);
		case FT_CORE_OBSTACLE_CONSTANT:
			addObstacleConst(_tex);
		default:
			break;
	}
}

//--------------------------------------------------------------
ofTexture& ftCore::getFlow(flowTools::ftFlowType _type) {
	
//	FT_CORE_INPUT,
//	FT_CORE_INPUT_DENSITY,
//	FT_CORE_INPUT_VELOCITY,
//	
//	FT_CORE_FLOW_VELOCITY,
//	FT_CORE_BRIDGE_DENSITY,
//	FT_CORE_BRIDGE_VELOCITY,
//	FT_CORE_BRIDGE_TEMPERATURE,
//	FT_CORE_BRIDGE_PRESSURE,
//	FT_CORE_FLUID_DENSITY,
//	FT_CORE_FLUID_VELOCITY,
//	FT_CORE_FLUID_TEMPERATURE,
//	FT_CORE_FLUID_PRESSURE,
//	FT_CORE_OBSTACLE_TEMPORARY,
//	FT_CORE_OBSTACLE_CONSTANT,
//	
//	switch (_type) {
////		case FT_CORE_INPUT
////			break;
////		case FT_CORE_INPUT_DENSITY:
////			break;
////		case FT_CORE_FLUID_VELOCITY:
////			break;
//		case FT_CORE_FLUID_TEMPERATURE:
//			
//			break;
//		case FT_CORE_FLUID_PRESSURE:
//			
//			break;
//		case FT_CORE_OBSTACLE_TEMPORARY:
//			
//		case FT_CORE_OBSTACLE_CONSTANT:
//			
//		default:
//			return ofTexture();
//			break;
//	}
}

//--------------------------------------------------------------
void ftCore::update(float _deltaTime){
	if (_deltaTime < 0) {
		deltaTime = ofGetElapsedTimef() - lastTime;
		lastTime = ofGetElapsedTimef();
	} else {
		deltaTime = _deltaTime;
	}
	
//	float dt = min(deltaTime, 1.f / 30.f);
	float dt = 1.0 / ofGetFrameRate();
	
	opticalFlow.update();
	
	velocityBridge.setSource(opticalFlow.getTexture());
	densityBridge.setVelocity(opticalFlow.getTexture());
	
	velocityBridge.update(dt);
	densityBridge.update(dt);
	
	fluidSimulation.addVelocity(velocityBridge.getTexture());
	fluidSimulation.addDensity(densityBridge.getTexture());
	fluidSimulation.addTemperature(densityBridge.getLuminanceMask());
	
	fluidSimulation.update(dt);
}

//--------------------------------------------------------------
void ftCore::keyPressed(ofKeyEventArgs &_key){
	switch (_key.key) {
		case '1': drawMode.set(FT_CORE_DRAW_FLUID_DENSITY); break;
		case '2': drawMode.set(FT_CORE_DRAW_FLUID_FIELDS); break;
		case '3': drawMode.set(FT_CORE_DRAW_FLUID_VELOCITY); break;
		case '4': drawMode.set(FT_CORE_DRAW_FLUID_PRESSURE); break;
		case '5': drawMode.set(FT_CORE_DRAW_FLUID_TEMPERATURE); break;
		case '6': drawMode.set(FT_CORE_DRAW_FLUID_OBSTACLE); break;
		case '7': drawMode.set(FT_CORE_DRAW_OPTICAL_FLOW_MASK); break;
		case '8': drawMode.set(FT_CORE_DRAW_OPTICAL_FLOW_TRAIL); break;
		case '9': drawMode.set(FT_CORE_DRAW_OPTICAL_FLOW_VELOCITY); break;
		default: break;
	}
}

//--------------------------------------------------------------
void ftCore::drawModeSetName(int &_value) {
	switch(_value) {
		case FT_CORE_DRAW_FLUID_DENSITY:		drawName.set("Fluid Density  (1)");	break;
		case FT_CORE_DRAW_FLUID_FIELDS:			drawName.set("Fluid Fields   (2)");	break;
		case FT_CORE_DRAW_FLUID_VELOCITY:		drawName.set("Fluid Velocity (3)");	break;
		case FT_CORE_DRAW_FLUID_PRESSURE:		drawName.set("Fluid Pressure (4)");	break;
		case FT_CORE_DRAW_FLUID_TEMPERATURE:	drawName.set("Fld Temperature(5)");	break;
		case FT_CORE_DRAW_FLUID_DIVERGENCE: 	drawName.set("Fld Divergence    ");	break;
		case FT_CORE_DRAW_FLUID_VORTICITY:		drawName.set("Fluid Vorticity   ");	break;
		case FT_CORE_DRAW_FLUID_BUOYANCY:		drawName.set("Fluid Buoyancy    ");	break;
		case FT_CORE_DRAW_FLUID_OBSTACLE:		drawName.set("Fluid Obstacle (6)");	break;
		case FT_CORE_DRAW_OPTICAL_FLOW_MASK:	drawName.set("Flow Density   (7)");	break;
		case FT_CORE_DRAW_OPTICAL_FLOW_TRAIL:	drawName.set("Flow Velocity  (8)");	break;
		case FT_CORE_DRAW_OPTICAL_FLOW_VELOCITY:drawName.set("Optical Flow   (9)");	break;
	}
}

//--------------------------------------------------------------
void ftCore::draw(int _x, int _y, int _w, int _h, ofBlendMode _blendmode) {
	switch(drawMode.get()) {
		case FT_CORE_DRAW_FLUID_DENSITY:		drawFluidDensity(_x,_y,_w,_h, _blendmode); break;
		case FT_CORE_DRAW_FLUID_FIELDS:			drawFluidFields(_x,_y,_w,_h, _blendmode); break;
		case FT_CORE_DRAW_FLUID_VELOCITY:		drawFluidVelocity(_x,_y,_w,_h, _blendmode); break;
		case FT_CORE_DRAW_FLUID_PRESSURE:		drawFluidPressure(_x,_y,_w,_h, _blendmode); break;
		case FT_CORE_DRAW_FLUID_TEMPERATURE:	drawFluidTemperature(_x,_y,_w,_h, _blendmode); break;
		case FT_CORE_DRAW_FLUID_DIVERGENCE: 	drawFluidDivergence(_x,_y,_w,_h, _blendmode); break;
		case FT_CORE_DRAW_FLUID_VORTICITY:		drawFluidVorticity(_x,_y,_w,_h, _blendmode); break;
		case FT_CORE_DRAW_FLUID_BUOYANCY:		drawFluidBuoyance(_x,_y,_w,_h, _blendmode); break;
		case FT_CORE_DRAW_FLUID_OBSTACLE:		drawFluidObstacle(_x,_y,_w,_h, _blendmode); break;
		case FT_CORE_DRAW_OPTICAL_FLOW_MASK:	drawMask(_x,_y,_w,_h, _blendmode); break;
		case FT_CORE_DRAW_OPTICAL_FLOW_TRAIL: 	drawTrail(_x,_y,_w,_h, _blendmode); break;
		case FT_CORE_DRAW_OPTICAL_FLOW_VELOCITY:drawOpticalFlow(_x,_y,_w,_h, _blendmode); break;
	}
}

//--------------------------------------------------------------
void ftCore::drawFluidDensity(int _x, int _y, int _w, int _h, ofBlendMode _blendmode) {
	fluidSimulation.draw(_x, _y, _w, _h, _blendmode);
}

//--------------------------------------------------------------
void ftCore::drawFluidFields(int _x, int _y, int _w, int _h, ofBlendMode _blendmode) {
	pressureField.setPressure(fluidSimulation.getPressure());
	velocityTemperatureField.setVelocity(fluidSimulation.getVelocity());
	velocityTemperatureField.setTemperature(fluidSimulation.getTemperature());
	pressureField.draw(_x, _y, _w, _h);
	velocityTemperatureField.draw(_x, _y, _w, _h);
}

//--------------------------------------------------------------
void ftCore::drawFluidVelocity(int _x, int _y, int _w, int _h, ofBlendMode _blendmode) {
	if (showScalar.get()) {
		displayScalar.setSource(fluidSimulation.getVelocity());
		displayScalar.draw(_x, _y, _w, _h, _blendmode);	}
	if (showField.get()) {
		velocityField.setVelocity(fluidSimulation.getVelocity());
		velocityField.draw(_x, _y, _w, _h);	}
}

//--------------------------------------------------------------
void ftCore::drawFluidPressure(int _x, int _y, int _w, int _h, ofBlendMode _blendmode) {
	if (showScalar.get()) {
		displayScalar.setSource(fluidSimulation.getPressure());
		displayScalar.draw(_x, _y, _w, _h, _blendmode); }
	if (showField.get()) {
		pressureField.setPressure(fluidSimulation.getPressure());
		pressureField.draw(_x, _y, _w, _h);	}
}

//--------------------------------------------------------------
void ftCore::drawFluidTemperature(int _x, int _y, int _w, int _h, ofBlendMode _blendmode) {
	if (showScalar.get()) {
		displayScalar.setSource(fluidSimulation.getTemperature());
		displayScalar.draw(_x, _y, _w, _h, _blendmode);	}
	if (showField.get()) {
		temperatureField.setTemperature(fluidSimulation.getTemperature());
		temperatureField.draw(_x, _y, _w, _h);	}
}

//--------------------------------------------------------------
void ftCore::drawFluidDivergence(int _x, int _y, int _w, int _h, ofBlendMode _blendmode) {
	if (showScalar.get()) {
		displayScalar.setSource(fluidSimulation.getDivergence());
		displayScalar.draw(_x, _y, _w, _h, _blendmode);	}
	if (showField.get()) {
		temperatureField.setTemperature(fluidSimulation.getDivergence());
		temperatureField.draw(_x, _y, _w, _h);	}
}

//--------------------------------------------------------------
void ftCore::drawFluidVorticity(int _x, int _y, int _w, int _h, ofBlendMode _blendmode) {
	if (showScalar.get()) {
		displayScalar.setSource(fluidSimulation.getConfinement());
		displayScalar.draw(_x, _y, _w, _h, _blendmode);	}
	if (showField.get()) {
		velocityField.setVelocity(fluidSimulation.getConfinement());
		velocityField.draw(_x, _y, _w, _h);	}
}

//--------------------------------------------------------------
void ftCore::drawFluidBuoyance(int _x, int _y, int _w, int _h, ofBlendMode _blendmode) {
	if (showScalar.get()) {
		displayScalar.setSource(fluidSimulation.getSmokeBuoyancy());
		displayScalar.draw(_x, _y, _w, _h, _blendmode);	}
	if (showField.get()) {
		velocityField.setVelocity(fluidSimulation.getSmokeBuoyancy());
		velocityField.draw(_x, _y, _w, _h);	}
}

//--------------------------------------------------------------
void ftCore::drawFluidObstacle(int _x, int _y, int _w, int _h, ofBlendMode _blendmode) {
	fluidSimulation.getObstacle().draw(_x, _y, _w, _h);
}

//--------------------------------------------------------------
void ftCore::drawMask(int _x, int _y, int _w, int _h, ofBlendMode _blendmode) {
	densityBridge.draw(_x, _y, _w, _h, _blendmode);
	
}

//--------------------------------------------------------------
void ftCore::drawTrail(int _x, int _y, int _w, int _h, ofBlendMode _blendmode) {
	if (showScalar.get()) {
		displayScalar.setSource(velocityBridge.getTexture());
		displayScalar.draw(_x, _y, _w, _h, _blendmode); }
	if (showField.get()) {
		velocityField.setVelocity(velocityBridge.getTexture());
		velocityField.draw(_x, _y, _w, _h); }
}

//--------------------------------------------------------------
void ftCore::drawOpticalFlow(int _x, int _y, int _w, int _h, ofBlendMode _blendmode) {
	if (showScalar.get()) {
		displayScalar.setSource(opticalFlow.getTexture());
		displayScalar.draw(_x, _y, _w, _h, _blendmode); }
	if (showField.get()) {
		velocityField.setVelocity(opticalFlow.getTexture());
		velocityField.draw(_x, _y, _w, _h); }
	}
