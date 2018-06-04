
#pragma once
	
#include "ofMain.h"
#include "ofxFlowTools.h"
	
using namespace flowTools;

class ftCore {
public:
	ftCore();
	~ftCore();
	
	void	setup(int _densityWidth = 1280, int _densityHeight = 720, int _flowWidth = 0, int _flowHeight = 0 );
	void	update(float _deltaTime = -1);
	void	draw(int _x, int _y, int _w, int _h, ofBlendMode _blendmode) { draw(ftFlowType(drawMode.get()), _x, _y, _w, _h, _blendmode); }
	void	draw(ftFlowType _type, int _x, int _y, int _w, int _h, ofBlendMode _blendmode);
	
	void	reset() 	{ velocityBridge.reset(); densityBridge.reset(); fluidSimulation.reset(); }
	void	keyPressed(ofKeyEventArgs & key);
	
	int		getDensityWidth() 	{ return densityWidth; }
	int		getDensityHeight() 	{ return densityHeight; }
	int		getFlowWidth() 		{ return flowWidth; }
	int		getFlowHeight() 	{ return flowHeight; }
	int		getFieldWidth() 	{ return fieldWidth; }
	int		getFieldHeight() 	{ return fieldHeight; }
	
	
	void 	setFlow(ftFlowType _type, ofTexture& _tex);
	void	setInput(ofTexture &_texture) 					{ setInputForVelocity(_texture), setInputForDensity(_texture); }
	void	setInputForVelocity(ofTexture &_forVelocity) 	{ opticalFlow.setInput(_forVelocity); }
	void	setInputForDensity(ofTexture &_forDensity)		{ densityBridge.setDensity(_forDensity); }
	void 	setFlowVelocity(ofTexture &_tex)				{ ; }
	void 	setBridgeVelocity(ofTexture &_tex)				{ ; }
	void 	setBridgeDensity(ofTexture &_tex)				{ ; }
	void 	setBridgeTemperature(ofTexture &_tex)			{ ; }
	void 	setBridgePressure(ofTexture &_tex)				{ ; }
	void 	setObstacleTemporary(ofTexture &_tex)			{ ; }
	void 	setObstacleConstant(ofTexture &_tex)			{ ; }
	void 	setFluidBuoyancy(ofTexture &_tex)				{ ; }
	void 	setFluidVorticity(ofTexture &_tex)				{ ; }
	void 	setFluidDivergence(ofTexture &_tex)				{ ; }
	void 	setFluidTemperature(ofTexture &_tex)			{ ; }
	void 	setFluidPressure(ofTexture &_tex)				{ ; }
	void 	setFluidVelocity(ofTexture &_tex)				{ ; }
	void 	setFluidDensity(ofTexture &_tex)				{ ; }
	
	void 	addFlow(ftFlowType _type, ofTexture& _tex, float _strength  = 1.0);
	void	addFluidDensity(ofTexture& _tex, float _strength  = 1.0) 	{ fluidSimulation.addDensity(_tex, _strength); }
	void	addFluidVelocity(ofTexture& _tex, float _strength  = 1.0)	{ fluidSimulation.addVelocity(_tex, _strength); }
	void	addFluidTemperature(ofTexture& _tex, float _strength  = 1.0){ fluidSimulation.addTemperature(_tex, _strength); }
	void	addFluidPressure(ofTexture& _tex, float _strength  = 1.0)	{ fluidSimulation.addPressure(_tex, _strength); }
	void	addObstacleConstant(ofTexture& _tex)						{ fluidSimulation.addObstacle(_tex); }
	void	addObstacleTemporary(ofTexture& _tex)						{ fluidSimulation.addTempObstacle(_tex); }

	ofTexture&	getFlow(ftFlowType _type);
//	ofTexture&	getInputVelocity()		{ return opticalFlow.getInput(); }
//	ofTexture&	getInputDensity()		{ return densityBridge.getDensityInput(); }
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
	ofTexture&	getFluidVorticityVelocity()		{ return fluidSimulation.getVorticityVelocity(); }
	ofTexture&	getFluidConfinement()	{ return fluidSimulation.getConfinement(); }
	ofTexture&	getFluidBuoyancy() 		{ return fluidSimulation.getBuoyancy(); }
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
	void showScalarListener(bool &_value);
	ofParameter<bool>	showField;
	void showFieldListener(bool &_value);
	ofParameter<float>	visualizeScale;
	void visualizeScaleListener(float& _value);
	
	// Parameters
	void				setupParameters();
	
	// DRAW
	ofParameter<int>	drawMode;
	void drawModeListener(int& _value) ;
	ofParameter<string> drawName;
	void				drawTex(ofTexture& _tex, int _x, int _y, int _w, int _h, ofBlendMode _blendMode = OF_BLENDMODE_ALPHA);
	void				drawVis(ofTexture& _tex, int _x, int _y, int _w, int _h, ofBlendMode _blendMode = OF_BLENDMODE_ALPHA);
};

