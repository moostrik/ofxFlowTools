 
#pragma once

#include "ofMain.h"
#include "ofxFlowTools.h"

#include "ftAdvectShader.h"
#include "ftBuoyancyShader.h"
#include "ftDivergenceShader.h"
#include "ftGradientShader.h"
#include "ftJacobiDiffusionShader.h"
#include "ftJacobiPressureShader.h"
#include "ftObstacleOffsetShader.h"
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
		ofTexture&	getObstacle()				{ return obstacleOffsetFbo.getTexture(); }
		ofTexture&	getObstacleEdges()			{ return obstacleFbo.getTexture(); }
		ofTexture&	getVorticityCurl()			{ return vorticityCurlFbo.getTexture(); }
		ofTexture&	getBuoyancy()				{ return buoyancyFbo.getTexture(); }
		
		void	drawVelocity(int _x, int _y, int _w, int _h)		{ drawInput (_x, _y, _w, _h); }
		void	drawOutput(int _x, int _y, int _w, int _h) override	{ drawDensity (_x, _y, _w, _h); }
		void	drawDensity(int _x, int _y, int _w, int _h)			{ outputFbo.getTexture().draw(_x, _y, _w, _h); }
		void	drawPressure(int _x, int _y, int _w, int _h)		{ visualizationField.draw(pressureFbo.getTexture(), _x, _y, _w, _h); }
		void	drawTemperature(int _x, int _y, int _w, int _h)		{ visualizationField.draw(temperatureFbo.getTexture(), _x, _y, _w, _h); }
		void	drawDivergence(int _x, int _y, int _w, int _h)		{ visualizationField.draw(divergenceFbo.getTexture(), _x, _y, _w, _h); }
		void	drawObstacle(int _x, int _y, int _w, int _h)		{ obstacleFbo.draw(_x, _y, _w, _h); }
		void	drawVorticity(int _x, int _y, int _w, int _h)		{ visualizationField.draw(vorticityForceFbo.getTexture(), _x, _y, _w, _h); }
		void	drawBuoyancy(int _x, int _y, int _w, int _h)		{ visualizationField.draw(buoyancyFbo.getTexture(), _x, _y, _w, _h); }
			
		int		getSimulationWidth()				{ return simulationWidth; }
		int		getSimulationHeight()				{ return simulationHeight; }
		int		getDensityWidth()					{ return densityWidth; }
		int		getDensityHeight()					{ return densityHeight; }
		
		float	getSpeed()							{ return speed.get(); }
		float	getDissipationVel()					{ return dissipationVel.get(); }
		float	getDissipationDen()					{ return dissipationDen.get(); }
		float	getDissipationTmp()					{ return dissipationTmp.get(); }
		float	getDissipationPrs()					{ return dissipationPrs.get(); }
		float	getViscosityVel()					{ return viscosityVel.get(); }
		float	getViscosityDen()					{ return viscosityDen.get(); }
		float	getViscosityTmp()					{ return viscosityTmp.get(); }
		float	getVorticity()						{ return vorticity.get(); }
		float	getBuoyancySigma()					{ return buoyancySigma.get(); }
		float	getBuoyancyWeight()					{ return buoyancyWeight.get(); }
		float	getBuoyancyAmbientTemperature()		{ return buoyancyAmbientTemperature.get(); }
		
		void	setSpeed(float value)				{ speed.set(value); }
		void	setVorticity(float value)			{ vorticity.set(value); }
		void	setDissipationVel(float value)		{ dissipationVel.set(value); }
		void	setDissipationDen(float value)		{ dissipationDen.set(value); }
		void	setDissipationTmp(float value)		{ dissipationTmp.set(value); }
		float	setViscosityVel(float value)		{ viscosityVel.set(value); }
		float	setViscosityDen(float value)		{ viscosityDen.set(value); }
		float	setViscosityTmp(float value)		{ viscosityTmp.set(value); }
		void	setBuoyancySigma(float value)		{ buoyancySigma.set(value); }
		void	setBuoyancyWeight(float value)		{ buoyancyWeight.set(value); }
		void	setBuoyancyAmbientTemperature(float value)	{ buoyancyAmbientTemperature.set(value); }
		
	protected:
		ofParameter<float>		speed;
		ofParameter<int>		gridScale;
		ofParameter<float>		vorticity;
		ofParameterGroup		dissipationParameters;
		ofParameter<float>		dissipationVel;
		ofParameter<float>		dissipationDen;
		ofParameter<float>		dissipationTmp;
		ofParameter<float>		dissipationPrs;
		ofParameterGroup		viscosityParameters;
		ofParameter<float>		viscosityVel;
		ofParameter<float>		viscosityDen;
		ofParameter<float>		viscosityTmp;
		ofParameterGroup		buoyancyParameters;
		ofParameter<float>		buoyancySigma;
		ofParameter<float>		buoyancyWeight;
		ofParameter<float>		buoyancyAmbientTemperature;
		
		ftAdvectShader				advectShader;
		ftBuoyancyShader			buoyancyShader;
		ftDivergenceShader			divergenceShader;
		ftGradientShader			gradientShader;
		ftJacobiDiffusionShader		jacobiDiffusionShader;
		ftJacobiPressureShader		jacobiPressureShader;
		ftObstacleOffsetShader		obstacleOffsetShader;
		ftVorticityCurlShader		vorticityCurlShader;
		ftVorticityForceShader		vorticityForceShader;
		
		ftAddBooleanShader			addBooleanShader;
		ftMultiplyForceShader		multiplyForceShader;
		
		ftPingPongFbo	pressureFbo;
		ftPingPongFbo	temperatureFbo;
		ftPingPongFbo	obstacleFbo;
		ofFbo			obstacleOffsetFbo;
		ofFbo			divergenceFbo;
		ofFbo			vorticityCurlFbo;
		ofFbo			vorticityForceFbo;
		ofFbo			buoyancyFbo;
		
		int		simulationWidth, simulationHeight, densityWidth, densityHeight;
		int		numJacobiIterationsProjection;
		int		numJacobiIterationsDiffuse;
//		int		gridScale;
		
		void	allocate(int _inputWidth, int _inputHeight, GLint _inputInternalFormat, int _outputWidth, int _outputHeight, GLint _outputInternalFormat) override ;
		
		void	initObstacle();
		
	};
}

