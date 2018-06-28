/*  ************************************************************************************
 *
 *  ftFluidSimulation
 *
 *  Created by Matthias Oostrik on 03/16.14.
 *  Copyright 2014 http://www.MatthiasOostrik.com All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the author nor the names of its contributors
 *       may be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 *  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 *  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 *  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 *  OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *	The Fluid shaders are adapted from various sources. Unfortunately I can't remember all, but the most important ones:
 *		* Mark J Harris: Various online sources
 *		* Patricio Gonzalez Vivo (http://www.patriciogonzalezvivo.com): ofxFluid
 * 
 *  ************************************************************************************ */

#include "ftFluidSimulation.h"

namespace flowTools {
	
	ftFluidSimulation::ftFluidSimulation(){
		
		parameters.setName("fluid solver");
		parameters.add(doReset.set("reset", false));
		parameters.add(speed.set("speed", 100, 0, 200));
		parameters.add(cellSize.set("cell size", 1.25, 0.0, 2.0));
		parameters.add(numJacobiIterations.set("iterations", 40, 1, 100));
		parameters.add(viscosity.set("viscosity", 0.1, 0, .4));
		parameters.add(vorticity.set("vorticity", 0.6, 0.0, 1));
		parameters.add(dissipation.set("dissipation", 0.002, 0, 0.01));
		advancedDissipationParameters.setName("advanced dissipation");
		advancedDissipationParameters.add(velocityOffset.set("velocity offset", -0.001, -0.01, 0.01));
		advancedDissipationParameters.add(densityOffset.set("density offset", 0, -0.01, 0.01));
		advancedDissipationParameters.add(temperatureOffset.set("temperature offset", 0.005, -0.01, 0.01));
		advancedDissipationParameters.add(pressureOffset.set("pressure offset", 0.005, -0.01, 0.01));
		parameters.add(advancedDissipationParameters);
		smokeBuoyancyParameters.setName("smoke buoyancy");
		smokeBuoyancyParameters.add(smokeSigma.set("sigma", 0.05, 0.0, 1.0));
		smokeBuoyancyParameters.add(smokeWeight.set("weight", 0.05, 0.0, 1.0));
		smokeBuoyancyParameters.add(ambientTemperature.set("ambient temperature", 0.0, 0.0, 1.0));
		smokeBuoyancyParameters.add(gravity.set("gravity", ofDefaultVec2(0., 9.80665), ofDefaultVec2(-10, -10), ofDefaultVec2(10, 10)));
		parameters.add(smokeBuoyancyParameters);
		maxValues.setName("maximum");
		maxValues.add(clampForce.set("clampForce", 0.05, 0, .1));
		maxValues.add(maxDensity.set("density", 2,0,5));
		maxValues.add(maxVelocity.set("velocity", 4,0,10));
		maxValues.add(maxTemperature.set("temperature", 2,0,5));
		maxValues.add(maxPressure.set("pressure", 2,0,5));
		parameters.add(maxValues);
		parameters.add(densityFromPressure.set("density from pressure", 0, -0.1, 0.1));
		parameters.add(densityFromVorticity.set("density from vorticity", -0.1, -0.5, 0.5));
	}
	
	//--------------------------------------------------------------
	void ftFluidSimulation::setup(int _flowWidth, int _flowHeight, int _densityWidth, int _densityHeight) {
		if (_densityWidth == 0 ) _densityWidth = _flowWidth;
		if (_densityHeight == 0 ) _densityHeight = _flowHeight;
		
		simulationWidth = _flowWidth;
		simulationHeight = _flowHeight;
		densityWidth = _densityWidth;
		densityHeight = _densityHeight;
		
		int	internalFormatDensity = GL_RGBA32F;
		int	internalFormatVelocity = GL_RG32F;
		int	interformatPressure = GL_R32F;
		int	internalFormatObstacle = GL_R8;
		
		ftFlow::allocate(simulationWidth, simulationHeight, internalFormatVelocity);
		outputFbo.allocate(densityWidth, densityHeight, internalFormatDensity);
		
		temperatureFbo.allocate(simulationWidth,simulationHeight,interformatPressure);
		ftUtil::zero(temperatureFbo);
		pressureFbo.allocate(simulationWidth,simulationHeight,interformatPressure);
		ftUtil::zero(pressureFbo);
		
		obstacleFbo.allocate(simulationWidth, simulationHeight, internalFormatObstacle);
		ftUtil::zero(obstacleFbo);
		createEdgeImage(obstacleFbo);
		
		divergenceFbo.allocate(simulationWidth, simulationHeight, interformatPressure);
		ftUtil::zero(divergenceFbo);
		smokeBuoyancyFbo.allocate(simulationWidth, simulationHeight, internalFormatVelocity);
		ftUtil::zero(smokeBuoyancyFbo);
		vorticityFirstPassFbo.allocate(simulationWidth, simulationHeight, internalFormatVelocity);
		ftUtil::zero(pressureFbo);
		vorticitySecondPassFbo.allocate(simulationWidth, simulationHeight, internalFormatVelocity);
		ftUtil::zero(pressureFbo);
		addPressureFbo.allocate(simulationWidth, simulationHeight, interformatPressure);
		ftUtil::zero(pressureFbo);
		addPressureFboDidChange = false;
		addTempObstacleFbo.allocate(simulationWidth, simulationHeight, internalFormatObstacle);
		ftUtil::zero(addTempObstacleFbo);
		addTempObstacleFboDidChange = false;
		combinedObstacleFbo.allocate(simulationWidth, simulationHeight, internalFormatObstacle);
		ftUtil::zero(combinedObstacleFbo);
		ftUtil::stretch(combinedObstacleFbo, obstacleFbo);
		
		deltaTime = 0;
		lastTime = 0;
		addTempObstacleFboDidChange = true;
		
	}
	
	//--------------------------------------------------------------
	void ftFluidSimulation::update(float _deltaTime){
		float time = ofGetElapsedTimef();
		if (_deltaTime != -1)
			deltaTime = _deltaTime;
		else
			deltaTime = time - lastTime;
		lastTime = time;
		timeStep = deltaTime * speed.get();// * simulationWidth;
		
		if (doReset) {
			doReset.set(false);
			reset();
		}
		
		ofPushStyle();
		
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		// OBSTACLE Fbo;
		
		if (combinedObstacleNeedsToBeCleaned) {
			ftUtil::zero(combinedObstacleFbo);
			ftUtil::stretch(combinedObstacleFbo, obstacleFbo);
			combinedObstacleNeedsToBeCleaned = false;
		}
		
		if (addTempObstacleFboDidChange) {
			ofEnableBlendMode(OF_BLENDMODE_ADD);
			ftUtil::stretch(combinedObstacleFbo, addTempObstacleFbo);
			addTempObstacleFboDidChange = false;
			ftUtil::zero(addTempObstacleFbo);
			combinedObstacleNeedsToBeCleaned = true;
		}
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);

		ftSwapFbo& velocityFbo = inputFbo;
		ftSwapFbo& densityFbo = outputFbo;
		
		// CLAMP LENGTH
//		if (maxDensity.get() > 0.0) {
//			densityFbo.swap();
//			clampLengthShader.update(*densityFbo.getFbo(),
//									 densityFbo.getBackTexture(),
//									 maxDensity.get(),
//									 clampForce.get());
//		}
//		if (maxVelocity.get() > 0.0) {
//			velocityFbo.swap();
//			clampLengthShader.update(*velocityFbo.getFbo(),
//									 velocityFbo.getBackTexture(),
//									 maxVelocity.get(),
//									 clampForce.get());
//		}
//		if (maxTemperature.get() > 0.0) {
//			temperatureFbo.swap();
//			clampLengthShader.update(*temperatureFbo.getFbo(),
//									 temperatureFbo.getBackTexture(),
//									 maxTemperature.get(),
//									 clampForce.get());
//		}
//		if (maxPressure.get() > 0.0) {
//			pressureFbo.swap();
//			clampLengthShader.update(*pressureFbo.getFbo(),
//									 pressureFbo.getBackTexture(),
//									 maxPressure.get(),
//									 clampForce.get());		}
		
		
		// VORTEX CONFINEMENT
		if (vorticity.get() > 0.0) {
			vorticityFirstPassShader.update(vorticityFirstPassFbo,
											velocityFbo.getTexture(),
											combinedObstacleFbo.getTexture());
			
			vorticitySecondPassShader.update(vorticitySecondPassFbo,
											 vorticityFirstPassFbo.getTexture(),
											 timeStep,
											 vorticity.get(),
											 cellSize.get());
			
			addVelocity(vorticitySecondPassFbo.getTexture());
		}
		
		// ADVECT
		velocityFbo.swap();
		advectShader.update(velocityFbo,
							velocityFbo.getBackTexture(),
							velocityFbo.getBackTexture(),
							combinedObstacleFbo.getTexture(),
							timeStep,
							1.0 - (dissipation.get() + velocityOffset.get()),
							cellSize.get());
		
		densityFbo.swap();
		advectShader.update(densityFbo,
							densityFbo.getBackTexture(),
							velocityFbo.getTexture(),
							combinedObstacleFbo.getTexture(),
							timeStep,
							1.0 - (dissipation.get() + densityOffset.get()),
							cellSize.get());
		
//		advectShader.update(*pressureFbo.getFbo(),
//							pressureFbo.getBackTexture(),
//							velocityFbo.getBackTexture(),
//							combinedObstacleFbo.getTexture(),
//							timeStep,
//							1.0 - (dissipation.get() + pressureOffset.get()),
//							cellSize.get());

		
		// DIFFUSE
		if (viscosity.get() > 0.0) {
			for (int i = 0; i < numJacobiIterations.get(); i++) {
				velocityFbo.swap();
				diffuseShader.update(velocityFbo,
									 velocityFbo.getBackTexture(),
									 combinedObstacleFbo.getTexture(),
//									 viscosity.get() * deltaTime); // works better than timeStep
									 viscosity.get() * timeStep);
			}
		 }
		 
		
		
		// SMOKE BUOYANCY -- UNSTABLE __ DISABLED FOR NOW
//		if (smokeSigma.get() > 0.0 && smokeWeight.get() > 0.0 ) {
//
//			temperatureFbo.swap();
//			advectShader.update(*temperatureFbo.getFbo(),
//								temperatureFbo.getBackTexture(),
//								velocityFbo.getTexture(),
//								combinedObstacleFbo.getTexture(),
//								timeStep,
//								1.0 - (dissipation.get() + temperatureOffset.get()), // WHY?
//								cellSize.get());
//			
//			smokeBuoyancyShader.update(smokeBuoyancyFbo,
//									   velocityFbo.getTexture(),
//									   temperatureFbo.getTexture(),
//									   densityFbo.getTexture(),
//									   ambientTemperature.get(),
//									   timeStep,
//									   smokeSigma.get(),
//									   smokeWeight.get(),
//									   gravity.get() * timeStep);
//			addVelocity(smokeBuoyancyFbo.getTexture());
//	
//		}
//		else
			ftUtil::zero(temperatureFbo);
		
		
		// DIVERGENCE AND JACOBI
		ftUtil::zero(divergenceFbo);
		divergenceShader.update(divergenceFbo,
								velocityFbo.getTexture(),
								combinedObstacleFbo.getTexture(),
								cellSize.get());
		
		ftUtil::zero(pressureFbo);
		for (int i = 0; i < numJacobiIterations.get(); i++) {
			pressureFbo.swap();
			jacobiShader.update(pressureFbo,
								pressureFbo.getBackTexture(),
								divergenceFbo.getTexture(),
								combinedObstacleFbo.getTexture(),
								cellSize.get());
		}
		
		// Multiply density by pressure and or vorticity
		if(densityFromPressure != 0) {
			densityFbo.swap();
			densityFloatMultiplierShader.update(densityFbo,
												densityFbo.getBackTexture(),
												pressureFbo.getTexture(),
												densityFromPressure.get());
		}
		
		if(densityFromVorticity != 0) {
			densityFbo.swap();
			densityVec2MultiplierShader.update(densityFbo,
											   densityFbo.getBackTexture(),
											   vorticitySecondPassFbo.getTexture(),
											   -densityFromVorticity.get());
		}
		
		
		// Drain some fluid
		if(addPressureFboDidChange == true) {
			addPressureFboDidChange = false;
			pressureFbo.swap();
			addMultipliedShader.update(pressureFbo,
									   pressureFbo.getTexture(),
									   addPressureFbo.getTexture(),
									   1.0,
									   1.0);
		}
		
		velocityFbo.swap();
		substractGradientShader.update(velocityFbo,
									   velocityFbo.getBackTexture(),
									   pressureFbo.getTexture(),
									   combinedObstacleFbo.getTexture(),
									   cellSize.get());
		
		ofPopStyle();
	}
	
	//--------------------------------------------------------------
	void ftFluidSimulation::addDensity(ofTexture & _tex, float _strength){
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		outputFbo.swap();
		addMultipliedShader.update(outputFbo,
								   outputFbo.getBackTexture(),
								   _tex,
								   1.0,
								   _strength);
		ofPopStyle();
	};
	
	//--------------------------------------------------------------
	void ftFluidSimulation::addVelocity(ofTexture & _tex, float _strength) {
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		inputFbo.swap();
		addMultipliedShader.update(inputFbo,
								   inputFbo.getBackTexture(),
								   _tex,
								   1.0,
								   _strength);
		ofPopStyle();
	}
	
	//--------------------------------------------------------------
	void ftFluidSimulation::addTemperature(ofTexture & _tex, float _strength){
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		temperatureFbo.swap();
		addMultipliedShader.update(temperatureFbo,
								   temperatureFbo.getBackTexture(),
								   _tex,
								   1.0,
								   _strength);
		ofPopStyle();
	}
	
	//--------------------------------------------------------------
	void ftFluidSimulation::addPressure(ofTexture& _tex, float _strength){
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		ftUtil::zero(addPressureFbo);
		
		addMultipliedShader.update(addPressureFbo,
								   addPressureFbo.getTexture(), // dubious
								   _tex,
								   1.0,
								   _strength);
		
		addPressureFboDidChange = true;
		ofPopStyle();
	}
	
	//--------------------------------------------------------------
	void ftFluidSimulation::addObstacle(ofTexture & _obstacleTexture){
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		ftUtil::stretch(obstacleFbo, _obstacleTexture);
		ofPopStyle();
	}
	
	//--------------------------------------------------------------
	void ftFluidSimulation::addTempObstacle(ofTexture & _obstacleTexture){
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		ftUtil::stretch(addTempObstacleFbo, _obstacleTexture);
		addTempObstacleFboDidChange = true;
		ofPopStyle();
	}
	
	//--------------------------------------------------------------
	void ftFluidSimulation::drawDensity(int _x, int _y, int _w, int _h){
		outputFbo.getTexture().draw(_x, _y, _w, _h);
	}
	
	//--------------------------------------------------------------
	void ftFluidSimulation::drawVelocity(int _x, int _y, int _w, int _h){
		if (toggleVisualisationField) { visualizeField.draw(inputFbo.getTexture(), _x, _y, _w, _h); }
		else { visualizeScalar.draw(inputFbo.getTexture(), _x, _y, _w, _h); }
	}
	
	//--------------------------------------------------------------
	void ftFluidSimulation::drawPressure(int _x, int _y, int _w, int _h) {
		if (toggleVisualisationField) { visualizeField.draw(pressureFbo.getTexture(), _x, _y, _w, _h); }
		else { visualizeScalar.draw(pressureFbo.getTexture(), _x, _y, _w, _h); }
	}
	
	//--------------------------------------------------------------
	void ftFluidSimulation::drawTemperature(int _x, int _y, int _w, int _h){
		if (toggleVisualisationField) { visualizeField.draw(temperatureFbo.getTexture(), _x, _y, _w, _h); }
		else { visualizeScalar.draw(temperatureFbo.getTexture(), _x, _y, _w, _h); }
	}
	
	//--------------------------------------------------------------
	void ftFluidSimulation::drawDivergence(int _x, int _y, int _w, int _h){
		if (toggleVisualisationField) { visualizeField.draw(divergenceFbo.getTexture(), _x, _y, _w, _h); }
		else { visualizeScalar.draw(divergenceFbo.getTexture(), _x, _y, _w, _h); }
	}
	
	//--------------------------------------------------------------
	void ftFluidSimulation::drawObstacles(int _x, int _y, int _w, int _h){
		combinedObstacleFbo.draw(_x, _y, _w, _h);
	}
	
	//--------------------------------------------------------------
	void ftFluidSimulation::drawVorticityVelocity(int _x, int _y, int _w, int _h){
		if (toggleVisualisationField) { visualizeField.draw(vorticityFirstPassFbo.getTexture(), _x, _y, _w, _h); }
		else { visualizeScalar.draw(vorticityFirstPassFbo.getTexture(), _x, _y, _w, _h); }
	}
	
	//--------------------------------------------------------------
	void ftFluidSimulation::drawConfinement(int _x, int _y, int _w, int _h){
		if (toggleVisualisationField) { visualizeField.draw(vorticitySecondPassFbo.getTexture(), _x, _y, _w, _h); }
		else { visualizeScalar.draw(vorticitySecondPassFbo.getTexture(), _x, _y, _w, _h); }
	}
	
	//--------------------------------------------------------------
	void ftFluidSimulation::drawBuoyancy(int _x, int _y, int _w, int _h){
		if (toggleVisualisationField) { visualizeField.draw(smokeBuoyancyFbo.getTexture(), _x, _y, _w, _h); }
		else { visualizeScalar.draw(smokeBuoyancyFbo.getTexture(), _x, _y, _w, _h); }
	}
	
	//--------------------------------------------------------------
	void ftFluidSimulation::reset() {
		ftFlow::reset();
		ftUtil::zero(pressureFbo);
		ftUtil::zero(temperatureFbo);
		createEdgeImage(obstacleFbo);
		combinedObstacleNeedsToBeCleaned = true;
	}
	
	//--------------------------------------------------------------
	void ftFluidSimulation::resetBackground() {
		ftUtil::zero(obstacleFbo);
		createEdgeImage(obstacleFbo);
		combinedObstacleNeedsToBeCleaned = true;
	}
	
	void ftFluidSimulation::createEdgeImage(ofFbo &_Fbo, int _edgeWidth, ofColor _backgroundColor, ofColor _edgeColor) {
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		_Fbo.begin();
		ofClear(_backgroundColor);
		ofSetColor(_edgeColor);
		ofDrawRectangle(_edgeWidth, _edgeWidth, _Fbo.getWidth() - _edgeWidth * 2, _Fbo.getHeight() - _edgeWidth * 2);
		_Fbo.end();
		ofPopStyle();
	}
}
