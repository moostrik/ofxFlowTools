/*  ************************************************************************************
 *
 *  ftFluidFlow
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

#include "ftFluidFlow.h"

namespace flowTools {
	
	ftFluidFlow::ftFluidFlow(){
		parameters.setName("fluid");
		parameters.add(speed.set("speed", .5, 0, 1));
		parameters.add(numJacobiIterations.set("iterations", 40, 1, 100));
		parameters.add(viscosity.set("viscosity", 0.0, 0, 1));
		parameters.add(vorticity.set("vorticity", 0.0, 0.0, 1));
		dissipationParameters.setName("dissipation");
		dissipationParameters.add(dissipationVel.set("velocity",0.0015, 0, 0.01));
		dissipationParameters.add(dissipationDen.set("density", 0.0015, 0, 0.01));
		dissipationParameters.add(dissipationPrs.set("pressure",0.025, 0, 0.1));
		parameters.add(dissipationParameters);
		smokeBuoyancyParameters.setName("smoke buoyancy");
		smokeBuoyancyParameters.add(smokeSigma.set("buoyancy", 0.5, 0.0, 1.0));
		smokeBuoyancyParameters.add(smokeWeight.set("weight", 0.05, 0.0, 1.0));
		smokeBuoyancyParameters.add(ambientTemperature.set("ambient temperature", 0.75, 0.0, 1.0));
		smokeBuoyancyParameters.add(gravity.set("gravity", ofDefaultVec2(0., -0.980665), ofDefaultVec2(-1, -1), ofDefaultVec2(1, 1)));
		parameters.add(smokeBuoyancyParameters);
//		parameters.add(wrap.set("wrap", false));
//		wrap.addListener(this, &ftFluidFlow::wrapListener);
		
	}
	
	//--------------------------------------------------------------
	void ftFluidFlow::setup(int _simulationWidth, int _simulationHeight, int _densityWidth, int _densityHeight) {
		allocate(_simulationWidth, _simulationHeight, GL_RG32F, _densityWidth, _densityHeight, GL_RGBA32F);
//		if(wrap.get()) { advectShader = &advectWrapShader; } else { advectShader = &advectNoWrapShader; }
		advectShader = &advectNoWrapShader;
	}
	
	void ftFluidFlow::allocate(int _simulationWidth, int _simulationHeight, GLint _simulationInternalFormat, int _densityWidth, int _densityHeight, GLint _densityInternalFormat) {
		simulationWidth = _simulationWidth;
		simulationHeight = _simulationHeight;
		densityWidth = _densityWidth;
		densityHeight = _densityHeight;
		
		ftFlow::allocate(simulationWidth, simulationHeight, _simulationInternalFormat, densityWidth, densityHeight, _densityInternalFormat);
		
		visualizationField.setup(simulationWidth, simulationHeight);
		
		temperatureFbo.allocate(simulationWidth,simulationHeight,GL_R32F);
		ftUtil::zero(temperatureFbo);
		pressureFbo.allocate(simulationWidth,simulationHeight,GL_R32F);
		ftUtil::zero(pressureFbo);
		obstacleFbo.allocate(simulationWidth, simulationHeight, GL_R8);
		ftUtil::zero(obstacleFbo);
		edgeFbo.allocate(simulationWidth, simulationHeight, GL_RGB32F);
		ftUtil::zero(edgeFbo);
		edgesFromObstacleShader.update(edgeFbo, obstacleFbo.getTexture());
		divergenceFbo.allocate(simulationWidth, simulationHeight, GL_R32F);
		ftUtil::zero(divergenceFbo);
		smokeBuoyancyFbo.allocate(simulationWidth, simulationHeight, GL_RG32F);
		ftUtil::zero(smokeBuoyancyFbo);
		vorticityVelocityFbo.allocate(simulationWidth, simulationHeight, GL_RG32F);
		ftUtil::zero(pressureFbo);
		vorticityConfinementFbo.allocate(simulationWidth, simulationHeight, GL_RG32F);
		ftUtil::zero(pressureFbo);
		
		initObstacle();
		
	}
	
	//--------------------------------------------------------------
	void ftFluidFlow::update(float _deltaTime){
		float timeStep = _deltaTime * speed.get() * simulationWidth;
		
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);

		ftPingPongFbo& velocityFbo = inputFbo;
		ftPingPongFbo& densityFbo = outputFbo;
		
		// ADVECT
		velocityFbo.swap();
		edgesNegativeShader.update(velocityFbo.get(), velocityFbo.getBackTexture(), edgeFbo.getTexture());
		velocityFbo.swap();
		advectShader->update(velocityFbo.get(), velocityFbo.getBackTexture(), velocityFbo.getBackTexture(), timeStep, 1.0 - dissipationVel.get());
//		velocityFbo.swap();
//		edgesNegativeShader.update(velocityFbo.get(), velocityFbo.getBackTexture(), edgeFbo.getTexture());
		
		
		// ADD FORCES: VORTEX CONFINEMENT
		if (vorticity.get() > 0.0) {
			vorticityVelocityShader.update(vorticityVelocityFbo.get(), velocityFbo.getTexture(), edgeFbo.getTexture());
//			float vorticityConfinement = timeStep * vorticity.get() / (simulationWidth * simulationHeight) * 1000000.0;
			float vorticityConfinement = (timeStep * vorticity.get() * 200.0) / simulationWidth;
			vorticityConfinementShader.update(vorticityConfinementFbo, vorticityVelocityFbo.getTexture(), vorticityConfinement);
			addVelocity(vorticityConfinementFbo.getTexture());
//			velocityFbo.swap();
//			edgesNegativeShader.update(velocityFbo.get(), velocityFbo.getBackTexture(), edgeFbo.getTexture());
		}
		
		// ADD FORCES: DIFFUSE
		if (viscosity.get() > 0.0) {
			float viscosityStep = timeStep * viscosity.get();// * 1000;
			for (int i = 0; i < numJacobiIterations.get(); i++) {
				velocityFbo.swap();
				diffuseShader.update(velocityFbo.get(), velocityFbo.getBackTexture(), viscosityStep);
			}
			//			velocityFbo.swap();
			//			edgesNegativeShader.update(velocityFbo.get(), velocityFbo.getBackTexture(), edgeFbo.getTexture());
		}
		
		// ADD FORCES:  SMOKE BUOYANCY
		if (smokeSigma.get() > 0.0 && smokeWeight.get() > 0.0 ) {
			temperatureFbo.swap();
			advectShader->update(temperatureFbo.get(), temperatureFbo.getBackTexture(), velocityFbo.getTexture(),  timeStep, 1.0 - dissipationDen.get());
			temperatureFbo.swap();
			edgesNegativeShader.update(temperatureFbo.get(), temperatureFbo.getBackTexture(), edgeFbo.getTexture());
			ftUtil::zero(smokeBuoyancyFbo);
			smokeBuoyancyShader.update(smokeBuoyancyFbo, temperatureFbo.getTexture(), densityFbo.getTexture(), ambientTemperature.get(), timeStep, smokeSigma.get(), smokeWeight.get(), gravity.get());
			addVelocity(smokeBuoyancyFbo.getTexture());
			velocityFbo.swap();
			edgesNegativeShader.update(velocityFbo.get(), velocityFbo.getBackTexture(), edgeFbo.getTexture());
		}
		else {
			ftUtil::zero(temperatureFbo);
		}
		
		// PRESSURE: DIVERGENCE
		divergenceShader.update(divergenceFbo, velocityFbo.getTexture());
		
		// PRESSURE: JACOBI
		pressureFbo.swap();
		multiplyForceShader.update(pressureFbo.get(), pressureFbo.getBackTexture(), 1.0 - dissipationPrs.get());
		for (int i = 0; i < numJacobiIterations.get(); i++) {
			pressureFbo.swap();
			jacobiEdgeShader.update(pressureFbo.get(), pressureFbo.getBackTexture(), divergenceFbo.getTexture(), edgeFbo.getTexture());
		}
//		pressureFbo.swap();
//		edgesPositiveShader.update(pressureFbo.get(), pressureFbo.getBackTexture(), edgeFbo.getTexture());
		
		// PRESSURE: SUBSTRACT GRADIENT
		velocityFbo.swap();
		substractGradientShader.update(velocityFbo.get(), velocityFbo.getBackTexture(), pressureFbo.getTexture());
//		velocityFbo.swap();
//		edgesNegativeShader.update(velocityFbo.get(), velocityFbo.getBackTexture(), edgeFbo.getTexture());
		
		// DENSITY:
		densityFbo.swap();
		advectShader->update(densityFbo.get(), densityFbo.getBackTexture(), velocityFbo.getTexture(), timeStep, 1.0 - dissipationDen.get());
		densityFbo.swap();
		obstacleShader.update(densityFbo.get(), densityFbo.getBackTexture(), obstacleFbo.getTexture());
		
		ofPopStyle();
	}
	
	//--------------------------------------------------------------
	void ftFluidFlow::setFlow(flowTools::ftFlowForceType _type, ofTexture &_tex) {
		switch (_type) {
			case FT_VELOCITY:		setVelocity(_tex);		break;
			case FT_DENSITY:		setDensity(_tex);		break;
			case FT_TEMPERATURE:	setTemperature(_tex);	break;
			case FT_PRESSURE:		setPressure(_tex);		break;
			case FT_OBSTACLE:		setObstacle(_tex);		break;
			default:
				ofLogWarning("ftFluidFlow: addFlow") << "no method to add flow of type " << _type;
				break;
		}
	}
	
	//--------------------------------------------------------------
	void ftFluidFlow::addFlow(flowTools::ftFlowForceType _type, ofTexture &_tex, float _strength) {
		switch (_type) {
			case FT_VELOCITY:		addVelocity(_tex, _strength);		break;
			case FT_DENSITY:		addDensity(_tex, _strength);		break;
			case FT_TEMPERATURE:	addTemperature(_tex, _strength);	break;
			case FT_PRESSURE:		addPressure(_tex, _strength);		break;
			case FT_OBSTACLE:		addObstacle(_tex);					break;
			default:
				ofLogWarning("ftFluidFlow: addFlow") << "no method to add flow of type " << _type;
				break;
		}
	}
	
	//--------------------------------------------------------------
	void ftFluidFlow::initObstacle(){
//		if (wrap.get()) {
//			ftUtil::zero(obstacleFbo);
//		}
//		else { // create edge
			ofPushStyle();
			ofEnableBlendMode(OF_BLENDMODE_DISABLED);
			ftUtil::one(obstacleFbo);
			obstacleFbo.begin();
			ofSetColor(0,0,0,255);
			int borderSize = 1;
			ofDrawRectangle(borderSize, borderSize, obstacleFbo.getWidth()-borderSize*2, obstacleFbo.getHeight()-borderSize*2);
			obstacleFbo.end();
			ofPopStyle();
//		}
		
		edgesFromObstacleShader.update(edgeFbo, obstacleFbo.getTexture());
	}
	
	//--------------------------------------------------------------
	void ftFluidFlow::setObstacle(ofTexture & _tex){
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		initObstacle();
		addBooleanShader.update(obstacleFbo.get(), obstacleFbo.getBackTexture(), _tex);
		ftUtil::zero(edgeFbo);
		edgesFromObstacleShader.update(edgeFbo, obstacleFbo.getTexture());
		ofPopStyle();
	}
	
	//--------------------------------------------------------------
	void ftFluidFlow::addObstacle(ofTexture & _tex){
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		obstacleFbo.swap();
		addBooleanShader.update(obstacleFbo.get(), obstacleFbo.getBackTexture(), _tex);
		edgesFromObstacleShader.update(edgeFbo, obstacleFbo.getTexture());
		ofPopStyle();
	}
	
	//--------------------------------------------------------------
	void ftFluidFlow::reset() {
		
		ftFlow::reset();
		ftUtil::zero(pressureFbo);
		ftUtil::zero(temperatureFbo);
		ftUtil::zero(divergenceFbo);
		ftUtil::zero(vorticityVelocityFbo);
		ftUtil::zero(vorticityConfinementFbo);
		ftUtil::zero(smokeBuoyancyFbo);
		
		initObstacle();
		
		jacobiEdgeShader = ftJacobiEdgeShader();
	}
}
