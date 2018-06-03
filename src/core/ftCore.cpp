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
	showScalar.addListener(this, &ftCore::showScalarListener);
	showField.addListener(this, &ftCore::showFieldListener);
	visualizeScale.addListener(this, &ftCore::visualizeScaleListener);
	
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
	float dt = (_deltaTime != -1)? _deltaTime : 1.0 / max(ofGetFrameRate(), 1.f);
	
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
void ftCore::draw(int _x, int _y, int _w, int _h, ofBlendMode _blendmode) {
	switch(drawMode.get()) {
		case FT_CORE_DRAW_FLUID_DENSITY:
			fluidSimulation.draw(_x, _y, _w, _h, _blendmode);
			break;
		case FT_CORE_DRAW_FLUID_FIELDS:
			pressureField.draw(fluidSimulation.getPressure(), _x, _y, _w, _h);
			velocityTemperatureField.draw(fluidSimulation.getVelocity(), fluidSimulation.getTemperature(), _x, _y, _w, _h, _blendmode);
			break;
		case FT_CORE_DRAW_FLUID_VELOCITY:
			displayScalar.draw(fluidSimulation.getVelocity(), _x, _y, _w, _h, _blendmode);
			velocityField.draw(fluidSimulation.getVelocity(), _x, _y, _w, _h, _blendmode);
			break;
		case FT_CORE_DRAW_FLUID_PRESSURE:
			displayScalar.draw(fluidSimulation.getPressure(), _x, _y, _w, _h, _blendmode);
			pressureField.draw(fluidSimulation.getPressure(), _x, _y, _w, _h, _blendmode);
			break;
		case FT_CORE_DRAW_FLUID_TEMPERATURE:
			displayScalar.draw(fluidSimulation.getTemperature(), _x, _y, _w, _h, _blendmode);
			temperatureField.draw(fluidSimulation.getTemperature(), _x, _y, _w, _h, _blendmode);
			break;
		case FT_CORE_DRAW_FLUID_DIVERGENCE:
			displayScalar.draw(fluidSimulation.getDivergence(), _x, _y, _w, _h, _blendmode);
			temperatureField.draw(fluidSimulation.getDivergence(), _x, _y, _w, _h, _blendmode);
			break;
		case FT_CORE_DRAW_FLUID_VORTICITY:
			displayScalar.draw(fluidSimulation.getConfinement(), _x, _y, _w, _h, _blendmode);
			velocityField.draw(fluidSimulation.getConfinement(), _x, _y, _w, _h, _blendmode);
			break;
		case FT_CORE_DRAW_FLUID_BUOYANCY:
			displayScalar.draw(fluidSimulation.getSmokeBuoyancy(), _x, _y, _w, _h, _blendmode);
			velocityField.draw(fluidSimulation.getSmokeBuoyancy(), _x, _y, _w, _h);
			break;
		case FT_CORE_DRAW_FLUID_OBSTACLE:
			fluidSimulation.getObstacle().draw(_x, _y, _w, _h);
			break;
		case FT_CORE_DRAW_OPTICAL_FLOW_MASK:
			densityBridge.draw(_x, _y, _w, _h, _blendmode);
			break;
		case FT_CORE_DRAW_OPTICAL_FLOW_TRAIL:
			displayScalar.draw(velocityBridge.getTexture(), _x, _y, _w, _h, _blendmode);
			velocityField.draw(velocityBridge.getTexture(), _x, _y, _w, _h, _blendmode);
			break;
		case FT_CORE_DRAW_OPTICAL_FLOW_VELOCITY:
			displayScalar.draw(opticalFlow.getTexture(), _x, _y, _w, _h, _blendmode);
			velocityField.draw(opticalFlow.getTexture(), _x, _y, _w, _h, _blendmode);
			break;
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
