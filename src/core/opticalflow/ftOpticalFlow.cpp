/*  ************************************************************************************
 *
 *  ftOpticalFlow
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

#include "ftOpticalFlow.h"

namespace flowTools {
	
	ftOpticalFlow::ftOpticalFlow(){
		parameters.setName("optical flow");
		offset.set("offset", 3, 1, 10);
		threshold.set("threshold", 0.02, 0, 0.2);
		strength.set("force", 3, .1, 10);			// 3 is best for normalization
		power.set("power", 1.0, .01, 1);			// 
		doInverseX.set("inverse x", true); 			// flow velocity is inverse to fluid velocity
		doInverseY.set("inverse y", true); 			// flow velocity is inverse to fluid velocity
		
//		parameters.add(offset);
		parameters.add(threshold);
//		parameters.add(strength);
//		parameters.add(power);
//		parameters.add(doInverseX);
//		parameters.add(doInverseY);
	};
	
	//--------------------------------------------------------------
	void ftOpticalFlow::setup(int _width, int _height){
		width = _width;
		height = _height;
		
		inputSwapFbo.allocate(width, height, GL_R8);
		velocityFbo.allocate(width, height, GL_RG32F);
		
		ftUtil::zero(inputSwapFbo);
		ftUtil::zero(velocityFbo);
			
		bFirstFrame = true;
		bSourceSet = false;
	};
	
	//--------------------------------------------------------------
	void ftOpticalFlow::update() {
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		
		if (bSourceSet) {
			bSourceSet = false;
			opticalFlowShader.update(velocityFbo,
									 inputSwapFbo.getTexture(),
									 inputSwapFbo.getBackTexture(),
									 offset.get(),
									 threshold.get(),
									 ofDefaultVec2(strength.get()),
									 power.get(),
									 doInverseX.get(),
									 doInverseY.get());
		}
		
		ofPopStyle();
	}
	
	//--------------------------------------------------------------
	void ftOpticalFlow::setInput(ofTexture& _tex){
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		
		inputSwapFbo.swap();
		
		if (ofGetNumChannelsFromGLFormat(_tex.getTextureData().glInternalFormat) != 1) {
			RGB2LumShader.update(inputSwapFbo, _tex);
		}
		else {
			ftUtil::stretch(inputSwapFbo, _tex);
		}
		
		if (bFirstFrame) {
			bFirstFrame = false;
			inputSwapFbo.swap();
			ftUtil::stretch(inputSwapFbo, inputSwapFbo.getBackTexture());
		}
		
		ofPopStyle();
		
		bSourceSet = true;
	}
	
	//--------------------------------------------------------------
	void ftOpticalFlow::reset() {
		ftUtil::zero(inputSwapFbo);
		ftUtil::zero(velocityFbo);
		
		bFirstFrame = true;
	}
}
