/*  ************************************************************************************
 *
 *  ftParticleFlow
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
 *  ************************************************************************************ */

#include "ftParticleFlow.h"

namespace flowTools {
	
	ftParticleFlow::ftParticleFlow(){
		parameters.setName("particle flow");
		parameters.add(bIsActive.set("active", true));
		parameters.add(speed.set("speed", 20, 0, 100));
		parameters.add(cellSize.set("cell size", 1.25, 0.0, 2.0));
		parameters.add(birthChance.set("birth chance", 0.5, 0, 1));
		parameters.add(birthVelocityChance.set("birth velocity chance", 0.5, 0, 1));
		parameters.add(lifeSpan.set("lifespan", 5, 0, 10));
		parameters.add(lifeSpanSpread.set("lifespan spread", .25, 0, 1));
		parameters.add(mass.set("mass", 1.3, 0, 2));
		parameters.add(massSpread.set("mass spread", .2, 0, 1));
		parameters.add(size.set("size", 2, 0, 10));
		parameters.add(sizeSpread.set("size spread", .75, 0, 1));
		parameters.add(twinkleSpeed.set("twinkle speed", 11, 0, 20));
		parameters.add(gravity.set("gravity", glm::vec2(0, 0), glm::vec2(-10,-10), glm::vec2(10,10)));
	}
	
	void ftParticleFlow::setup(int _simulationWidth, int _simulationHeight, int _numParticlesX, int _numParticlesY) {
		simulationWidth = _simulationWidth;
		simulationHeight = _simulationHeight;
		numParticlesX = _numParticlesX;
		numParticlesY = _numParticlesY;
		numParticles = (numParticlesX * numParticlesY);
				
		particleMesh.setMode(OF_PRIMITIVE_POINTS);
		for(int x = 0; x < _numParticlesX; x++){
			for(int y = 0; y < _numParticlesY; y++){
				particleMesh.addVertex(ofVec3f(x,y));
				particleMesh.addTexCoord(ofVec2f(x, y));
			}
		}
		
		int internalFormatVelocity = GL_RG32F;
		
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);  // Why?
		
		ofFboSettings settings;
		settings.width = numParticlesX;
		settings.height = numParticlesY;
		settings.minFilter	= GL_NEAREST;
		settings.maxFilter	= GL_NEAREST;
		settings.numColorbuffers = 2;
		
		settings.internalformat	= GL_RGBA32F;
		particleAgeLifespanMassSizeSwapBuffer.allocate(settings);
		ftUtil::zero(particleAgeLifespanMassSizeSwapBuffer);
		
		settings.internalformat	= internalFormatVelocity;
		particlePositionSwapBuffer.allocate(settings);
		ftUtil::zero(particlePositionSwapBuffer);
//		initPositionShader.update(*particlePositionSwapBuffer.getBuffer());
		particlePositionSwapBuffer.swap();
		particleHomeBuffer.allocate(numParticlesX, numParticlesY, internalFormatVelocity);
		ftUtil::zero(particleHomeBuffer);
		initPositionShader.update(particleHomeBuffer);
		
		fluidVelocitySwapBuffer.allocate(simulationWidth, simulationHeight, internalFormatVelocity);
		ftUtil::zero(fluidVelocitySwapBuffer);
		flowVelocitySwapBuffer.allocate(simulationWidth, simulationHeight, internalFormatVelocity);
		ftUtil::zero(flowVelocitySwapBuffer);
		densitySwapBuffer.allocate(simulationWidth, simulationHeight, GL_RGBA32F);
		ftUtil::zero(densitySwapBuffer);
		obstacleBuffer.allocate(simulationWidth, simulationHeight, GL_RGB); // GL_RED??
		ftUtil::zero(obstacleBuffer);
		
		ofPopStyle();
		
	}
	
	void ftParticleFlow::update(float _deltaTime) {
		float time = ofGetElapsedTimef();
		if (_deltaTime != 0)
			deltaTime = _deltaTime;
		else
			deltaTime = time - lastTime;
		lastTime = time;
		
		if (bIsActive.get()) {
			timeStep = deltaTime * speed.get();
			
			ofPushStyle();
			ofEnableBlendMode(OF_BLENDMODE_DISABLED);
			
			
			particleAgeLifespanMassSizeSwapBuffer.swap();
			ALMSParticleShader.update(particleAgeLifespanMassSizeSwapBuffer,
									   particleAgeLifespanMassSizeSwapBuffer.getBackTexture(),
									   particlePositionSwapBuffer.getTexture(),
									   flowVelocitySwapBuffer.getTexture(),
									   densitySwapBuffer.getTexture(),
									   obstacleBuffer.getTexture(),
									   deltaTime,
									   birthChance.get(),
									   birthVelocityChance.get(),
									   lifeSpan.get(), lifeSpanSpread.get(),
									   mass.get(), massSpread.get(),
									   size.get(), sizeSpread.get());
			
			particlePositionSwapBuffer.swap();
			moveParticleShader.update(particlePositionSwapBuffer,
									  particlePositionSwapBuffer.getBackTexture(),
									  particleAgeLifespanMassSizeSwapBuffer.getTexture(),
									  fluidVelocitySwapBuffer.getTexture(),
									  particleHomeBuffer.getTexture(),
									  timeStep,
									  cellSize.get(),
									  gravity);
			
			ofPopStyle();
	 
			ftUtil::zero(flowVelocitySwapBuffer);
			ftUtil::zero(fluidVelocitySwapBuffer);
			ftUtil::zero(densitySwapBuffer);
			ftUtil::zero(obstacleBuffer);
		}
	}
	
	void ftParticleFlow::draw(int _x, int _y, int _width, int _height, ofBlendMode _blendmode) {
		if (isActive()) {
			ofPushStyle();
			ofPushView();
			ofEnableBlendMode(_blendmode);
			ofTranslate(_x, _y);
			ofScale(_width / numParticlesX, _height / numParticlesY);
			drawParticleShader.update(particleMesh, numParticles, particlePositionSwapBuffer.getTexture(), particleAgeLifespanMassSizeSwapBuffer.getTexture(), twinkleSpeed.get());
			
			ofPopView();
			ofPopStyle();
		}
	}
	
	//--------------------------------------------------------------
	void ftParticleFlow::addFlow(flowTools::ftFlowType _type, ofTexture &_tex, float _strength) {
		switch (_type) {
			case FT_CORE_FLOW_VELOCITY:
				addFlowVelocity(_tex, _strength);
				break;
			case FT_CORE_FLUID_VELOCITY:
				addFluidVelocity(_tex, _strength);
				break;
			case FT_CORE_OBSTACLE_CONSTANT:
				setObstacle(_tex);
			default:
				break;
		}
	}
	void ftParticleFlow::addFlowVelocity(ofTexture & _tex, float _strength) {
		if (isActive()) {
			ofPushStyle();
			ofEnableBlendMode(OF_BLENDMODE_DISABLED);
			flowVelocitySwapBuffer.swap();
			addMultipliedShader.update(flowVelocitySwapBuffer,
									   flowVelocitySwapBuffer.getBackTexture(),
									   _tex,
									   1.0,
									   _strength);
			ofPopStyle();
		}
	}
	
	void ftParticleFlow::addFluidVelocity (ofTexture& _tex, float _strength) {
		if (isActive()) {
			ofPushStyle();
			ofEnableBlendMode(OF_BLENDMODE_DISABLED);
			fluidVelocitySwapBuffer.swap();
			addMultipliedShader.update(fluidVelocitySwapBuffer,
									   fluidVelocitySwapBuffer.getBackTexture(),
									   _tex,
									   1.0,
									   _strength);
			ofPopStyle();
		}
	}
	
	void ftParticleFlow::setObstacle (ofTexture& _tex) {
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		ftUtil::zero(obstacleBuffer);
		obstacleBuffer.begin();
		_tex.draw(0,0,simulationWidth,simulationHeight);
		obstacleBuffer.end();
		ofPopStyle();
	}
}
