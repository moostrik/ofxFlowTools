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
		
		int internalFormatVelocity = GL_RG32F;
		
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);  // Why?
		
		ftFlow::allocate(simulationWidth, simulationHeight, GL_RG32F);
		
		fluidVelocityFbo = inputFbo;
//		ftUtil::zero(fluidVelocityFbo);
		flowVelocityFbo.allocate(simulationWidth, simulationHeight, internalFormatVelocity);
		ftUtil::zero(flowVelocityFbo);
		densityFbo.allocate(simulationWidth, simulationHeight, GL_RGBA32F);
		ftUtil::zero(densityFbo);
		obstacleFbo.allocate(simulationWidth, simulationHeight, GL_R8); // GL_RED??
		ftUtil::zero(obstacleFbo);
		
		ofFboSettings settings;
		settings.width = numParticlesX;
		settings.height = numParticlesY;
		settings.minFilter	= GL_NEAREST;
		settings.maxFilter	= GL_NEAREST;
		settings.numColorbuffers = 2;
		settings.internalformat	= GL_RGBA32F;
		particleAgeLifespanMassSizeFbo.allocate(settings);
		ftUtil::zero(particleAgeLifespanMassSizeFbo);
		settings.internalformat	= internalFormatVelocity;
		particlePositionFbo.allocate(settings);
		ftUtil::zero(particlePositionFbo);
//		initPositionShader.update(*particlePositionFbo.getFbo());
//		particlePositionFbo.swap();
		particleHomeFbo.allocate(numParticlesX, numParticlesY, internalFormatVelocity);
		ftUtil::zero(particleHomeFbo);
		initPositionShader.update(particleHomeFbo);
		
		particleMesh.setMode(OF_PRIMITIVE_POINTS);
		for(int x = 0; x < _numParticlesX; x++){
			for(int y = 0; y < _numParticlesY; y++){
				particleMesh.addVertex(glm::vec3(x,y,0));
				particleMesh.addTexCoord(glm::vec2(x, y));
			}
		}
		
		ofPopStyle();
		
	}
	
	void ftParticleFlow::update(float _deltaTime) {
		
		if (bIsActive.get()) {
			float timeStep = _deltaTime * speed.get();
			
			ofPushStyle();
			ofEnableBlendMode(OF_BLENDMODE_DISABLED);
			
			
			particleAgeLifespanMassSizeFbo.swap();
			ALMSParticleShader.update(particleAgeLifespanMassSizeFbo,
									   particleAgeLifespanMassSizeFbo.getBackTexture(),
									   particlePositionFbo.getTexture(),
									   flowVelocityFbo.getTexture(),
									   densityFbo.getTexture(),
									   obstacleFbo.getTexture(),
									   _deltaTime,	// should this not be timeStep?
									   birthChance.get(),
									   birthVelocityChance.get(),
									   lifeSpan.get(), lifeSpanSpread.get(),
									   mass.get(), massSpread.get(),
									   size.get(), sizeSpread.get());
			
			particlePositionFbo.swap();
			moveParticleShader.update(particlePositionFbo,
									  particlePositionFbo.getBackTexture(),
									  particleAgeLifespanMassSizeFbo.getTexture(),
									  fluidVelocityFbo.getTexture(),
									  particleHomeFbo.getTexture(),
									  timeStep,
									  cellSize.get(),
									  gravity);
			
			ofPopStyle();
	 
			ftUtil::zero(flowVelocityFbo);
			ftUtil::zero(fluidVelocityFbo);
			ftUtil::zero(densityFbo);
			ftUtil::zero(obstacleFbo);
		}
	}
	
	void ftParticleFlow::reset() {
		ftFlow::reset();
		ftUtil::zero(particlePositionFbo);
		ftUtil::zero(particleAgeLifespanMassSizeFbo);
	}
	
	void ftParticleFlow::draw(int _x, int _y, int _width, int _height) {
		if (isActive()) {
			ofPushView();
			ofTranslate(_x, _y);
			ofScale(_width / (float)numParticlesX, _height / (float)numParticlesY);
			drawParticleShader.update(particleMesh, numParticles, particlePositionFbo.getTexture(), particleAgeLifespanMassSizeFbo.getTexture(), twinkleSpeed.get());
			ofPopView();
		}
	}
	
	//--------------------------------------------------------------
	void ftParticleFlow::addFlow(flowTools::ftFlowForceType _type, ofTexture &_tex, float _strength) {
		switch (_type) {
			case FT_VELOCITY_NORM:
				addFlowVelocity(_tex, _strength);
				break;
			case FT_VELOCITY:
				addFluidVelocity(_tex, _strength);
				break;
			case FT_OBSTACLE:
				setObstacle(_tex);
			default:
				break;
		}
	}
	void ftParticleFlow::addFlowVelocity(ofTexture & _tex, float _strength) {
		if (isActive()) {
			ofPushStyle();
			ofEnableBlendMode(OF_BLENDMODE_DISABLED);
			flowVelocityFbo.swap();
			addMultipliedShader.update(flowVelocityFbo,
									   flowVelocityFbo.getBackTexture(),
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
			fluidVelocityFbo.swap();
			addMultipliedShader.update(fluidVelocityFbo,
									   fluidVelocityFbo.getBackTexture(),
									   _tex,
									   1.0,
									   _strength);
			ofPopStyle();
		}
	}
	
	void ftParticleFlow::setObstacle (ofTexture& _tex) {
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		ftUtil::zero(obstacleFbo);
		obstacleFbo.begin();
		_tex.draw(0,0,simulationWidth,simulationHeight);
		obstacleFbo.end();
		ofPopStyle();
	}
}
