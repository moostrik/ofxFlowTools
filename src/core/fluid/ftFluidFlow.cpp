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
		parameters.add(speed.set("speed"							, 0.3, 0.0, 1.0));
		parameters.add(gridScale.set("gridScale"					, 1  , 1  , 100));
		dissipationParameters.setName("dissipation");
		dissipationParameters.add(dissipationVel.set("velocity"		, 0.1, 0.0, 1.0));
		dissipationParameters.add(dissipationDen.set("density"		, 0.1, 0.0, 1.0));
		dissipationParameters.add(dissipationTmp.set("temperature"	, 0.1, 0.0, 1.0));
		dissipationParameters.add(dissipationPrs.set("pressure"		, 0.1, 0.0, 1.0));
		parameters.add(dissipationParameters);
		viscosityParameters.setName("viscosity");
		viscosityParameters.add(viscosityVel.set("velocity"			, 0.5, 0.0, 1.0));
		viscosityParameters.add(viscosityDen.set("density"			, 0.0, 0.0, 1.0));
		viscosityParameters.add(viscosityTmp.set("temperature"		, 0.0, 0.0, 1.0));
		parameters.add(viscosityParameters);
		parameters.add(vorticity.set("vorticity"					, 1.0, 0.0, 1.0));
		buoyancyParameters.setName("smoke buoyancy");
		buoyancyParameters.add(buoyancySigma.set("buoyancy"			, 0.6, 0.0, 1.0));
		buoyancyParameters.add(buoyancyWeight.set("weight"			, 0.2, 0.0, 1.0));
		buoyancyParameters.add(buoyancyAmbientTemperature.set("ambient temperature", 0.2, 0.0, 1.0));
		parameters.add(buoyancyParameters);
		
		numJacobiIterationsProjection = 40;
		numJacobiIterationsDiffuse = 20;
//		gridScale = 1;
	}
	
	//--------------------------------------------------------------
	void ftFluidFlow::setup(int _simulationWidth, int _simulationHeight, int _densityWidth, int _densityHeight) {
		allocate(_simulationWidth, _simulationHeight, GL_RG32F, _densityWidth, _densityHeight, GL_RGBA32F);
	}
	
	//--------------------------------------------------------------
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
		divergenceFbo.allocate(simulationWidth, simulationHeight, GL_R32F);
		ftUtil::zero(divergenceFbo);
		vorticityCurlFbo.allocate(simulationWidth, simulationHeight, GL_RG32F);
		ftUtil::zero(vorticityCurlFbo);
		vorticityForceFbo.allocate(simulationWidth, simulationHeight, GL_RG32F);
		ftUtil::zero(vorticityForceFbo);
		buoyancyFbo.allocate(simulationWidth, simulationHeight, GL_RG32F);
		ftUtil::zero(buoyancyFbo);
		obstacleFbo.allocate(simulationWidth, simulationHeight, GL_R8);
		ftUtil::zero(obstacleFbo);
		obstacleOffsetFbo.allocate(simulationWidth, simulationHeight, GL_RGBA8);
		ftUtil::zero(obstacleOffsetFbo);
		
		initObstacle();
		obstacleOffsetShader.update(obstacleOffsetFbo, obstacleFbo.getTexture());
	}
	
	//--------------------------------------------------------------
	void ftFluidFlow::update(float _deltaTime){
		float timeStep = speed.get() * 5;
		
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);

		ftPingPongFbo& velocityFbo = inputFbo;
		ftPingPongFbo& densityFbo = outputFbo;
		
		// ADVECT & DISSEPATE
		float disVel = 1.0 - _deltaTime * dissipationVel.get();
		velocityFbo.swap();
		advectShader.update(velocityFbo.get(), velocityFbo.getBackTexture(), velocityFbo.getBackTexture(), obstacleFbo.getTexture(), gridScale, timeStep, disVel);
		
		float disDen = 1.0 - _deltaTime * dissipationDen.get();
		densityFbo.swap();
		advectShader.update(densityFbo.get(), densityFbo.getBackTexture(), velocityFbo.getTexture(), obstacleFbo.getTexture(), gridScale, timeStep, disDen);
		
		float disTmp = 1.0 - _deltaTime * dissipationTmp.get();
		temperatureFbo.swap();
		advectShader.update(temperatureFbo.get(), temperatureFbo.getBackTexture(), velocityFbo.getTexture(), obstacleFbo.getTexture(), gridScale, timeStep, disTmp);
		
		float disPrs = 1.0 - dissipationPrs.get();
		pressureFbo.swap();
		multiplyForceShader.update(pressureFbo.get(), pressureFbo.getBackTexture(), disPrs);
		
		// DIFFUSE
		if (viscosityVel.get() > 0.0) {
			float visDifStep = 0.25 * viscosityVel.get();
			for (int i = 0; i < numJacobiIterationsDiffuse; i++) {
				velocityFbo.swap();
				jacobiDiffusionShader.update(velocityFbo.get(), velocityFbo.getBackTexture(), obstacleFbo.getTexture(), obstacleOffsetFbo.getTexture(), gridScale, visDifStep);
			}
		}
		if (viscosityDen.get() > 0.0) {
			float visDenStep = _deltaTime * 20 * viscosityDen.get();
			for (int i = 0; i < numJacobiIterationsDiffuse; i++) {
				densityFbo.swap();
				jacobiDiffusionShader.update(densityFbo.get(), densityFbo.getBackTexture(), obstacleFbo.getTexture(), obstacleOffsetFbo.getTexture(), gridScale, visDenStep);
			}
		}
		if (viscosityTmp.get() > 0.0) {
			float visTmpStep = timeStep * viscosityTmp.get();
			for (int i = 0; i < numJacobiIterationsDiffuse; i++) {
				temperatureFbo.swap();
				jacobiDiffusionShader.update(temperatureFbo.get(), temperatureFbo.getBackTexture(), obstacleFbo.getTexture(), obstacleOffsetFbo.getTexture(), gridScale, visTmpStep);
			}
		}
		
		// VORTEX CONFINEMENT
		if (vorticity.get() > 0.0) {
			float vorticityStep = timeStep * vorticity.get();
			vorticityCurlShader.update(vorticityCurlFbo, velocityFbo.getTexture(), obstacleFbo.getTexture(), gridScale);
			vorticityForceShader.update(vorticityForceFbo, vorticityCurlFbo.getTexture(), gridScale, vorticityStep);
			addVelocity(vorticityForceFbo.getTexture());
		}
		
		// BUOYANCY
		if (buoyancySigma.get() > 0.0 && buoyancyWeight.get() > 0.0 ) {
			float bouyStep = timeStep * buoyancySigma.get();
			buoyancyShader.update(buoyancyFbo, velocityFbo.getTexture(), temperatureFbo.getTexture(), densityFbo.getTexture(), bouyStep, buoyancyWeight.get(), buoyancyAmbientTemperature );
			addVelocity(buoyancyFbo.getTexture());
		}
		
		// PRESSURE
		divergenceShader.update(divergenceFbo, velocityFbo.getTexture(), obstacleFbo.getTexture(), obstacleOffsetFbo.getTexture(), gridScale);
		for (int i = 0; i < numJacobiIterationsProjection; i++) {
			pressureFbo.swap();
			jacobiPressureShader.update(pressureFbo.get(), pressureFbo.getBackTexture(), divergenceFbo.getTexture(), obstacleFbo.getTexture(), obstacleOffsetFbo.getTexture(), gridScale);
		}
		velocityFbo.swap();
		gradientShader.update(velocityFbo.get(), velocityFbo.getBackTexture(), pressureFbo.getTexture(), obstacleFbo.getTexture(), obstacleOffsetFbo.getTexture(), gridScale);
		
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
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		ftUtil::one(obstacleFbo);
		obstacleFbo.begin();
		ofSetColor(0,0,0,255);
		int borderSize = 1;
		ofDrawRectangle(borderSize, borderSize, obstacleFbo.getWidth()-borderSize*2, obstacleFbo.getHeight()-borderSize*2);
		obstacleFbo.end();
		ofPopStyle();
	}
	
	//--------------------------------------------------------------
	void ftFluidFlow::setObstacle(ofTexture & _tex){
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		initObstacle();
		addBooleanShader.update(obstacleFbo.get(), obstacleFbo.getBackTexture(), _tex);
		obstacleOffsetShader.update(obstacleOffsetFbo, obstacleFbo.getTexture());
		ofPopStyle();
	}
	
	//--------------------------------------------------------------
	void ftFluidFlow::addObstacle(ofTexture & _tex){
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		obstacleFbo.swap();
		addBooleanShader.update(obstacleFbo.get(), obstacleFbo.getBackTexture(), _tex);
		obstacleOffsetShader.update(obstacleOffsetFbo, obstacleFbo.getTexture());
		ofPopStyle();
	}
	
	//--------------------------------------------------------------
	void ftFluidFlow::reset() {
		ftFlow::reset();
		ftUtil::zero(pressureFbo);
		ftUtil::zero(temperatureFbo);
		ftUtil::zero(divergenceFbo);
		ftUtil::zero(vorticityCurlFbo);
		ftUtil::zero(buoyancyFbo);
		initObstacle();
		
		advectShader			= ftAdvectShader();
		buoyancyShader 			= ftBuoyancyShader();
		divergenceShader		= ftDivergenceShader();
		gradientShader			= ftGradientShader();
		jacobiDiffusionShader	= ftJacobiDiffusionShader();
		obstacleOffsetShader	= ftObstacleOffsetShader();
		vorticityCurlShader		= ftVorticityCurlShader();
		vorticityForceShader	= ftVorticityForceShader();
	}
}

