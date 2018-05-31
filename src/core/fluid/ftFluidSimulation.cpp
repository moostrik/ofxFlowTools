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
	void ftFluidSimulation::setup(int _simulationWidth, int _simulationHeight, int _densityWidth, int _densityHeight) {
		simulationWidth = _simulationWidth;
		simulationHeight = _simulationHeight;
		densityWidth = (!_densityWidth)? simulationWidth : _densityWidth;
		densityHeight = (!_densityHeight)? simulationHeight: _densityHeight;
		
		int	internalFormatDensity = GL_RGBA32F;
		int	internalFormatVelocity = GL_RG32F;
		int	interformatPressure = GL_R32F;
		int	internalFormatObstacle = GL_R8;
			
		densitySwapBuffer.allocate(densityWidth,densityHeight,internalFormatDensity);
		ftUtil::zero(densitySwapBuffer);
		velocitySwapBuffer.allocate(simulationWidth,simulationHeight,internalFormatVelocity);
		ftUtil::zero(velocitySwapBuffer);
		temperatureSwapBuffer.allocate(simulationWidth,simulationHeight,interformatPressure);
		ftUtil::zero(temperatureSwapBuffer);
		pressureSwapBuffer.allocate(simulationWidth,simulationHeight,interformatPressure);
		ftUtil::zero(pressureSwapBuffer);
		
		obstacleBuffer.allocate(simulationWidth, simulationHeight, internalFormatObstacle);
		ftUtil::zero(obstacleBuffer);
		createEdgeImage(obstacleBuffer);
		
		divergenceBuffer.allocate(simulationWidth, simulationHeight, interformatPressure);
		ftUtil::zero(divergenceBuffer);
		smokeBuoyancyBuffer.allocate(simulationWidth, simulationHeight, internalFormatVelocity);
		ftUtil::zero(smokeBuoyancyBuffer);
		vorticityFirstPassBuffer.allocate(simulationWidth, simulationHeight, internalFormatVelocity);
		ftUtil::zero(pressureSwapBuffer);
		vorticitySecondPassBuffer.allocate(simulationWidth, simulationHeight, internalFormatVelocity);
		ftUtil::zero(pressureSwapBuffer);
		addPressureBuffer.allocate(simulationWidth, simulationHeight, interformatPressure);
		ftUtil::zero(pressureSwapBuffer);
		addPressureBufferDidChange = false;
		addTempObstacleBuffer.allocate(simulationWidth, simulationHeight, internalFormatObstacle);
		ftUtil::zero(addTempObstacleBuffer);
		addTempObstacleBufferDidChange = false;
		combinedObstacleBuffer.allocate(simulationWidth, simulationHeight, internalFormatObstacle);
		ftUtil::zero(combinedObstacleBuffer);
		ftUtil::stretch(combinedObstacleBuffer, obstacleBuffer);
		
		deltaTime = 0;
		lastTime = 0;
		addTempObstacleBufferDidChange = true;
		
	}
	
	//--------------------------------------------------------------
	void ftFluidSimulation::update(float _deltaTime){
		float time = ofGetElapsedTimef();
		if (_deltaTime != 0)
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
		// OBSTACLE BUFFER;
		
		if (combinedObstacleNeedsToBeCleaned) {
			ftUtil::zero(combinedObstacleBuffer);
			ftUtil::stretch(combinedObstacleBuffer, obstacleBuffer);
			combinedObstacleNeedsToBeCleaned = false;
		}
		
		if (addTempObstacleBufferDidChange) {
			ofEnableBlendMode(OF_BLENDMODE_ADD);
			ftUtil::stretch(combinedObstacleBuffer, addTempObstacleBuffer);
			addTempObstacleBufferDidChange = false;
			ftUtil::zero(addTempObstacleBuffer);
			combinedObstacleNeedsToBeCleaned = true;
		}
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);

		// CLAMP LENGTH
//		if (maxDensity.get() > 0.0) {
//			densitySwapBuffer.swap();
//			clampLengthShader.update(*densitySwapBuffer.getBuffer(),
//									 densitySwapBuffer.getBackTexture(),
//									 maxDensity.get(),
//									 clampForce.get());
//		}
//		if (maxVelocity.get() > 0.0) {
//			velocitySwapBuffer.swap();
//			clampLengthShader.update(*velocitySwapBuffer.getBuffer(),
//									 velocitySwapBuffer.getBackTexture(),
//									 maxVelocity.get(),
//									 clampForce.get());
//		}
//		if (maxTemperature.get() > 0.0) {
//			temperatureSwapBuffer.swap();
//			clampLengthShader.update(*temperatureSwapBuffer.getBuffer(),
//									 temperatureSwapBuffer.getBackTexture(),
//									 maxTemperature.get(),
//									 clampForce.get());
//		}
//		if (maxPressure.get() > 0.0) {
//			pressureSwapBuffer.swap();
//			clampLengthShader.update(*pressureSwapBuffer.getBuffer(),
//									 pressureSwapBuffer.getBackTexture(),
//									 maxPressure.get(),
//									 clampForce.get());		}
		
		
		// VORTEX CONFINEMENT
		if (vorticity.get() > 0.0) {
			vorticityFirstPassShader.update(vorticityFirstPassBuffer,
											velocitySwapBuffer.getTexture(),
											combinedObstacleBuffer.getTexture());
			
			vorticitySecondPassShader.update(vorticitySecondPassBuffer,
											 vorticityFirstPassBuffer.getTexture(),
											 timeStep,
											 vorticity.get(),
											 cellSize.get());
			
			addVelocity(vorticitySecondPassBuffer.getTexture());
		}
		
		// ADVECT
		velocitySwapBuffer.swap();
		advectShader.update(velocitySwapBuffer,
							velocitySwapBuffer.getBackTexture(),
							velocitySwapBuffer.getBackTexture(),
							combinedObstacleBuffer.getTexture(),
							timeStep,
							1.0 - (dissipation.get() + velocityOffset.get()),
							cellSize.get());
		
		densitySwapBuffer.swap();
		advectShader.update(densitySwapBuffer,
							densitySwapBuffer.getBackTexture(),
							velocitySwapBuffer.getTexture(),
							combinedObstacleBuffer.getTexture(),
							timeStep,
							1.0 - (dissipation.get() + densityOffset.get()),
							cellSize.get());
		
//		advectShader.update(*pressureSwapBuffer.getBuffer(),
//							pressureSwapBuffer.getBackTexture(),
//							velocitySwapBuffer.getBackTexture(),
//							combinedObstacleBuffer.getTexture(),
//							timeStep,
//							1.0 - (dissipation.get() + pressureOffset.get()),
//							cellSize.get());

		
		// DIFFUSE
		if (viscosity.get() > 0.0) {
			for (int i = 0; i < numJacobiIterations.get(); i++) {
				velocitySwapBuffer.swap();
				diffuseShader.update(velocitySwapBuffer,
									 velocitySwapBuffer.getBackTexture(),
									 combinedObstacleBuffer.getTexture(),
//									 viscosity.get() * deltaTime); // works better than timeStep
									 viscosity.get() * timeStep);
			}
		 }
		 
		
		
		// SMOKE BUOYANCY -- UNSTABLE __ DISABLED FOR NOW
//		if (smokeSigma.get() > 0.0 && smokeWeight.get() > 0.0 ) {
//
//			temperatureSwapBuffer.swap();
//			advectShader.update(*temperatureSwapBuffer.getBuffer(),
//								temperatureSwapBuffer.getBackTexture(),
//								velocitySwapBuffer.getTexture(),
//								combinedObstacleBuffer.getTexture(),
//								timeStep,
//								1.0 - (dissipation.get() + temperatureOffset.get()), // WHY?
//								cellSize.get());
//			
//			smokeBuoyancyShader.update(smokeBuoyancyBuffer,
//									   velocitySwapBuffer.getTexture(),
//									   temperatureSwapBuffer.getTexture(),
//									   densitySwapBuffer.getTexture(),
//									   ambientTemperature.get(),
//									   timeStep,
//									   smokeSigma.get(),
//									   smokeWeight.get(),
//									   gravity.get() * timeStep);
//			addVelocity(smokeBuoyancyBuffer.getTexture());
//	
//		}
//		else
			ftUtil::zero(temperatureSwapBuffer);
		
		
		// DIVERGENCE AND JACOBI
		ftUtil::zero(divergenceBuffer);
		divergenceShader.update(divergenceBuffer,
								velocitySwapBuffer.getTexture(),
								combinedObstacleBuffer.getTexture(),
								cellSize.get());
		
		ftUtil::zero(pressureSwapBuffer);
		for (int i = 0; i < numJacobiIterations.get(); i++) {
			pressureSwapBuffer.swap();
			jacobiShader.update(pressureSwapBuffer,
								pressureSwapBuffer.getBackTexture(),
								divergenceBuffer.getTexture(),
								combinedObstacleBuffer.getTexture(),
								cellSize.get());
		}
		
		// Multiply density by pressure and or vorticity
		if(densityFromPressure != 0) {
			densitySwapBuffer.swap();
			densityFloatMultiplierShader.update(densitySwapBuffer,
												densitySwapBuffer.getBackTexture(),
												pressureSwapBuffer.getTexture(),
												densityFromPressure.get());
		}
		
		if(densityFromVorticity != 0) {
			densitySwapBuffer.swap();
			densityVec2MultiplierShader.update(densitySwapBuffer,
											   densitySwapBuffer.getBackTexture(),
											   vorticitySecondPassBuffer.getTexture(),
											   -densityFromVorticity.get());
		}
		
		
		// Drain some fluid
		if(addPressureBufferDidChange == true) {
			addPressureBufferDidChange = false;
			pressureSwapBuffer.swap();
			addMultipliedShader.update(pressureSwapBuffer,
									   pressureSwapBuffer.getTexture(),
									   addPressureBuffer.getTexture(),
									   1.0,
									   1.0);
		}
		
		velocitySwapBuffer.swap();
		substractGradientShader.update(velocitySwapBuffer,
									   velocitySwapBuffer.getBackTexture(),
									   pressureSwapBuffer.getTexture(),
									   combinedObstacleBuffer.getTexture(),
									   cellSize.get());
		
		ofPopStyle();
	}
	
	//--------------------------------------------------------------
	void ftFluidSimulation::addDensity(ofTexture & _tex, float _strength){
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		densitySwapBuffer.swap();
		addMultipliedShader.update(densitySwapBuffer,
								   densitySwapBuffer.getBackTexture(),
								   _tex,
								   1.0,
								   _strength);
		ofPopStyle();
	};
	
	//--------------------------------------------------------------
	void ftFluidSimulation::addVelocity(ofTexture & _tex, float _strength) {
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		velocitySwapBuffer.swap();
		addMultipliedShader.update(velocitySwapBuffer,
								   velocitySwapBuffer.getBackTexture(),
								   _tex,
								   1.0,
								   _strength);
		ofPopStyle();
	}
	
	//--------------------------------------------------------------
	void ftFluidSimulation::addTemperature(ofTexture & _tex, float _strength){
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		temperatureSwapBuffer.swap();
		addMultipliedShader.update(temperatureSwapBuffer,
								   temperatureSwapBuffer.getBackTexture(),
								   _tex,
								   1.0,
								   _strength);
		ofPopStyle();
	}
	
	//--------------------------------------------------------------
	void ftFluidSimulation::addPressure(ofTexture& _tex, float _strength){
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		ftUtil::zero(addPressureBuffer);
		
		addMultipliedShader.update(addPressureBuffer,
								   addPressureBuffer.getTexture(), // dubious
								   _tex,
								   1.0,
								   _strength);
		
		addPressureBufferDidChange = true;
		ofPopStyle();
	}
	
	//--------------------------------------------------------------
	void ftFluidSimulation::addObstacle(ofTexture & _obstacleTexture){
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		ftUtil::stretch(obstacleBuffer, _obstacleTexture);
		ofPopStyle();
	}
	
	//--------------------------------------------------------------
	void ftFluidSimulation::addTempObstacle(ofTexture & _obstacleTexture){
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		ftUtil::stretch(addTempObstacleBuffer, _obstacleTexture);
		addTempObstacleBufferDidChange = true;
		ofPopStyle();
	}
	
	//--------------------------------------------------------------
	void ftFluidSimulation::draw(int x, int y, float _width, float _height, ofBlendMode _blendmode){
		ofPushStyle();
		ofEnableBlendMode(_blendmode);
		densitySwapBuffer.getTexture().draw(x,y,_width,_height);
		ofPopStyle();
	}
	
	//--------------------------------------------------------------
	void ftFluidSimulation::reset() {
		
		ftUtil::zero(densitySwapBuffer);
		ftUtil::zero(velocitySwapBuffer);
		ftUtil::zero(pressureSwapBuffer);
		ftUtil::zero(temperatureSwapBuffer);
		createEdgeImage(obstacleBuffer);
		combinedObstacleNeedsToBeCleaned = true;
	}
	
	//--------------------------------------------------------------
	void ftFluidSimulation::resetBackground() {
		ftUtil::zero(obstacleBuffer);
		createEdgeImage(obstacleBuffer);
		combinedObstacleNeedsToBeCleaned = true;
	}
	
	void ftFluidSimulation::createEdgeImage(ofFbo &buffer, int _edgeWidth, ofColor _backgroundColor, ofColor _edgeColor) {
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		buffer.begin();
		ofClear(_backgroundColor);
		ofSetColor(_edgeColor);
		ofDrawRectangle(_edgeWidth, _edgeWidth, buffer.getWidth() - _edgeWidth * 2, buffer.getHeight() - _edgeWidth * 2);
		buffer.end();
		ofPopStyle();
	}
}
