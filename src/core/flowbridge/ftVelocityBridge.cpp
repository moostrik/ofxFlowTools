/*  ************************************************************************************
 *
 *  ftVelocityBridge
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

#include "ftVelocityBridge.h"

namespace flowTools {
	
	ftVelocityBridge::ftVelocityBridge(){
		parameters.setName("velocity input");
		parameters.add(trailWeight.set("trail weight", .5, 0, .99));
		parameters.add(blurRadius.set("blur radius", 5, 0, 10));
		parameters.add(strength.set("speed", 1, .1, 100));
	};
	
	void	ftVelocityBridge::setup(int _width, int _height){
		width = _width;
		height = _height;
		
		swapBuffer.allocate(width, height, GL_RG32F);
		swapBuffer.black();
		multiplyFbo.allocate(width, height, GL_RG32F);
		
		bSourceSet = false;
	};
	
	void ftVelocityBridge::setSource(ofTexture& _tex) {
		velocityTexture = &_tex;
		bSourceSet = true;
	}
	
	void ftVelocityBridge::update(float _deltaTime) {
		if (bSourceSet) {
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		
		swapBuffer.swap();
		trailShader.update(*swapBuffer.getBuffer(), *velocityTexture, swapBuffer.getBackTexture(), trailWeight.get());
		
		if (blurRadius.get() > 0) { blurShader.update(*swapBuffer.getBuffer(), 1, blurRadius.get()); }
		
		multiplyShader.update(multiplyFbo, swapBuffer.getTexture(), strength.get() * _deltaTime );
		
		ofPopStyle();
		}
	}
	
	
}
