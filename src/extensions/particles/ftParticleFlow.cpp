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
		parameters.setName("particles");
		parameters.add(speed.set("speed", 20, 0, 100));
		cellSize.set("cell size", 1, 0.0, 2.0);
//		parameters.add(cellSize.set("cell size", 1, 0.0, 2.0));
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
	
	void ftParticleFlow::setup(int _simulationWidth, int _simulationHeight, int _densityWidth, int _densityHeight, int _numParticlesX, int _numParticlesY) {
		numParticlesX = _numParticlesX;
		numParticlesY = _numParticlesY;
		numParticles = (numParticlesX * numParticlesY);
		simulationWidth = _simulationWidth;
		
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);  // Why?
		
		ftFlow::allocate(_simulationWidth, _simulationHeight, GL_RG32F, _densityWidth, _densityHeight, GL_RGBA32F);
		
		flowVelocityFbo.allocate(_simulationWidth, _simulationHeight, GL_RG32F);
		ftUtil::zero(flowVelocityFbo);
		densityFbo.allocate(numParticlesX, numParticlesY, GL_RGBA32F);
		ftUtil::zero(densityFbo);
		obstacleFbo.allocate(_simulationWidth, _simulationHeight, GL_R8);
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
		settings.internalformat	= GL_RG32F;
		particlePositionFbo.allocate(settings);
		ftUtil::zero(particlePositionFbo);
		particleHomeFbo.allocate(numParticlesX, numParticlesY, GL_RG32F);
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
		float timeStep = _deltaTime * speed.get() * simulationWidth;
		
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
								  inputFbo.getTexture(),
								  particleHomeFbo.getTexture(),
								  timeStep,
								  cellSize.get(),
								  gravity);
		
		ofPopStyle();
		
//		ftUtil::zero(flowVelocityFbo);
//		ftUtil::zero(inputFbo);
//		ftUtil::zero(densityFbo);
//		ftUtil::zero(obstacleFbo);
	}
	
	void ftParticleFlow::reset() {
		ftUtil::zero(particlePositionFbo);
		ftUtil::zero(particleAgeLifespanMassSizeFbo);
	}
	
	void ftParticleFlow::drawOutput(int _x, int _y, int _w, int _h) {
		ofPushView();
		ofTranslate(_x, _y);
		ofScale(_w / (float)numParticlesX, _h / (float)numParticlesY);
		drawParticleShader.update(particleMesh, numParticles, particlePositionFbo.getTexture(), particleAgeLifespanMassSizeFbo.getTexture(), twinkleSpeed.get());
		ofPopView();
	}
	
	//--------------------------------------------------------------
	void ftParticleFlow::setFlow(flowTools::ftFlowForceType _type, ofTexture &_tex) {
		switch (_type) {
			case FT_VELOCITY_NORM:
				setFlowVelocity(_tex);
				break;
			case FT_VELOCITY:
				setFluidVelocity(_tex);
				break;
			case FT_DENSITY:
				setDensity(_tex);
				break;
			case FT_OBSTACLE:
				setObstacle(_tex);
			default:
				break;
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
			case FT_DENSITY:
				addDensity(_tex, _strength);
				break;
			case FT_OBSTACLE:
				addObstacle(_tex);
			default:
				break;
		}
	}
}
