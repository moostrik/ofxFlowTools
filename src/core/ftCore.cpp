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
	// draw fields all on 256th resolution
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
	parameters.add(drawMode.set("draw mode", FT_FLUID_DENSITY, FT_FLOW_VELOCITY, FT_FLUID_DENSITY));
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
	
	drawMode.addListener(this, &ftCore::drawModeListener);
	showScalar.addListener(this, &ftCore::showScalarListener);
	showField.addListener(this, &ftCore::showFieldListener);
	visualizeScale.addListener(this, &ftCore::visualizeScaleListener);
	
	parameters.add(opticalFlow.getParameters());
	parameters.add(velocityBridge.getParameters());
	parameters.add(densityBridge.getParameters());
	parameters.add(fluidSimulation.getParameters());
}

//--------------------------------------------------------------
void ftCore::update(float _deltaTime){
	float dt = (_deltaTime != -1)? _deltaTime : 1.0 / max(ofGetFrameRate(), 1.f);
	
	opticalFlow.update();
	
	velocityBridge.setInput(opticalFlow.getVelocity());
	densityBridge.setVelocity(opticalFlow.getVelocity());
	
	velocityBridge.update(dt);
	densityBridge.update(dt);
	
	fluidSimulation.addVelocity(velocityBridge.getVelocity());
	fluidSimulation.addDensity(densityBridge.getDensity());
	fluidSimulation.addTemperature(densityBridge.getLuminance());
	
	fluidSimulation.update(dt);
}

//--------------------------------------------------------------
void ftCore::setFlow(flowTools::ftFlowType _type, ofTexture &_tex) {
	switch (_type) {
		case FT_INPUT:  			setFlow(FT_INPUT_FOR_VELOCITY, _tex); setFlow(FT_INPUT_FOR_DENSITY, _tex); break;
		case FT_INPUT_FOR_VELOCITY: setInputForVelocity(_tex); break;
		case FT_INPUT_FOR_DENSITY:  setInputForDensity(_tex); break;
		case FT_FLOW_VELOCITY:  	setFlowVelocity(_tex); break;
		case FT_BRIDGE_VELOCITY:  	setBridgeVelocity(_tex); break;
		case FT_BRIDGE_DENSITY:  	setBridgeDensity(_tex); break;
//		case FT_BRIDGE_TEMPERATURE: setBridgeTemperature(_tex); break;
//		case FT_BRIDGE_PRESSURE:  	setBridgePressure(_tex); break;
//		case FT_OBSTACLE_TEMPORARY: setObstacleTemporary(_tex); break;
//		case FT_OBSTACLE_CONSTANT:  setObstacleConstant(_tex); break;
//		case FT_FLUID_BUOYANCY: 	setFluidBuoyancy(_tex); break;
//		case FT_FLUID_VORTICITY: 	setFluidVorticity(_tex); break;
//		case FT_FLUID_DIVERGENCE: 	setFluidDivergence(_tex); break;
//		case FT_FLUID_TEMPERATURE: 	setFluidTemperature(_tex); break;
//		case FT_FLUID_PRESSURE: 	setFluidPressure(_tex); break;
//		case FT_FLUID_VELOCITY: 	setFluidVelocity(_tex); break;
//		case FT_FLUID_DENSITY: 		setFluidDensity(_tex); break;
		default:
			ofLogWarning("ftCore: addFlow") << "no method to set flow of type " << ftFlowNames[_type];
			break;
	}
}

//--------------------------------------------------------------
void ftCore::addFlow(flowTools::ftFlowType _type, ofTexture &_tex, float _strength) {
	switch (_type) {
//		case FT_FLOW_VELOCITY:  	addFlowVelocity(_tex); break;
//		case FT_BRIDGE_VELOCITY:  	addBridgeVelocity(_tex); break;
//		case FT_BRIDGE_DENSITY:  	addBridgeDensity(_tex); break;
//		case FT_BRIDGE_TEMPERATURE: addBridgeTemperature(_tex); break;
//		case FT_BRIDGE_PRESSURE:  	addBridgePressure(_tex); break;
		case FT_OBSTACLE_TEMPORARY: addObstacleTemporary(_tex); break;
		case FT_OBSTACLE_CONSTANT:  addObstacleConstant(_tex); break;
//		case FT_FLUID_BUOYANCY: 	addFluidBuoyancy(_tex); break;
//		case FT_FLUID_VORTICITY: 	addFluidVorticity(_tex); break;
//		case FT_FLUID_DIVERGENCE: 	addFluidDivergence(_tex); break;
		case FT_FLUID_TEMPERATURE: 	addFluidTemperature(_tex); break;
		case FT_FLUID_PRESSURE: 	addFluidPressure(_tex); break;
		case FT_FLUID_VELOCITY: 	addFluidVelocity(_tex); break;
		case FT_FLUID_DENSITY: 		addFluidDensity(_tex); break;
		default:
			ofLogWarning("ftCore: addFlow") << "no method to add flow of type " << ftFlowNames[_type];
			break;
	}
}

//--------------------------------------------------------------
ofTexture& ftCore::getFlow(flowTools::ftFlowType _type) {
	
	string dinges = ftFlowNames[FT_OBSTACLE_TEMPORARY];
	switch (_type) {
//		case FT_INPUT_VELOCITY:  	getInputVelocity(); break;
//		case FT_INPUT_DENSITY:  	getInputDensity(); break;
		case FT_FLOW_VELOCITY:  	getOpticalFlowVelocity(); break;
		case FT_BRIDGE_VELOCITY:  	getBridgeVelocity(); break;
		case FT_BRIDGE_DENSITY:  	getBridgeDensity(); break;
//		case FT_BRIDGE_TEMPERATURE: getBridgeTemperature(); break;
//		case FT_BRIDGE_PRESSURE:  	getBridgePressure(); break;
//		case FT_OBSTACLE_TEMPORARY: getObstacleTemporary(); break;
//		case FT_OBSTACLE_CONSTANT:  getObstacleConstant(); break;
		case FT_FLUID_BUOYANCY: 	getFluidBuoyancy(); break;
		case FT_FLUID_VORTICITY: 	getFluidVorticityVelocity(); break;
		case FT_FLUID_DIVERGENCE: 	getFluidDivergence(); break;
		case FT_FLUID_TEMPERATURE: 	getFluidTemperature(); break;
		case FT_FLUID_PRESSURE: 	getFluidPressure(); break;
		case FT_FLUID_VELOCITY: 	getFluidVelocity(); break;
		case FT_FLUID_DENSITY: 		getFluidDensity(); break;
		default:
			ofLogWarning("ftCore: addFlow") << "no method to get flow of type " << ftFlowNames[_type];
			break;
	}
}

//--------------------------------------------------------------
void ftCore::draw(ftFlowType _type, int _x, int _y, int _w, int _h, ofBlendMode _blendmode) {
	switch(_type) {
		case FT_INPUT: 				draw(FT_INPUT_FOR_VELOCITY, _x, _y, _w, _h, _blendmode); break;
		case FT_INPUT_FOR_VELOCITY: drawTex(opticalFlow.getInput(), _x, _y, _w, _h, _blendmode); break;
		case FT_INPUT_FOR_DENSITY: 	drawTex(opticalFlow.getInput(), _x, _y, _w, _h, _blendmode); break;
		case FT_FLOW_VELOCITY: 		displayScalar.draw(opticalFlow.getVelocity(), _x, _y, _w, _h, _blendmode);
									velocityField.draw(opticalFlow.getVelocity(), _x, _y, _w, _h, _blendmode); break;
		case FT_BRIDGE_VELOCITY: 	displayScalar.draw(velocityBridge.getOutput(), _x, _y, _w, _h, _blendmode);
									velocityField.draw(velocityBridge.getOutput(), _x, _y, _w, _h, _blendmode); break;
		case FT_BRIDGE_DENSITY: 	drawTex(densityBridge.getVisible(), _x, _y, _w, _h, _blendmode); break;
		case FT_BRIDGE_TEMPERATURE: break;
		case FT_BRIDGE_PRESSURE: 	break;
		case FT_OBSTACLE_TEMPORARY:
		case FT_OBSTACLE_CONSTANT: 	drawTex(fluidSimulation.getObstacle(), _x, _y, _w, _h, _blendmode); break;
		case FT_FLUID_BUOYANCY: 	displayScalar.draw(fluidSimulation.getBuoyancy(), _x, _y, _w, _h, _blendmode);
									velocityField.draw(fluidSimulation.getBuoyancy(), _x, _y, _w, _h); break;
		case FT_FLUID_VORTICITY: 	displayScalar.draw(fluidSimulation.getConfinement(), _x, _y, _w, _h, _blendmode);
									velocityField.draw(fluidSimulation.getConfinement(), _x, _y, _w, _h, _blendmode); break;
		case FT_FLUID_DIVERGENCE: 	displayScalar.draw(fluidSimulation.getDivergence(), _x, _y, _w, _h, _blendmode);
									temperatureField.draw(fluidSimulation.getDivergence(), _x, _y, _w, _h, _blendmode); break;
		case FT_FLUID_TEMPERATURE: 	displayScalar.draw(fluidSimulation.getTemperature(), _x, _y, _w, _h, _blendmode);
									temperatureField.draw(fluidSimulation.getTemperature(), _x, _y, _w, _h, _blendmode); break;
		case FT_FLUID_PRESSURE: 	displayScalar.draw(fluidSimulation.getPressure(), _x, _y, _w, _h, _blendmode);
									pressureField.draw(fluidSimulation.getPressure(), _x, _y, _w, _h, _blendmode); break;
		case FT_FLUID_VELOCITY: 	displayScalar.draw(fluidSimulation.getVelocity(), _x, _y, _w, _h, _blendmode);
									velocityField.draw(fluidSimulation.getVelocity(), _x, _y, _w, _h, _blendmode); break;
		case FT_FLUID_DENSITY: 		fluidSimulation.draw(_x, _y, _w, _h, _blendmode); break;
		case FT_NONE:
		default: break;
	}
}

//--------------------------------------------------------------
void ftCore::drawTex(ofTexture &_tex, int _x, int _y, int _w, int _h, ofBlendMode _blendMode) {
	ofPushStyle();
	ofEnableBlendMode(_blendMode);
	_tex.draw(_x, _y, _w, _h);
	ofPopStyle();
}

//--------------------------------------------------------------
void ftCore::drawModeListener(int &_value) {
	drawName.set(ftFlowNames[_value]);
	
//	switch(_value) {
//		case FT_INPUT_DENSITY:		drawName.set("Input Density  (1)"); break;
//		case FT_INPUT_VELOCITY:		drawName.set("Input Velocity"); 	break;
//		case FT_FLOW_VELOCITY:		drawName.set("Optical Flow   (2)"); break;
//		case FT_BRIDGE_VELOCITY:	drawName.set("Bridge Velocity(3)");	break;
//		case FT_BRIDGE_DENSITY:		drawName.set("Bridge Density (4)");	break;
//		case FT_BRIDGE_TEMPERATURE:	drawName.set("Br. Temperature");	break;
//		case FT_BRIDGE_PRESSURE:	drawName.set("Bridge Pressure");	break;
//		case FT_OBSTACLE_TEMPORARY:	drawName.set("Obst. Temporary");	break;
//		case FT_OBSTACLE_CONSTANT:	drawName.set("Obst. Constant (6)");	break;
//		case FT_FLUID_BUOYANCY:		drawName.set("Fluid Buoyancy");		break;
//		case FT_FLUID_VORTICITY:	drawName.set("Fl. Vorticity  ");	break;
//		case FT_FLUID_DIVERGENCE:	drawName.set("Fl. Divergence (6)");	break;
//		case FT_FLUID_TEMPERATURE:	drawName.set("Fl. Temperature(7)");	break;
//		case FT_FLUID_PRESSURE:		drawName.set("Fluid Pressure (8)");	break;
//		case FT_FLUID_VELOCITY:		drawName.set("Fluid Velocity (9)");	break;
//		case FT_FLUID_DENSITY:		drawName.set("Fluid Density  (0)");	break;
//	}
}

//--------------------------------------------------------------
void ftCore::keyPressed(ofKeyEventArgs &_key){
	switch (_key.key) {
		case '1': drawMode.set(FT_INPUT); break;
		case '2': drawMode.set(FT_FLOW_VELOCITY); break;
		case '3': drawMode.set(FT_BRIDGE_VELOCITY); break;
		case '4': drawMode.set(FT_BRIDGE_DENSITY); break;
		case '5': drawMode.set(FT_OBSTACLE_CONSTANT); break;
		case '6': drawMode.set(FT_FLUID_DIVERGENCE); break;
		case '7': drawMode.set(FT_FLUID_TEMPERATURE); break;
		case '8': drawMode.set(FT_FLUID_PRESSURE); break;
		case '9': drawMode.set(FT_FLUID_VELOCITY); break;
		case '0': drawMode.set(FT_FLUID_DENSITY); break;
		default: break;
	}
}

//--------------------------------------------------------------
void ftCore::showScalarListener(bool &_value) {
	displayScalar.setActive(_value);
}

//--------------------------------------------------------------
void ftCore::showFieldListener(bool &_value) {
	velocityField.setActive(_value);
	velocityTemperatureField.setActive(_value);
	temperatureField.setActive(_value);
	velocityTemperatureField.setActive(_value);
	pressureField.setActive(_value);
}

//--------------------------------------------------------------
void ftCore::visualizeScaleListener(float& _value)	{
	displayScalar.setScale(_value);
	velocityField.setVelocityScale(_value);
	velocityTemperatureField.setVelocityScale(_value);
	temperatureField.setScale(_value);
	velocityTemperatureField.setTemperatureScale(_value);
	pressureField.setScale(_value);
}
