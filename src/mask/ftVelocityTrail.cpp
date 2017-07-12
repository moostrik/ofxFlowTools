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
		parameters.add(trailWeight.set("trail weight", .5, 0, .99));
		parameters.add(blurPasses.set("blur passes", 3, 0, 10));
		parameters.add(blurRadius.set("blur radius", 5, 0, 10));
		
		lastTime = ofGetElapsedTimef();
	};
	
	void	ftVelocityTrail::setup(int _width, int _height){
		width = _width;
		height = _height;
		
		trailSwapBuffer.allocate(width, height, GL_RGB32F);
		trailSwapBuffer.black();
	};
	
	void ftVelocityTrail::update() {
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		
		if (trailWeight.get() > 0.0) {
			trailSwapBuffer.swap();
			trailShader.update(*trailSwapBuffer.getBuffer(), *velocityTexture, trailSwapBuffer.getBackTexture(), trailWeight.get());
		}
		else {
			trailSwapBuffer.getBuffer()->stretchIntoMe(*velocityTexture);
		}
			
		if (blurPasses.get() > 0 && blurRadius.get() > 0) {
			blurShader.update(*trailSwapBuffer.getBuffer(), blurPasses.get(), blurRadius.get());
		}
		
		ofPopStyle();
	}
	
	
}
