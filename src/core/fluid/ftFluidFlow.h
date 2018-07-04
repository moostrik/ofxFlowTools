 
#pragma once

#include "ofMain.h"
#include "ofxFlowTools.h"

#include "ftDiffuseShader.h"
#include "ftAdvectShader.h"
#include "ftDivergenceShader.h"
#include "ftJacobiShader.h"
#include "ftSubstractGradientShader.h"
#include "ftSmokeBuoyancyShader.h"
#include "ftVorticityFirstPassShader.h"
#include "ftVorticitySecondPassShader.h"

#include "ftAddMultipliedShader.h"
#include "ftClampLengthShader.h"

#include "ftDensityVec2Multiplier.h"
#include "ftDensityFloatMultiplier.h"

namespace flowTools {
	class ftFluidFlow : public ftFlow{
	public:
		ftFluidFlow();
		
		void	setup(int _flowWidth, int _flowHeight) { setup(_flowWidth, _flowHeight, _flowWidth, _flowHeight); }
		void	setup(int _flowWidth, int _flowHeight, int _densityWidth, int _densityHeight);
		void	update(float _deltaTime = -1);
		
		void	reset();
		void	resetBackground();
		
		void 	addFlow(ftFlowForceType _type, ofTexture& _tex, float _strength  = 1.0);
		void	addDensity(ofTexture& _tex, float _strength  = 1.0);
		void	addVelocity(ofTexture& _tex, float _strength  = 1.0);
		void	addTemperature(ofTexture& _tex, float _strength  = 1.0);
		void	addPressure(ofTexture& _tex, float _strength  = 1.0);
		void	addObstacle(ofTexture& _tex);
		void	addTempObstacle(ofTexture& _tex);
		
		ofTexture&	getDensity()			{ return getOutput(); }
		ofTexture&	getVelocity()			{ return getInput(); }
		ofTexture&	getPressure()			{ return pressureFbo.getTexture(); }
		ofTexture&	getTemperature()		{ return temperatureFbo.getTexture(); }
		ofTexture&	getDivergence()			{ return divergenceFbo.getTexture(); }
		ofTexture&	getObstacle()			{ return combinedObstacleFbo.getTexture(); }
		ofTexture&	getVorticityVelocity()	{ return vorticityFirstPassFbo.getTexture(); }
		ofTexture&	getConfinement()		{ return vorticitySecondPassFbo.getTexture(); }
		ofTexture&	getBuoyancy()			{ return smokeBuoyancyFbo.getTexture(); }
		
		void	draw() { draw(0, 0, densityWidth, densityHeight); }
		void	draw(int _x, int _y) { draw(_x, _y, densityWidth, densityHeight); }
		void	draw(int _x, int _y, float _w, float _h) { drawDensity (_x, _y, _w, _h) ;}
		void	drawDensity(int _x, int _y, int _w, int _h);
		void	drawVelocity(int _x, int _y, int _w, int _h);
		void	drawPressure(int _x, int _y, int _w, int _h);
		void	drawTemperature(int _x, int _y, int _w, int _h);
		void	drawDivergence(int _x, int _y, int _w, int _h);
		void	drawObstacles(int _x, int _y, int _w, int _h);
		void	drawVorticityVelocity(int _x, int _y, int _w, int _h);
		void	drawConfinement(int _x, int _y, int _w, int _h);
		void	drawBuoyancy(int _x, int _y, int _w, int _h);
			
		int		getSimulationWidth()				{ return simulationWidth; }
		int		getSimulationHeight()				{ return simulationHeight; }
		int		getDensityWidth()					{ return densityWidth; }
		int		getDensityHeight()					{ return densityHeight; }
		
		float	getTimeStep()						{ return timeStep; }
		float	getSpeed()							{ return speed.get(); }
		float	getCellSize()						{ return cellSize.get(); }
		float	getNumJacobiIterations()			{ return numJacobiIterations.get(); }
		float	getViscosity()						{ return viscosity.get(); }
		float	getVorticity()						{ return vorticity.get(); }
		float	getDissipation()					{ return dissipation.get(); }
		float	getDissipationVelocityOffset()		{return velocityOffset.get(); }
		float	getDissipationDensityOffset()		{return densityOffset.get(); }
		float	getDissipationTemperatureOffset()	{return temperatureOffset.get(); }
		float	getSmokeSigma()						{ return smokeSigma.get(); }
		float	getSmokeWeight()					{ return smokeWeight.get(); }
		float	getAmbientTemperature()				{ return ambientTemperature.get(); }
		glm::vec2 getGravity()						{ return gravity.get(); }
		float	getClampForce()						{ return clampForce.get(); }
		float	getMaxVelocity()					{ return maxVelocity.get(); }
		float	getMaxDensity()						{ return maxDensity.get(); }
		float	getMaxTemperature()					{ return maxTemperature.get(); }
		float	getDensityFromVorticity()			{ return densityFromVorticity.get(); }
		float	getDensityFromPressure()			{ return densityFromPressure.get(); }
		
		void	setSpeed(float value)				{speed.set(value);}
		void	setCellSize(float value)			{cellSize.set(value);}
		void	setNumJacobiIterations(float value)	{numJacobiIterations.set(value);}
		void	setViscosity(float value)			{viscosity.set(value);}
		void	setVorticity(float value)			{vorticity.set(value);}
		void	setDissipation(float value)			{dissipation.set(value);}
		void	setDissipationVelocityOffset(float value)	{velocityOffset.set(value);}
		void	setDissipationDensityOffset(float value)	{densityOffset.set(value);}
		void	setDissipationTemperatureOffset(float value){temperatureOffset.set(value);}
		void	setSmokeSigma(float value)			{smokeSigma.set(value);}
		void	setSmokeWeight(float value)			{smokeWeight.set(value);}
		void	setAmbientTemperature(float value)	{ambientTemperature.set(value);}
		void	setGravity(glm::vec2 value)			{gravity.set(value);}
		void	setClampForce(float value)			{clampForce.set(value);}
		void	setMaxVelocity(float value)			{maxVelocity.set(value);}
		void	setMaxDensity(float value)			{maxDensity.set(value);}
		void	setMaxTemperature(float value)		{maxTemperature.set(value);}
		void	setDensityFromVorticity(float value){densityFromVorticity.set(value);}
		void	setDensityFromPressure(float value)	{densityFromPressure.set(value);}
		
		ofParameterGroup&	getParameters() 	{ return parameters; }
	protected:
		ofParameterGroup	parameters;
		ofParameter<bool>	doReset;
		ofParameter<float>	speed;
		ofParameter<float>	cellSize;
		ofParameter<int>	numJacobiIterations;
		ofParameter<float>	viscosity;
		ofParameter<float>	vorticity;
		ofParameter<float>	dissipation;
		ofParameterGroup	advancedDissipationParameters;
		ofParameter<float>	velocityOffset;
		ofParameter<float>	densityOffset;
		ofParameter<float>	pressureOffset;
		ofParameter<float>	temperatureOffset;
		ofParameterGroup	smokeBuoyancyParameters;
		ofParameter<float>	smokeSigma;
		ofParameter<float>	smokeWeight;
		ofParameter<float>	ambientTemperature;
		ofParameter<glm::vec2>gravity;
		ofParameterGroup	maxValues;
		ofParameter<float>	clampForce;
		ofParameter<float>	maxVelocity;
		ofParameter<float>	maxDensity;
		ofParameter<float>	maxTemperature;
		ofParameter<float>	maxPressure;
		ofParameter<float>	densityFromPressure;
		ofParameter<float>	densityFromVorticity;
		
		ftDiffuseShader				diffuseShader;
		ftAdvectShader				advectShader;
		ftDivergenceShader			divergenceShader;
		ftJacobiShader				jacobiShader;
		ftSubstractGradient			substractGradientShader;
		ftSmokeBuoyancyShader		smokeBuoyancyShader;
		ftVorticityFirstPassShader	vorticityFirstPassShader;
		ftVorticitySecondPassShader	vorticitySecondPassShader;
		
		ftAddMultipliedShader		addMultipliedShader;
		
		ftClampLengthShader			clampLengthShader;
		ftDensityFloatMultiplier	densityFloatMultiplierShader;
		ftDensityVec2Multiplier		densityVec2MultiplierShader;
		
		ftPingPongFbo	temperatureFbo;
		ftPingPongFbo	pressureFbo;
		ofFbo		divergenceFbo;
		ofFbo		smokeBuoyancyFbo;
		ofFbo		vorticityFirstPassFbo;
		ofFbo		vorticitySecondPassFbo;
		ofFbo		addPressureFbo;
		bool		addPressureFboDidChange;
		ofFbo		obstacleFbo;
		ofFbo		addTempObstacleFbo;
		bool		addTempObstacleFboDidChange;
		ofFbo		combinedObstacleFbo;
		bool		combinedObstacleNeedsToBeCleaned;
		
		int simulationWidth;
		int simulationHeight;
		int densityWidth;
		int densityHeight;
		float deltaTime;
		float lastTime;
		float timeStep;
		
		void createEdgeImage(ofFbo& _Fbo, int _edgeWidth = 1, ofColor _backgroundColor = ofColor(255, 255, 255, 255), ofColor _edgeColor = ofColor(0, 0, 0, 255));

	};
}

