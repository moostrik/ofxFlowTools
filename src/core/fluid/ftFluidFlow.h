 
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
		
		void	setup(int _flowWidth, int _flowHeight)	{ setup(_flowWidth, _flowHeight, _flowWidth, _flowHeight); }
		void	setup(int _flowWidth, int _flowHeight, int _densityWidth, int _densityHeight);
		void	update(float _deltaTime);
		void	reset() override;
		
		void 	setFlow(ftFlowForceType _type, ofTexture& _tex);
		void	setVelocity(ofTexture& _tex)	{ setInput(_tex); }
		void	setDensity(ofTexture& _tex)		{ setOutput(_tex); }
		void	setTemperature(ofTexture& _tex)	{ set(temperatureFbo, _tex); }
		void	setPressure(ofTexture& _tex)	{ set(pressureFbo, _tex); }
		void	setObstacle(ofTexture& _tex)	{ createEdgeImage(obstacleFbo); addObstacle(_tex); }
		
		void 	addFlow(ftFlowForceType _type, ofTexture& _tex, float _strength  = 1.0);
		void	addVelocity(ofTexture& _tex, float _strength  = 1.0)	{ addInput(_tex, _strength); }
		void	addDensity(ofTexture& _tex, float _strength  = 1.0)		{ addOutput(_tex); }
		void	addTemperature(ofTexture& _tex, float _strength  = 1.0)	{ add(temperatureFbo, _tex); }
		void	addPressure(ofTexture& _tex, float _strength  = 1.0)	{ add(pressureFbo, _tex); }
		void	addObstacle(ofTexture& _tex);
		
		ofTexture&	getDensity()				{ return getOutput(); }
		ofTexture&	getVelocity()				{ return getInput(); }
		ofTexture&	getPressure()				{ return pressureFbo.getTexture(); }
		ofTexture&	getTemperature()			{ return temperatureFbo.getTexture(); }
		ofTexture&	getDivergence()				{ return divergenceFbo.getTexture(); }
		ofTexture&	getObstacle()				{ return obstacleFbo.getTexture(); }
		ofTexture&	getVorticityVelocity()		{ return vorticityVelocityFbo.getTexture(); }
		ofTexture&	getVorticityConfinement()	{ return vorticityConfinementFbo.getTexture(); }
		ofTexture&	getBuoyancy()				{ return smokeBuoyancyFbo.getTexture(); }
		
		void	drawVelocity(int _x, int _y, int _w, int _h)			{ drawInput (_x, _y, _w, _h); }
		void	drawOutput(int _x, int _y, int _w, int _h) override		{ drawDensity (_x, _y, _w, _h); }
		void	drawDensity(int _x, int _y, int _w, int _h)				{ outputFbo.getTexture().draw(_x, _y, _w, _h); }
		void	drawPressure(int _x, int _y, int _w, int _h)			{ visualizationField.draw(pressureFbo.getTexture(), _x, _y, _w, _h); }
		void	drawTemperature(int _x, int _y, int _w, int _h)			{ visualizationField.draw(temperatureFbo.getTexture(), _x, _y, _w, _h); }
		void	drawDivergence(int _x, int _y, int _w, int _h)			{ visualizationField.draw(divergenceFbo.getTexture(), _x, _y, _w, _h); }
		void	drawObstacle(int _x, int _y, int _w, int _h)			{ obstacleFbo.draw(_x, _y, _w, _h); }
		void	drawVorticityVelocity(int _x, int _y, int _w, int _h)	{ visualizationField.draw(vorticityVelocityFbo.getTexture(), _x, _y, _w, _h); }
		void	drawVorticityConfinement(int _x, int _y, int _w, int _h){ visualizationField.draw(vorticityConfinementFbo.getTexture(), _x, _y, _w, _h); }
		void	drawBuoyancy(int _x, int _y, int _w, int _h)			{ visualizationField.draw(smokeBuoyancyFbo.getTexture(), _x, _y, _w, _h); }
			
		int		getSimulationWidth()				{ return simulationWidth; }
		int		getSimulationHeight()				{ return simulationHeight; }
		int		getDensityWidth()					{ return densityWidth; }
		int		getDensityHeight()					{ return densityHeight; }
		
		float	getSpeed()							{ return speed.get(); }
		float	getCellSize()						{ return cellSize.get(); }
		float	getNumJacobiIterations()			{ return numJacobiIterations.get(); }
		float	getViscosity()						{ return viscosity.get(); }
		float	getVorticity()						{ return vorticity.get(); }
		float	getDissipation()					{ return dissipation.get(); }
		float	getSmokeSigma()						{ return smokeSigma.get(); }
		float	getSmokeWeight()					{ return smokeWeight.get(); }
		float	getAmbientTemperature()				{ return ambientTemperature.get(); }
		glm::vec2 getGravity()						{ return gravity.get(); }
		
		void	setSpeed(float value)				{speed.set(value);}
		void	setCellSize(float value)			{cellSize.set(value);}
		void	setNumJacobiIterations(float value)	{numJacobiIterations.set(value);}
		void	setViscosity(float value)			{viscosity.set(value);}
		void	setVorticity(float value)			{vorticity.set(value);}
		void	setDissipation(float value)			{dissipation.set(value);}
		void	setSmokeSigma(float value)			{smokeSigma.set(value);}
		void	setSmokeWeight(float value)			{smokeWeight.set(value);}
		void	setAmbientTemperature(float value)	{ambientTemperature.set(value);}
		void	setGravity(glm::vec2 value)			{gravity.set(value);}
		
	protected:
		ofParameter<float>			speed;
		ofParameter<float>			cellSize;
		ofParameter<int>			numJacobiIterations;
		ofParameter<float>			viscosity;
		ofParameter<float>			vorticity;
		ofParameter<float>			dissipation;
		ofParameterGroup			smokeBuoyancyParameters;
		ofParameter<float>			smokeSigma;
		ofParameter<float>			smokeWeight;
		ofParameter<float>			ambientTemperature;
		ofParameter<glm::vec2>		gravity;
		ofParameterGroup			maxValues;
		
		ftDiffuseShader				diffuseShader;
		ftAdvectShader				advectShader;
		ftDivergenceShader			divergenceShader;
		ftJacobiShader				jacobiShader;
		ftSubstractGradient			substractGradientShader;
		ftSmokeBuoyancyShader		smokeBuoyancyShader;
		ftVorticityFirstPassShader	vorticityFirstPassShader;
		ftVorticitySecondPassShader	vorticitySecondPassShader;
		
		ftClampLengthShader			clampLengthShader;
		ftDensityFloatMultiplier	densityFloatMultiplierShader;
		ftDensityVec2Multiplier		densityVec2MultiplierShader;
		
		ftPingPongFbo	temperatureFbo;
		ftPingPongFbo	pressureFbo;
		ofFbo			divergenceFbo;
		ofFbo			obstacleFbo;
		ofFbo			vorticityVelocityFbo;
		ofFbo			vorticityConfinementFbo;
		ofFbo			smokeBuoyancyFbo;
		
		int simulationWidth, simulationHeight, densityWidth, densityHeight;
		
		void createEdgeImage(ofFbo& _Fbo, int _edgeWidth = 1, ofColor _backgroundColor = ofColor(255, 255, 255, 255), ofColor _edgeColor = ofColor(0, 0, 0, 255));
	};
}

