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
		parameters.add(speed.set("speed", 100, 0, 200));
		parameters.add(cellSize.set("cell size", 1.25, 0.0, 2.0));
		parameters.add(numJacobiIterations.set("iterations", 40, 1, 100));
		parameters.add(viscosity.set("viscosity", 0.1, 0, .4));
		parameters.add(vorticity.set("vorticity", 0.6, 0.0, 1));
		parameters.add(dissipation.set("dissipation", 0.002, 0, 0.01));
		smokeBuoyancyParameters.setName("smoke buoyancy");
		smokeBuoyancyParameters.add(smokeSigma.set("sigma", 0.05, 0.0, 1.0));
		smokeBuoyancyParameters.add(smokeWeight.set("weight", 0.05, 0.0, 1.0));
		smokeBuoyancyParameters.add(ambientTemperature.set("ambient temperature", 0.0, 0.0, 1.0));
		smokeBuoyancyParameters.add(gravity.set("gravity", ofDefaultVec2(0., 9.80665), ofDefaultVec2(-10, -10), ofDefaultVec2(10, 10)));
//		parameters.add(smokeBuoyancyParameters);
	}
	
	//--------------------------------------------------------------
	void ftFluidFlow::setup(int _flowWidth, int _flowHeight, int _densityWidth, int _densityHeight) {		
		simulationWidth = _flowWidth;
		simulationHeight = _flowHeight;
		densityWidth = _densityWidth;
		densityHeight = _densityHeight;
		
		ftFlow::allocate(simulationWidth, simulationHeight, GL_RG32F, densityWidth, densityHeight, GL_RGBA32F);
		visualizationField.setup(simulationWidth, simulationHeight);
		
		temperatureFbo.allocate(simulationWidth,simulationHeight,GL_R32F);
		ftUtil::zero(temperatureFbo);
		pressureFbo.allocate(simulationWidth,simulationHeight,GL_R32F);
		ftUtil::zero(pressureFbo);
		
		obstacleFbo.allocate(simulationWidth, simulationHeight, GL_R8);
		ftUtil::zero(obstacleFbo);
		createEdgeImage(obstacleFbo);
		
		divergenceFbo.allocate(simulationWidth, simulationHeight, GL_R32F);
		ftUtil::zero(divergenceFbo);
		smokeBuoyancyFbo.allocate(simulationWidth, simulationHeight, GL_RG32F);
		ftUtil::zero(smokeBuoyancyFbo);
		vorticityVelocityFbo.allocate(simulationWidth, simulationHeight, GL_RG32F);
		ftUtil::zero(pressureFbo);
		vorticityConfinementFbo.allocate(simulationWidth, simulationHeight, GL_RG32F);
		ftUtil::zero(pressureFbo);
		
	}
	
	//--------------------------------------------------------------
	void ftFluidFlow::update(float _deltaTime){
		float timeStep = _deltaTime * speed.get();
		
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);

		ftPingPongFbo& velocityFbo = inputFbo;
		ftPingPongFbo& densityFbo = outputFbo;
		
		
		// VORTEX CONFINEMENT
		if (vorticity.get() > 0.0) {
			vorticityFirstPassShader.update(vorticityVelocityFbo,
											velocityFbo.getTexture(),
											obstacleFbo.getTexture());
			
			vorticitySecondPassShader.update(vorticityConfinementFbo,
											 vorticityVelocityFbo.getTexture(),
											 timeStep,
											 vorticity.get(),
											 cellSize.get());
			
			addVelocity(vorticityConfinementFbo.getTexture());
		}
		
		// ADVECT
		velocityFbo.swap();
		advectShader.update(velocityFbo.get(),
							velocityFbo.getBackTexture(),
							velocityFbo.getBackTexture(),
							obstacleFbo.getTexture(),
							timeStep,
							1.0 - (dissipation.get()),
							cellSize.get());
		
		densityFbo.swap();
		advectShader.update(densityFbo.get(),
							densityFbo.getBackTexture(),
							velocityFbo.getTexture(),
							obstacleFbo.getTexture(),
							timeStep,
							1.0 - (dissipation.get()),
							cellSize.get());
		
		// DIFFUSE
		if (viscosity.get() > 0.0) {
			for (int i = 0; i < numJacobiIterations.get(); i++) {
				velocityFbo.swap();
				diffuseShader.update(velocityFbo.get(),
									 velocityFbo.getBackTexture(),
									 obstacleFbo.getTexture(),
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
//								obstacleFbo.getTexture(),
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
								obstacleFbo.getTexture(),
								cellSize.get());
		
		ftUtil::zero(pressureFbo);
		for (int i = 0; i < numJacobiIterations.get(); i++) {
			pressureFbo.swap();
			jacobiShader.update(pressureFbo.get(),
								pressureFbo.getBackTexture(),
								divergenceFbo.getTexture(),
								obstacleFbo.getTexture(),
								cellSize.get());
		}
		
		velocityFbo.swap();
		substractGradientShader.update(velocityFbo.get(),
									   velocityFbo.getBackTexture(),
									   pressureFbo.getTexture(),
									   obstacleFbo.getTexture(),
									   cellSize.get());
		
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
	void ftFluidFlow::addObstacle(ofTexture & _tex){
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		ftUtil::stretch(obstacleFbo, _tex);
		ofPopStyle();
	}
	
	//--------------------------------------------------------------
	void ftFluidFlow::reset() {
		ftFlow::reset();
		ftUtil::zero(pressureFbo);
		ftUtil::zero(temperatureFbo);
		createEdgeImage(obstacleFbo);
	}
	
	//--------------------------------------------------------------
	void ftFluidFlow::createEdgeImage(ofFbo &_Fbo, int _edgeWidth, ofColor _backgroundColor, ofColor _edgeColor) {
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
