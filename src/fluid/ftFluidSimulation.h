 
#pragma once

#include "ofMain.h"
#include "ftFbo.h"
#include "ftSwapBuffer.h"
#include "ftDiffuseShader.h"
#include "ftAdvectShader.h"
#include "ftDivergenceShader.h"
#include "ftJacobiShader.h"
#include "ftSubstractGradientShader.h"
#include "ftSmokeBuoyancyShader.h"
#include "ftVorticityFirstPassShader.h"
#include "ftVorticitySecondPassShader.h"
#include "ftAddForceShader.h"
#include "ftToScalarShader.h"
#include "ftClampLengthShader.h"

#include "ftDensityVec2Multiplier.h"
#include "ftDensityFloatMultiplier.h"


namespace flowTools {
	class ftFluidSimulation {
	public:
		ftFluidSimulation();
		
		void	setup(int _simulationWidth, int _simulationHeight, int _densityWidth = 0, int _densityHeight = 0);
		void    update(float _deltaTime = 0);
		void    draw(int x = 0, int y = 0, float _width = -1, float _height = -1);
		void	reset();
		void	resetBackground();
		
		void    addDensity(ofTexture& _tex, float _strength  = 1.0);
		void    addVelocity(ofTexture& _tex, float _strength  = 1.0);
		void    addTemperature(ofTexture& _tex, float _strength  = 1.0);
		void    addPressure(ofTexture& _tex, float _strength  = 1.0);
		void    addObstacle(ofTexture& _tex);
		void    addTempObstacle(ofTexture& _tex);
		
		ofTexture&	getDensity() {return densitySwapBuffer.getBackTexture();};
		ofTexture&	getVelocity() {return velocitySwapBuffer.getBackTexture();};
		ofTexture&	getPressure() {return pressureSwapBuffer.getBackTexture();}
		ofTexture&	getTemperature() {return temperatureSwapBuffer.getBackTexture();}
		ofTexture&	getDivergence() {return divergenceBuffer.getTexture();}
		ofTexture&	getObstacle() {return combinedObstacleBuffer.getTexture();}
		ofTexture&	getCurlMagnitude() {return vorticityFirstPassBuffer.getTexture();}
		ofTexture&	getConfinement() {return vorticitySecondPassBuffer.getTexture();}
		ofTexture&	getSmokeBuoyancy() {return smokeBuoyancyBuffer.getTexture();}
			
		int		getSimulationWidth() {return simulationWidth;}
		int		getSimulationHeight() {return simulationHeight;}
		int		getDensityWidth() {return densityWidth;}
		int		getDensityHeight() {return densityHeight;}
		
		float getTimeStep() {return timeStep;}
		float getSpeed() {return speed.get();}
		float getCellSize() {return cellSize.get();}
		float getNumJacobiIterations() {return numJacobiIterations.get();}
		float getViscosity() {return viscosity.get();}
		float getVorticity() {return vorticity.get();}
		float getDissipation() {return dissipation.get();}
		float getDissipationVelocityOffset() {return velocityOffset.get();}
		float getDissipationDensityOffset() {return densityOffset.get();}
		float getDissipationTemperatureOffset() {return temperatureOffset.get();}
		float getSmokeSigma() {return smokeSigma.get();}
		float getSmokeWeight() {return smokeWeight.get();}
		float getAmbientTemperature() {return ambientTemperature.get();}
		ofVec2f getGravity() {return gravity.get();}
		float getClampForce() {return clampForce.get();}
		float getMaxVelocity() {return maxVelocity.get();}
		float getMaxDensity() {return maxDensity.get();}
		float getMaxTemperature() {return maxTemperature.get();}
		float getDensityFromVorticity() {return densityFromVorticity.get();}
		float getDensityFromPressure() {return densityFromPressure.get();}
		
		void setSpeed(float value) {speed.set(value);}
		void setCellSize(float value) {cellSize.set(value);}
		void setNumJacobiIterations(float value) {numJacobiIterations.set(value);}
		void setViscosity(float value) {viscosity.set(value);}
		void setVorticity(float value) {vorticity.set(value);}
		void setDissipation(float value) {dissipation.set(value);}
		void setDissipationVelocityOffset(float value) {velocityOffset.set(value);}
		void setDissipationDensityOffset(float value) {densityOffset.set(value);}
		void setDissipationTemperatureOffset(float value) {temperatureOffset.set(value);}
		void setSmokeSigma(float value) {smokeSigma.set(value);}
		void setSmokeWeight(float value) {smokeWeight.set(value);}
		void setAmbientTemperature(float value) {ambientTemperature.set(value);}
		void setGravity(ofVec2f value) {gravity.set(value);}
		void setClampForce(float value) {clampForce.set(value);}
		void setMaxVelocity(float value) {maxVelocity.set(value);}
		void setMaxDensity(float value) {maxDensity.set(value);}
		void setMaxTemperature(float value) {maxTemperature.set(value);}
		void setDensityFromVorticity(float value) {densityFromVorticity.set(value);}
		void setDensityFromPressure(float value) {densityFromPressure.set(value);}
		
		ofParameterGroup	parameters;
	private:
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
		ofParameter<float>	temperatureOffset;
		ofParameterGroup	smokeBuoyancyParameters;
		ofParameter<float>	smokeSigma;
		ofParameter<float>	smokeWeight;
		ofParameter<float>	ambientTemperature;
		ofParameter<ofVec2f>gravity;
		ofParameterGroup	maxValues;
		ofParameter<float>	clampForce;
		ofParameter<float>	maxVelocity;
		ofParameter<float>	maxDensity;
		ofParameter<float>	maxTemperature;
		ofParameter<float>	densityFromPressure;
		ofParameter<float>	densityFromVorticity;
		
		ftDiffuseShader			diffuseShader;
		ftAdvectShader			advectShader;
		ftDivergenceShader		divergenceShader;
		ftJacobiShader			jacobiShader;
		ftSubstractGradient		substractGradientShader;
		ftSmokeBuoyancyShader	smokeBuoyancyShader;
		ftVorticityFirstPassShader	vorticityFirstPassShader;
		ftVorticitySecondPassShader vorticitySecondPassShader;
		ftAddForceShader		addShader;
		ftClampLengthShader		clampLengthShader;
		ftDensityFloatMultiplier	densityFloatMultiplierShader;
		ftDensityVec2Multiplier		densityVec2MultiplierShader;
				
		ftSwapBuffer	densitySwapBuffer;
		ftSwapBuffer	velocitySwapBuffer;
		ftSwapBuffer	temperatureSwapBuffer;
		ftSwapBuffer	pressureSwapBuffer;
		ftFbo			divergenceBuffer;
		ftFbo			smokeBuoyancyBuffer;
		ftFbo			vorticityFirstPassBuffer;
		ftFbo			vorticitySecondPassBuffer;
		ftFbo			addPressureBuffer;
		bool			addPressureBufferDidChange;
		ftFbo			obstacleBuffer;
		ftFbo			addTempObstacleBuffer;
		bool			addTempObstacleBufferDidChange;
		ftFbo			combinedObstacleBuffer;
		bool			combinedObstacleNeedsToBeCleaned;
		
		int simulationWidth;
		int simulationHeight;
		int densityWidth;
		int densityHeight;
		float deltaTime;
		float lastTime;
		float timeStep;
		
		void createEdgeImage(ftFbo& buffer, int _edgeWidth = 1, ofColor _backgroundColor = ofColor(255, 255, 255, 255), ofColor _edgeColor = ofColor(0, 0, 0, 255));

	};
}

