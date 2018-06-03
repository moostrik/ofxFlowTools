
#pragma once
	
#include "ofMain.h"
#include "ofxFlowTools.h"
	
	using namespace flowTools;
	
	enum drawModeEnum{
		FT_CORE_DRAW_FLUID_DENSITY = 0,
		FT_CORE_DRAW_FLUID_FIELDS,
		FT_CORE_DRAW_FLUID_VELOCITY,
		FT_CORE_DRAW_FLUID_PRESSURE,
		FT_CORE_DRAW_FLUID_TEMPERATURE,
		FT_CORE_DRAW_FLUID_DIVERGENCE,
		FT_CORE_DRAW_FLUID_VORTICITY,
		FT_CORE_DRAW_FLUID_BUOYANCY,
		FT_CORE_DRAW_FLUID_OBSTACLE,
		FT_CORE_DRAW_OPTICAL_FLOW_MASK,
		FT_CORE_DRAW_OPTICAL_FLOW_TRAIL,
		FT_CORE_DRAW_OPTICAL_FLOW_VELOCITY,
		FT_CORE_DRAW_RESERVED
	};


class ftCore {
public:
	ftCore();
	~ftCore();
	
	void	setup(int _densityWidth = 1280, int _densityHeight = 720, int _flowWidth = 0, int _flowHeight = 0 );
	void	setInput(ofTexture &_texture) { setInput(_texture, _texture); }
	void	setInput(ofTexture &_forVelocity, ofTexture &_forDensity);
	void	update(float _deltaTime = -1);
	void	draw(int _x, int _y, int _w, int _h, ofBlendMode _blendmode);
	
	void	reset() 	{ velocityBridge.reset(); densityBridge.reset(); fluidSimulation.reset(); }
	void	keyPressed(ofKeyEventArgs & key);
	
	int		getDensityWidth() 	{ return densityWidth; }
	int		getDensityHeight() 	{ return densityHeight; }
	int		getFlowWidth() 		{ return flowWidth; }
	int		getFlowHeight() 	{ return flowHeight; }
	int		getFieldWidth() 	{ return fieldWidth; }
	int		getFieldHeight() 	{ return fieldHeight; }
	
	void 		addFlow(ftFlowType _type, ofTexture& _tex, float _strength  = 1.0);
	
	void 		setFlow(ftFlowType _type, ofTexture& _tex);
	
	void	addFluidDensity(ofTexture& _tex, float _strength  = 1.0) 	{ fluidSimulation.addDensity(_tex, _strength); }
	void	addFluidVelocity(ofTexture& _tex, float _strength  = 1.0)	{ fluidSimulation.addVelocity(_tex, _strength); }
	void	addFluidTemperature(ofTexture& _tex, float _strength  = 1.0){ fluidSimulation.addTemperature(_tex, _strength); }
	void	addFluidPressure(ofTexture& _tex, float _strength  = 1.0)	{ fluidSimulation.addPressure(_tex, _strength); }
	void	addObstacleConst(ofTexture& _tex)							{ fluidSimulation.addObstacle(_tex); }
	void	addObstacleTemp(ofTexture& _tex)							{ fluidSimulation.addTempObstacle(_tex); }

	ofTexture&	getFlow(ftFlowType _type);
	ofTexture&	getOpticalFlowVelocity(){ return opticalFlow.getVelocity(); }
	ofTexture&	getBridgeDensity()		{ return densityBridge.getDensity(); }
	ofTexture&	getBridgeVelocity()		{ return velocityBridge.getVelocity(); }
//	ofTexture&	getBridgePressure()		{ return ftPressureBridge.getVelocity(); }
//	ofTexture&	getBridgeTemperature()	{ return ftTemperatureBridge.getVelocity(); }
	
	ofTexture&	getFluidDensity()		{ return fluidSimulation.getDensity(); }
	ofTexture&	getFluidVelocity()		{ return fluidSimulation.getVelocity(); }
	ofTexture&	getFluidPressure()		{ return fluidSimulation.getPressure(); }
	ofTexture&	getFluidTemperature()	{ return fluidSimulation.getTemperature(); }
	ofTexture&	getFluidDivergence()	{ return fluidSimulation.getDivergence(); }
	ofTexture&	getFluidCurlMagnitude()	{ return fluidSimulation.getCurlMagnitude(); }
	ofTexture&	getFluidConfinement()	{ return fluidSimulation.getConfinement(); }
	ofTexture&	getFluidSmokeBuoyancy()	{ return fluidSimulation.getSmokeBuoyancy(); }
	
//	ofTexture&	getObstacleTemporary()	{ return fluidSimulation.getObstacleTemporary(); }
//	ofTexture&	getObstacleConstant()	{ return fluidSimulation.getObstacleConstgant(); }
	
	
	ofParameterGroup& getParameters() 				{ return parameters; }
	ofParameterGroup& getCoreParameters() 			{ return flowCoreParameters; }
	ofParameterGroup& getOpticalFlowParameters() 	{ return opticalFlow.getParameters(); }
	ofParameterGroup& getVelocityBridgeParameters() { return velocityBridge.getParameters(); }
	ofParameterGroup& getDensityBridgeParameters() 	{ return densityBridge.getParameters(); }
	ofParameterGroup& getFluidSimulationParameters(){ return fluidSimulation.getParameters(); }
	
	ofParameterGroup	parameters;
private:
	// Time
	float				lastTime;
	float				deltaTime;
	
	// FlowTools
	int					densityWidth;
	int					densityHeight;
	int					flowWidth;
	int					flowHeight;
	int					fieldWidth;
	int					fieldHeight;
	
	ftOpticalFlow		opticalFlow;
	ftVelocityBridge	velocityBridge;
	ftDensityBridge		densityBridge;
	ftFluidSimulation	fluidSimulation;
	
	// Visualisations
	ofParameterGroup	flowCoreParameters;
	ofParameterGroup	visualizeParameters;
	ftDisplayScalar		displayScalar;
	ftVelocityField		velocityField;
	ftTemperatureField	temperatureField;
	ftPressureField		pressureField;
	ftVTField			velocityTemperatureField;
	
	ofParameter<bool>	showScalar;
	ofParameter<bool>	showField;
	ofParameter<float>	visualizeScale;
	void				setScale(float& _value)	{ 	displayScalar.setScale(_value);
													velocityField.setVelocityScale(_value);
													velocityTemperatureField.setVelocityScale(_value);
													temperatureField.setScale(_value);
													velocityTemperatureField.setTemperatureScale(_value);
													pressureField.setScale(_value); }
	
	// Parameters
	void				setupParameters();
	
	// DRAW
	ofParameter<int>	drawMode;
	void				drawModeSetName(int& _value) ;
	ofParameter<string> drawName;
	
	void				drawComposite(int _x, int _y, int _w, int _h, ofBlendMode _blendmode = OF_BLENDMODE_ALPHA);
	void				drawFluidFields(int _x, int _y, int _w, int _h, ofBlendMode _blendmode = OF_BLENDMODE_ALPHA);
	
	void				drawFluidDensity(int _x, int _y, int _w, int _h, ofBlendMode _blendmode = OF_BLENDMODE_ALPHA);
	void				drawFluidVelocity(int _x, int _y, int _w, int _h, ofBlendMode _blendmode = OF_BLENDMODE_ALPHA);
	void				drawFluidPressure(int _x, int _y, int _w, int _h, ofBlendMode _blendmode = OF_BLENDMODE_ALPHA);
	void				drawFluidTemperature(int _x, int _y, int _w, int _h, ofBlendMode _blendmode = OF_BLENDMODE_ALPHA);
	void				drawFluidDivergence(int _x, int _y, int _w, int _h, ofBlendMode _blendmode = OF_BLENDMODE_ALPHA);
	void				drawFluidVorticity(int _x, int _y, int _w, int _h, ofBlendMode _blendmode = OF_BLENDMODE_ALPHA);
	void				drawFluidBuoyance(int _x, int _y, int _w, int _h, ofBlendMode _blendmode = OF_BLENDMODE_ALPHA);
	void				drawFluidObstacle(int _x, int _y, int _w, int _h, ofBlendMode _blendmode = OF_BLENDMODE_ALPHA);
	
	void				drawMask(int _x, int _y, int _w, int _h, ofBlendMode _blendmode = OF_BLENDMODE_ALPHA);
	
	void				drawTrail(int _x, int _y, int _w, int _h, ofBlendMode _blendmode = OF_BLENDMODE_ALPHA);
	void				drawOpticalFlow(int _x, int _y, int _w, int _h, ofBlendMode _blendmode = OF_BLENDMODE_ALPHA);
};

