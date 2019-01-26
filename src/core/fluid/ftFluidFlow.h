 
#pragma once

#include "ofMain.h"
#include "ofxFlowTools.h"

#include "ftAdvectShader.h"
#include "ftAdvectWrapShader.h"
#include "ftDiffuseShader.h"
#include "ftDivergenceShader.h"
#include "ftJacobiShader.h"
#include "ftJacobiObstacleShader.h"
#include "ftSubstractGradientShader.h"
#include "ftSmokeBuoyancyShader.h"
#include "ftVorticityVelocityShader.h"
#include "ftVorticityConfinementShader.h"

#include "ftAddBooleanShader.h"
#include "ftObstacleOffsetShader.h"
#include "ftApplyObstaclePosShader.h"
#include "ftApplyObstacleNegShader.h"
#include "ftApplyObstacleZeroShader.h"
#include "ftApplyObstacleDensityShader.h"

#include "ftMultiplyForceShader.h"
#include "ftClampLengthShader.h"

namespace flowTools {
	class ftFluidFlow : public ftFlow{
	public:
		ftFluidFlow();
		
		void	setup(int _simulationWidth, int _simulationHeight)	{ setup(_simulationWidth, _simulationHeight, _simulationWidth, _simulationHeight); }
		void	setup(int _simulationWidth, int _simulationHeight, int _densityWidth, int _densityHeight);
		void	update(float _deltaTime);
		void	reset() override;
		
		void 	setFlow(ftFlowForceType _type, ofTexture& _tex);
		void	setVelocity(ofTexture& _tex)	{ setInput(_tex); }
		void	setDensity(ofTexture& _tex)		{ set(outputFbo, _tex); }
		void	setTemperature(ofTexture& _tex)	{ set(temperatureFbo, _tex); }
		void	setPressure(ofTexture& _tex)	{ set(pressureFbo, _tex); }
		void	setObstacle(ofTexture& _tex);
		
		void 	addFlow(ftFlowForceType _type, ofTexture& _tex, float _strength  = 1.0);
		void	addVelocity(ofTexture& _tex, float _strength  = 1.0)	{ addInput(_tex, _strength); }
		void	addDensity(ofTexture& _tex, float _strength  = 1.0)		{ add(outputFbo, _tex); }
		void	addTemperature(ofTexture& _tex, float _strength  = 1.0)	{ add(temperatureFbo, _tex); }
		void	addPressure(ofTexture& _tex, float _strength  = 1.0)	{ add(pressureFbo, _tex); }
		void	addObstacle(ofTexture& _tex);
		
		ofTexture&	getDensity()				{ return getOutput(); }
		ofTexture&	getVelocity()				{ return getInput(); }
		ofTexture&	getPressure()				{ return pressureFbo.getTexture(); }
		ofTexture&	getTemperature()			{ return temperatureFbo.getTexture(); }
		ofTexture&	getDivergence()				{ return divergenceFbo.getTexture(); }
		ofTexture&	getObstacle()				{ return obstacleFbo.getTexture(); }
		ofTexture&	getObstacleOffset()			{ return obstacleOffsetFbo.getTexture(); }
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
		void	drawObstacleOffset(int _x, int _y, int _w, int _h)		{ visualizationField.draw(obstacleOffsetFbo.getTexture(), _x, _y, _w, _h); }
		void	drawVorticityVelocity(int _x, int _y, int _w, int _h)	{ visualizationField.draw(vorticityVelocityFbo.getTexture(), _x, _y, _w, _h); }
		void	drawVorticityConfinement(int _x, int _y, int _w, int _h){ visualizationField.draw(vorticityConfinementFbo.getTexture(), _x, _y, _w, _h); }
		void	drawBuoyancy(int _x, int _y, int _w, int _h)			{ visualizationField.draw(smokeBuoyancyFbo.getTexture(), _x, _y, _w, _h); }
			
		int		getSimulationWidth()				{ return simulationWidth; }
		int		getSimulationHeight()				{ return simulationHeight; }
		int		getDensityWidth()					{ return densityWidth; }
		int		getDensityHeight()					{ return densityHeight; }
		
		float	getSpeed()							{ return speed.get(); }
		float	getNumJacobiIterations()			{ return numJacobiIterations.get(); }
		float	getViscosity()						{ return viscosity.get(); }
		float	getVorticity()						{ return vorticity.get(); }
		float	getDissipationVel()					{ return dissipationVel.get(); }
		float	getDissipationDen()					{ return dissipationDen.get(); }
		float	getDissipationPrs()					{ return dissipationPrs.get(); }
		float	getSmokeSigma()						{ return smokeSigma.get(); }
		float	getSmokeWeight()					{ return smokeWeight.get(); }
		float	getAmbientTemperature()				{ return ambientTemperature.get(); }
		glm::vec2 getGravity()						{ return gravity.get(); }
		
		void	setSpeed(float value)				{speed.set(value);}
		void	setNumJacobiIterations(float value)	{numJacobiIterations.set(value);}
		void	setViscosity(float value)			{viscosity.set(value);}
		void	setVorticity(float value)			{vorticity.set(value);}
		void	setDissipationVel(float value)		{dissipationVel.set(value);}
		void	setDissipationDen(float value)		{dissipationDen.set(value);}
		void	setDissipationPrs(float value)		{dissipationPrs.set(value);}
		void	setSmokeSigma(float value)			{smokeSigma.set(value);}
		void	setSmokeWeight(float value)			{smokeWeight.set(value);}
		void	setAmbientTemperature(float value)	{ambientTemperature.set(value);}
		void	setGravity(glm::vec2 value)			{gravity.set(value);}
		
	protected:
		ofParameter<float>			speed;
		ofParameter<int>			numJacobiIterations;
		ofParameter<float>			viscosity;
		ofParameter<float>			vorticity;
		ofParameterGroup			dissipationParameters;
		ofParameter<float>			dissipationVel;
		ofParameter<float>			dissipationDen;
		ofParameter<float>			dissipationPrs;
		ofParameterGroup			smokeBuoyancyParameters;
		ofParameter<float>			smokeSigma;
		ofParameter<float>			smokeWeight;
		ofParameter<float>			ambientTemperature;
		ofParameter<glm::vec2>		gravity;
//		ofParameter<bool>			wrap;
//		void wrapListener(bool &_value) { if(_value) { advectShader = &advectWrapShader; } else { advectShader = &advectNoWrapShader; } initObstacle(); }
		
		ftAdvectShader				advectNoWrapShader;
		ftAdvectWrapShader			advectWrapShader;
		ftAdvectShader*				advectShader;
		ftDiffuseShader				diffuseShader;
		ftDivergenceShader			divergenceShader;
		ftJacobiShader				jacobiShader;
		ftJacobiObstacleShader		jacobiObstacleShader;
		ftSubstractGradientShader	substractGradientShader;
		ftSmokeBuoyancyShader		smokeBuoyancyShader;
		ftVorticityVelocityShader	vorticityVelocityShader;
		ftVorticityConfinementShader vorticityConfinementShader;
		
		ftAddBooleanShader			addBooleanShader;
		ftObstacleOffsetShader		obstacleOffsetShader;
//		ftApplyObstacleShader		applyObstacleShader;
		ftApplyObstaclePosShader	applyObstaclePosShader;
		ftApplyObstacleNegShader	applyObstacleNegShader;
		ftApplyObstacleZeroShader	applyObstacleZeroShader;
		ftApplyObstacleDensityShader applyObstacleDensityShader;
		
		ftMultiplyForceShader		multiplyForceShader;
		ftClampLengthShader			clampLengthShader;
		
		ftPingPongFbo	temperatureFbo;
		ftPingPongFbo	pressureFbo;
		ofFbo			divergenceFbo;
		ftPingPongFbo	vorticityVelocityFbo;
		ofFbo			vorticityConfinementFbo;
		ofFbo			smokeBuoyancyFbo;
		ftPingPongFbo	obstacleFbo;
		ofFbo			obstacleOffsetFbo;
		
		int simulationWidth, simulationHeight, densityWidth, densityHeight;
		
		void allocate(int _inputWidth, int _inputHeight, GLint _inputInternalFormat, int _outputWidth, int _outputHeight, GLint _outputInternalFormat) override ;
		void initObstacle();
		
	};
}

