 
#pragma once

#include "ofMain.h"
#include "ofxFlowTools.h"

#include "ftAdvectShader.h"
#include "ftBuoyancyShader.h"
#include "ftDivergenceShader.h"
#include "ftGradientShader.h"
#include "ftJacobiShader.h"
#include "ftObstacleBoundsShader.h"
#include "ftVorticityCurlShader.h"
#include "ftVorticityForceShader.h"

#include "ftAddBooleanShader.h"
#include "ftMultiplyForceShader.h"

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
		ofTexture&	getObstacle()				{ return obstacleNFbo.getTexture(); }
		ofTexture&	getObstacleEdges()			{ return obstacleCFbo.getTexture(); }
		ofTexture&	getVorticityVelocity()		{ return vorticityCurlFbo.getTexture(); }
		ofTexture&	getVorticityConfinement()	{ return vorticityForceFbo.getTexture(); }
		ofTexture&	getBuoyancy()				{ return buoyancyFbo.getTexture(); }
		
		void	drawVelocity(int _x, int _y, int _w, int _h)			{ drawInput (_x, _y, _w, _h); }
		void	drawOutput(int _x, int _y, int _w, int _h) override		{ drawDensity (_x, _y, _w, _h); }
		void	drawDensity(int _x, int _y, int _w, int _h)				{ outputFbo.getTexture().draw(_x, _y, _w, _h); }
		void	drawPressure(int _x, int _y, int _w, int _h)			{ visualizationField.draw(pressureFbo.getTexture(), _x, _y, _w, _h); }
		void	drawTemperature(int _x, int _y, int _w, int _h)			{ visualizationField.draw(temperatureFbo.getTexture(), _x, _y, _w, _h); }
		void	drawDivergence(int _x, int _y, int _w, int _h)			{ visualizationField.draw(divergenceFbo.getTexture(), _x, _y, _w, _h); }
		void	drawObstacle(int _x, int _y, int _w, int _h)			{ obstacleCFbo.draw(_x, _y, _w, _h); }
		void	drawObstacleEdges(int _x, int _y, int _w, int _h)		{ obstacleNFbo.draw(_x, _y, _w, _h); }
		void	drawVorticityVelocity(int _x, int _y, int _w, int _h)	{ visualizationField.draw(vorticityCurlFbo.getTexture(), _x, _y, _w, _h); }
		void	drawVorticityConfinement(int _x, int _y, int _w, int _h){ visualizationField.draw(vorticityForceFbo.getTexture(), _x, _y, _w, _h); }
		void	drawBuoyancy(int _x, int _y, int _w, int _h)			{ visualizationField.draw(buoyancyFbo.getTexture(), _x, _y, _w, _h); }
			
		int		getSimulationWidth()				{ return simulationWidth; }
		int		getSimulationHeight()				{ return simulationHeight; }
		int		getDensityWidth()					{ return densityWidth; }
		int		getDensityHeight()					{ return densityHeight; }
		
		float	getSpeed()							{ return speed.get(); }
//		float	getViscosity()						{ return viscosity.get(); }
		float	getVorticity()						{ return vorticity.get(); }
		float	getDissipationVel()					{ return dissipationVel.get(); }
		float	getDissipationDen()					{ return dissipationDen.get(); }
		float	getDissipationTmp()					{ return dissipationTmp.get(); }
		float	getSmokeSigma()						{ return smokeSigma.get(); }
		float	getSmokeWeight()					{ return smokeWeight.get(); }
		float	getAmbientTemperature()				{ return ambientTemperature.get(); }
		
		void	setSpeed(float value)				{speed.set(value);}
//		void	setViscosity(float value)			{viscosity.set(value);}
		void	setVorticity(float value)			{vorticity.set(value);}
		void	setDissipationVel(float value)		{dissipationVel.set(value);}
		void	setDissipationDen(float value)		{dissipationDen.set(value);}
		void	setDissipationTmp(float value)		{dissipationTmp.set(value);}
		void	setSmokeSigma(float value)			{smokeSigma.set(value);}
		void	setSmokeWeight(float value)			{smokeWeight.set(value);}
		void	setAmbientTemperature(float value)	{ambientTemperature.set(value);}
		void	setGravity(glm::vec2 value)			{gravity.set(value);}
		
	protected:
		ofParameter<float>			speed;
		ofParameter<float>			gridScale;
		ofParameter<float>			vorticity;
		ofParameterGroup			dissipationParameters;
		ofParameter<float>			dissipationVel;
		ofParameter<float>			dissipationDen;
		ofParameter<float>			dissipationTmp;
		ofParameter<float>			dissipationPrs;
		ofParameterGroup			viscosityParameters;
		ofParameter<float>			viscosityVel;
		ofParameter<float>			viscosityDen;
		ofParameter<float>			viscosityTmp;
		ofParameterGroup			smokeBuoyancyParameters;
		ofParameter<float>			smokeSigma;
		ofParameter<float>			smokeWeight;
		ofParameter<float>			ambientTemperature;
		ofParameter<glm::vec2>		gravity;
//		ofParameter<bool>			wrap;
//		void wrapListener(bool &_value) { if(_value) { advectShader = &advectWrapShader; } else { advectShader = &advectNoWrapShader; } initObstacle(); }
		
		ftAdvectShader				advectShader;
		ftBuoyancyShader			buoyancyShader;
		ftDivergenceShader			divergenceShader;
		ftGradientShader			gradientShader;
		ftJacobiShader				jacobiShader;
		ftObstacleBoundsShader		obstacleBoundsShader;
		ftVorticityCurlShader		vorticityCurlShader;
		ftVorticityForceShader		vorticityForceShader;
		
		ftAddBooleanShader			addBooleanShader;
		ftMultiplyForceShader		multiplyForceShader;
		
		ftPingPongFbo	pressureFbo;
		ftPingPongFbo	temperatureFbo;
		ftPingPongFbo	obstacleCFbo;
		ofFbo			obstacleNFbo;
		ofFbo			divergenceFbo;
		ftPingPongFbo	vorticityCurlFbo;
		ftPingPongFbo	vorticityForceFbo;
		ofFbo			buoyancyFbo;
		
		int		numJacobiIterationsProjection;
		int		numJacobiIterationsDiffuse;
		
		
		int simulationWidth, simulationHeight, densityWidth, densityHeight;
		
		void allocate(int _inputWidth, int _inputHeight, GLint _inputInternalFormat, int _outputWidth, int _outputHeight, GLint _outputInternalFormat) override ;
		void initObstacle();
		
	};
}

