#include "ftCore.h"


ftCore::ftCore() {
	ofAddListener(ofEvents().keyPressed, this, &ftCore::keyPressed);
}

ftCore::~ftCore() {
	ofRemoveListener(ofEvents().keyPressed, this, &ftCore::keyPressed);
}

//--------------------------------------------------------------
void ftCore::setup(int _densityW, int _densityH, int _flowW, int _flowH, int _fieldW, int _fieldH){
	
	densityWidth = _densityW;
	densityHeight = _densityH;
	// process all but the density on 16th resolution
	flowWidth = _flowW;
	flowHeight = _flowH;
	// draw fields all on 256th resolution
	fieldWidth = _fieldW;
	fieldHeight = _fieldH;
		
	// CORE FLOW
	opticalFlow.setup(flowWidth, flowHeight);
	velocityBridge.setup(flowWidth, flowHeight);
	densityBridge.setup(flowWidth, flowHeight, densityWidth, densityHeight);
	fluidSimulation.setup(flowWidth, flowHeight, densityWidth, densityHeight);
	
	flows.push_back(&opticalFlow);
	flows.push_back(&velocityBridge);
	flows.push_back(&densityBridge);
	flows.push_back(&fluidSimulation);
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
	visualizeParameters.add(visualizeScale.set("scale", 0.3, 0.1, 3.0));
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
	updateOpticalFlow();
	updateBridgeVelocity(dt);
	updateBridgeDensity(dt);
	updateFluid(dt);
}

void ftCore::updateOpticalFlow(){
	opticalFlow.update();
}

void ftCore::updateBridgeVelocity(float _deltaTime){
	velocityBridge.setInput(opticalFlow.getVelocity());
	velocityBridge.update(_deltaTime);
}

void ftCore::updateBridgeDensity(float _deltaTime){
	densityBridge.setVelocity(opticalFlow.getVelocity());
	densityBridge.update(_deltaTime);
}

void ftCore::updateFluid(float _deltaTime){
	fluidSimulation.addVelocity(velocityBridge.getVelocity());
	fluidSimulation.addDensity(densityBridge.getDensity());
	fluidSimulation.addTemperature(densityBridge.getLuminance());
	
	fluidSimulation.update(_deltaTime);
}

//--------------------------------------------------------------
void ftCore::setFlow(flowTools::ftFlowType _type, ofTexture &_tex) {
	switch (_type) {
		case FT_INPUT_FOR_VELOCITY: setInputForVelocity(_tex); break;
		case FT_INPUT_FOR_DENSITY:  setInputForDensity(_tex); break;
		case FT_FLOW_VELOCITY:  	setOpticalFlowVelocity(_tex); break;
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
void ftCore::draw(ftFlowType _type, int _x, int _y, int _w, int _h) {
	switch(_type) {
		case FT_INPUT_FOR_VELOCITY: opticalFlow.drawInput(_x, _y, _w, _h); break;
		case FT_INPUT_FOR_DENSITY: 	densityBridge.drawInput(_x, _y, _w, _h); break;
		case FT_FLOW_VELOCITY: 		opticalFlow.draw(_x, _y, _w, _h); break;
		case FT_BRIDGE_VELOCITY: 	velocityBridge.draw(_x, _y, _w, _h); break;
		case FT_BRIDGE_DENSITY: 	densityBridge.draw(_x, _y, _w, _h); break;
		case FT_BRIDGE_TEMPERATURE: break;
		case FT_BRIDGE_PRESSURE: 	break;
		case FT_OBSTACLE_TEMPORARY: break;
		case FT_OBSTACLE_CONSTANT: 	fluidSimulation.drawObstacles(_x, _y, _w, _h); break;
		case FT_FLUID_BUOYANCY: 	fluidSimulation.drawBuoyancy(_x, _y, _w, _h); break;
		case FT_FLUID_VORTICITY: 	fluidSimulation.drawVorticityVelocity(_x, _y, _w, _h); break;
		case FT_FLUID_DIVERGENCE: 	fluidSimulation.drawDivergence(_x, _y, _w, _h); break;
		case FT_FLUID_TEMPERATURE: 	fluidSimulation.drawTemperature(_x, _y, _w, _h); break;
		case FT_FLUID_PRESSURE: 	fluidSimulation.drawPressure(_x, _y, _w, _h); break;
		case FT_FLUID_VELOCITY: 	fluidSimulation.drawVelocity(_x, _y, _w, _h); break;
		case FT_FLUID_DENSITY: 		fluidSimulation.draw(_x, _y, _w, _h); break;
		case FT_NONE:
		default: break;
	}
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
		case '1': drawMode.set(FT_INPUT_FOR_DENSITY); break;
		case '2': drawMode.set(FT_INPUT_FOR_VELOCITY); break;
		case '3': drawMode.set(FT_FLOW_VELOCITY); break;
		case '4': drawMode.set(FT_BRIDGE_VELOCITY); break;
		case '5': drawMode.set(FT_BRIDGE_DENSITY); break;
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
	for (auto flow : flows) {
		flow->setDrawField(_value);
	}
	velocityField.setActive(_value);
	velocityTemperatureField.setActive(_value);
	temperatureField.setActive(_value);
	velocityTemperatureField.setActive(_value);
	pressureField.setActive(_value);
}

//--------------------------------------------------------------
void ftCore::visualizeScaleListener(float& _value)	{
	for (auto flow : flows) {
		flow->setDrawScale(_value);
	}
	displayScalar.setScale(_value);
	velocityField.setVelocityScale(_value);
	velocityTemperatureField.setVelocityScale(_value);
	temperatureField.setScale(_value);
	velocityTemperatureField.setTemperatureScale(_value);
	pressureField.setScale(_value);
}
