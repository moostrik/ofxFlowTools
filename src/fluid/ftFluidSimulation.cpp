/*  ************************************************************************************
 *
 *  ftFluidSimulation
 *
 *  Created by Matthiasm Oostrik on 03/16.14.
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
		parameters.add(speed.set("speed", 10, 0, 100));
		parameters.add(cellSize.set("cell size", 1.25, 0.0, 2.0));
		parameters.add(numJacobiIterations.set("iterations", 40, 1, 100));
		parameters.add(viscosity.set("viscosity", 0.005, 0, 1));
		parameters.add(vorticity.set("vorticity", 0.1, 0.0, 1));
		parameters.add(dissipation.set("dissipation", 0.01f, 0, 0.02));
		advancedDissipationParameters.setName("advanced dissipation");
		advancedDissipationParameters.add(velocityOffset.set("velocity offset", 0, -0.01, 0.01));
		advancedDissipationParameters.add(densityOffset.set("density offset", 0, -0.01, 0.01));
		advancedDissipationParameters.add(temperatureOffset.set("temperature offset", 0, -0.01, 0.01));
		parameters.add(advancedDissipationParameters);
		smokeBuoyancyParameters.setName("smoke buoyancy");
		smokeBuoyancyParameters.add(smokeSigma.set("sigma", 0.05, 0.0, 0.1));
		smokeBuoyancyParameters.add(smokeWeight.set("weight", 0.05, 0.0, 1.0));
		smokeBuoyancyParameters.add(ambientTemperature.set("ambient temperature", 0.0, 0.0, 1.0));
		smokeBuoyancyParameters.add(gravity.set("gravity", ofVec2f(0,0), ofVec2f(-1,-1), ofVec2f(1,1)));
		parameters.add(smokeBuoyancyParameters);
		maxValues.setName("maximum");
		maxValues.add(clampForce.set("clampForce", 0, 0, .1));
		maxValues.add(maxDensity.set("density", 1,0,5));
		maxValues.add(maxVelocity.set("velocity", 1,0,10));
		maxValues.add(maxTemperature.set("temperature", 1,0,5));
		parameters.add(maxValues);
		parameters.add(densityFromPressure.set("density from pressure", 0, -0.1, 0.1));
		parameters.add(densityFromVorticity.set("density from vorticity", 0, -0.5, 0.5));
	}
	
	//--------------------------------------------------------------
	void ftFluidSimulation::setup(int _simulationWidth, int _simulationHeight, int _densityWidth, int _densityHeight, bool doFasterInternalFormat) {
		simulationWidth = _simulationWidth;
		simulationHeight = _simulationHeight;
		densityWidth = (!_densityWidth)? simulationWidth : _densityWidth;
		densityHeight = (!_densityHeight)? simulationHeight: _densityHeight;
		
		int internalFormatDensity, internalFormatVelocity, interformatPressure, internalFormatObstacle;
		if (doFasterInternalFormat) {	 // This gives errors with ofGLUtils, but it runs around 15% faster.
			internalFormatDensity = GL_RGBA32F;
			internalFormatVelocity = GL_RG32F;
			interformatPressure = GL_R32F;
			internalFormatObstacle = GL_RED;
			
		}
		else {							 // This gives no errors
			internalFormatDensity = GL_RGBA32F;
			internalFormatVelocity = GL_RGBA32F;
			interformatPressure = GL_RGBA32F;
			internalFormatObstacle = GL_RGB;
		}
		densitySwapBuffer.allocate(densityWidth,densityHeight,internalFormatDensity);
		densitySwapBuffer.clear();
		velocitySwapBuffer.allocate(simulationWidth,simulationHeight,internalFormatVelocity);
		velocitySwapBuffer.clear();
		temperatureSwapBuffer.allocate(simulationWidth,simulationHeight,interformatPressure);
		//		temperatureSwapBuffer.clear(ambientTemperature.get());
		temperatureSwapBuffer.clear();
		pressureSwapBuffer.allocate(simulationWidth,simulationHeight,interformatPressure);
		
		obstacleBuffer.allocate(simulationWidth, simulationHeight, internalFormatObstacle);
		obstacleBuffer.clear();
		createEdgeImage(obstacleBuffer);
		
		divergenceBuffer.allocate(simulationWidth, simulationHeight, internalFormatVelocity);
		smokeBuoyancyBuffer.allocate(simulationWidth, simulationHeight, internalFormatVelocity);
		smokeBuoyancyBuffer.clear();
		vorticityFirstPassBuffer.allocate(simulationWidth, simulationHeight, internalFormatVelocity);
		vorticitySecondPassBuffer.allocate(simulationWidth, simulationHeight, internalFormatVelocity);
		addPressureBuffer.allocate(simulationWidth, simulationHeight, interformatPressure);
		addPressureBufferDidChange = false;
		addTempObstacleBuffer.allocate(simulationWidth, simulationHeight, internalFormatObstacle);
		addTempObstacleBuffer.clear();
		addTempObstacleBufferDidChange = false;
		combinedObstacleBuffer.allocate(simulationWidth, simulationHeight, internalFormatObstacle);
		combinedObstacleBuffer.clear();
		combinedObstacleBuffer.scaleIntoMe(obstacleBuffer);
		
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
		timeStep = deltaTime * speed.get();
		
		if (doReset) {
			doReset.set(false);
			reset();
		}
		
		ofPushStyle();
		
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		// OBSTACLE BUFFER;
		
		if (combinedObstacleNeedsToBeCleaned) {
			combinedObstacleBuffer.clear();
			combinedObstacleBuffer.scaleIntoMe(obstacleBuffer);
			combinedObstacleNeedsToBeCleaned = false;
		}
		
		if (addTempObstacleBufferDidChange) {
			ofEnableBlendMode(OF_BLENDMODE_ADD);
			combinedObstacleBuffer.scaleIntoMe(addTempObstacleBuffer);
			addTempObstacleBufferDidChange = false;
			addTempObstacleBuffer.clear();
			combinedObstacleNeedsToBeCleaned = true;
		}
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);

		// CLAMP LENGTH
		if (maxDensity.get() > 0.0) {
			clampLengthShader.update(*densitySwapBuffer.dst,
									 densitySwapBuffer.src->getTextureReference(),
									 maxDensity.get(),
									 clampForce.get());
			densitySwapBuffer.swap();
		}
		if (maxVelocity.get() > 0.0) {
			clampLengthShader.update(*velocitySwapBuffer.dst,
									 velocitySwapBuffer.src->getTextureReference(),
									 maxVelocity.get(),
									 clampForce.get());
			velocitySwapBuffer.swap();
		}
		if (maxTemperature.get() > 0.0) {
			clampLengthShader.update(*temperatureSwapBuffer.dst,
									 temperatureSwapBuffer.src->getTextureReference(),
									 maxTemperature.get(),
									 clampForce.get());
			temperatureSwapBuffer.swap();
		}		
		
		
		// VORTEX CONFINEMENT
		if (vorticity.get() > 0.0) {
			vorticityFirstPassShader.update(vorticityFirstPassBuffer,
											velocitySwapBuffer.src->getTextureReference(),
											combinedObstacleBuffer.getTextureReference());
			
			vorticitySecondPassShader.update(vorticitySecondPassBuffer,
											 vorticityFirstPassBuffer.getTextureReference(),
											 timeStep,
											 vorticity.get(),
											 cellSize.get());
			
			addVelocity(vorticitySecondPassBuffer.getTextureReference());
		}
		
		// ADVECT
		advectShader.update(*velocitySwapBuffer.dst,
							velocitySwapBuffer.src->getTextureReference(),
							velocitySwapBuffer.src->getTextureReference(),
							combinedObstacleBuffer.getTextureReference(),
							timeStep,
							1.0 - (dissipation.get() + velocityOffset.get()),
							cellSize.get());
		velocitySwapBuffer.swap();
		
		
		advectShader.update(*densitySwapBuffer.dst,
							densitySwapBuffer.src->getTextureReference(),
							velocitySwapBuffer.src->getTextureReference(),
							combinedObstacleBuffer.getTextureReference(),
							timeStep,
							1.0 - (dissipation.get() + densityOffset.get()),
							cellSize.get());
		densitySwapBuffer.swap();
		
		
		
		// DIFFUSE
		if (viscosity.get() > 0.0) {
			for (int i = 0; i < numJacobiIterations.get(); i++) {
				diffuseShader.update(*velocitySwapBuffer.dst,
									 velocitySwapBuffer.src->getTextureReference(),
									 combinedObstacleBuffer.getTextureReference(),
									 viscosity.get() * deltaTime);
				velocitySwapBuffer.swap();
			}
		 }
		 
		
		
		// SMOKE BUOYANCY
		if (smokeSigma.get() > 0.0 && smokeWeight.get() > 0.0 ) {
			
			advectShader.update(*temperatureSwapBuffer.dst,
								temperatureSwapBuffer.src->getTextureReference(),
								velocitySwapBuffer.src->getTextureReference(),
								combinedObstacleBuffer.getTextureReference(),
								timeStep,
								1.0 - (dissipation.get() + temperatureOffset.get()),
								cellSize.get());
			temperatureSwapBuffer.swap();
			
			smokeBuoyancyShader.update(smokeBuoyancyBuffer,
									   velocitySwapBuffer.src->getTextureReference(),
									   temperatureSwapBuffer.src->getTextureReference(),
									   densitySwapBuffer.src->getTextureReference(),
									   ambientTemperature.get(),
									   timeStep,
									   smokeSigma.get(),
									   smokeWeight.get(),
									   gravity.get());
			addVelocity(smokeBuoyancyBuffer.getTextureReference());
	
		}
		else
			temperatureSwapBuffer.clear();
		
		
		// DIVERGENCE AND JACOBI
		divergenceBuffer.clear();
		divergenceShader.update(divergenceBuffer,
								velocitySwapBuffer.src->getTextureReference(),
								combinedObstacleBuffer.getTextureReference(),
								cellSize.get());
		
		pressureSwapBuffer.clear();
		for (int i = 0; i < numJacobiIterations.get(); i++) {
			jacobiShader.update(*pressureSwapBuffer.dst,
								pressureSwapBuffer.src->getTextureReference(),
								divergenceBuffer.getTextureReference(),
								combinedObstacleBuffer.getTextureReference(),
								cellSize.get());
			pressureSwapBuffer.swap();
		}
		
		// Multiply density by pressure and or vorticity
		if(densityFromPressure != 0) {
			densityFloatMultiplierShader.update(*densitySwapBuffer.dst,
												densitySwapBuffer.src->getTextureReference(),
												pressureSwapBuffer.src->getTextureReference(),
												densityFromPressure.get());
			densitySwapBuffer.swap();
		}
		
		if(densityFromVorticity != 0) {
			densityVec2MultiplierShader.update(*densitySwapBuffer.dst,
											   densitySwapBuffer.src->getTextureReference(),
											   vorticitySecondPassBuffer.getTextureReference(),
											   -densityFromVorticity.get());
			densitySwapBuffer.swap();
		}
		
		
		// Drain some fluid
		if(addPressureBufferDidChange == true) {
			addPressureBufferDidChange = false;
			addShader.update(*pressureSwapBuffer.dst,
							 pressureSwapBuffer.src->getTextureReference(),
							 addPressureBuffer.getTextureReference(),
							 1.0);
			pressureSwapBuffer.swap();
		}
		
		substractGradientShader.update(*velocitySwapBuffer.dst,
									   velocitySwapBuffer.src->getTextureReference(),
									   pressureSwapBuffer.src->getTextureReference(),
									   combinedObstacleBuffer.getTextureReference(),
									   cellSize.get());
		velocitySwapBuffer.swap();
		
		ofPopStyle();
	}
	
	//--------------------------------------------------------------
	void ftFluidSimulation::addDensity(ofTexture & _tex, float _strength){
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		addShader.update(*densitySwapBuffer.dst,
						 densitySwapBuffer.src->getTextureReference(),
						 _tex,
						 _strength);
		densitySwapBuffer.swap();
		ofPopStyle();
	};
	
	//--------------------------------------------------------------
	void ftFluidSimulation::addVelocity(ofTexture & _tex, float _strength) {
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		addShader.update(*velocitySwapBuffer.dst,
						 velocitySwapBuffer.src->getTextureReference(),
						 _tex,
						 _strength);
		velocitySwapBuffer.swap();
		ofPopStyle();
	}
	
	//--------------------------------------------------------------
	void ftFluidSimulation::addTemperature(ofTexture & _tex, float _strength){
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		addShader.update(*temperatureSwapBuffer.dst,
						 temperatureSwapBuffer.src->getTextureReference(),
						 _tex,
						 _strength);
		temperatureSwapBuffer.swap();
		ofPopStyle();
	}
	
	//--------------------------------------------------------------
	void ftFluidSimulation::addPressure(ofTexture& _tex, float _strength){
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		addPressureBuffer.clear();
		
		addShader.update(addPressureBuffer,
						 addPressureBuffer.getTextureReference(), // dubious
						 _tex,
						 _strength);
		
		addPressureBufferDidChange = true;
		ofPopStyle();
	}
	
	//--------------------------------------------------------------
	void ftFluidSimulation::addObstacle(ofTexture & _obstacleTexture){
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		obstacleBuffer.scaleIntoMe(_obstacleTexture);
		ofPopStyle();
	}
	
	//--------------------------------------------------------------
	void ftFluidSimulation::addTempObstacle(ofTexture & _obstacleTexture){
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		addTempObstacleBuffer.scaleIntoMe(_obstacleTexture);
		addTempObstacleBufferDidChange = true;
		ofPopStyle();
	}
	
	//--------------------------------------------------------------
	void ftFluidSimulation::draw(int x, int y, float _width, float _height){
		if (_width == -1) _width = densityWidth;
		if (_height == -1) _height = densityHeight;
		
		densitySwapBuffer.src->draw(x,y,_width,_height);
	}
	
	//--------------------------------------------------------------
	void ftFluidSimulation::reset() {
		
		densitySwapBuffer.clear();
		velocitySwapBuffer.clear();
		temperatureSwapBuffer.clear();
		createEdgeImage(obstacleBuffer);
		combinedObstacleNeedsToBeCleaned = true;
	}
	
	void ftFluidSimulation::createEdgeImage(ftFbo &buffer, int _edgeWidth, ofColor _backgroundColor, ofColor _edgeColor) {
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		buffer.begin();
		ofClear(_backgroundColor);
		ofSetColor(_edgeColor);
		ofRect(_edgeWidth, _edgeWidth, buffer.getWidth() - _edgeWidth * 2, buffer.getHeight() - _edgeWidth * 2);
		buffer.end();
		ofPopStyle();
	}
}
