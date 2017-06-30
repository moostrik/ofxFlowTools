/*  ************************************************************************************
 *
 *  ftVelocityTrail
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
 *	The Optical Flow Shader is based on a Quartz Composer patch, but was also inspired by ofxMIOFlowGLSL by Princemio
 *
 *  ************************************************************************************ */

#include "ftVelocityTrail.h"

namespace flowTools {
	
	ftVelocityTrail::ftVelocityTrail(){
		parameters.setName("flow trail");
		parameters.add(strength.set("strength", 10, 0, 100));
		//		parameters.add(doTimeBlurDecay.set("do time blur", true));
		doTimeBlurDecay.set("do time blur", true);
		timeBlurParameters.setName("time decay blur");
		timeBlurParameters.add(timeBlurDecay.set("decay", 3, 0, 10));
		timeBlurParameters.add(timeBlurRadius.set("blur radius", 3, 0, 10));
		parameters.add(timeBlurParameters);
		
		lastTime = ofGetElapsedTimef();
	};
	
	void	ftVelocityTrail::setup(int _width, int _height){
		width = _width;
		height = _height;
		
		trailBuffer.allocate(width, height, GL_RGB32F);
		trailBuffer.black();
	};
	
	void ftVelocityTrail::update(float _deltaTime) {
		float time = ofGetElapsedTimef();
		if (_deltaTime != 0)
			deltaTime = _deltaTime;
		else
			deltaTime = min(ofGetElapsedTimef() - lastTime, 1.f / 30.f);
		lastTime = time;
		timeStep = deltaTime * strength.get();
		
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		
		timeBlurShader.update(trailBuffer, *velocityTexture, timeBlurDecay * deltaTime, timeBlurRadius);
		
		ofPopStyle();
	}
	
	
}
