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
		strength.set("power", 1, .01, 1);
		doInverseX.set("inverse x", true); // flow velocity is inverse to fluid velocity
		doInverseY.set("inverse y", true); // flow velocity is inverse to fluid velocity
		
//		parameters.add(offset);
		parameters.add(threshold);
		parameters.add(strength);
//		parameters.add(doInverseX);
//		parameters.add(doInverseY);
	};
	
	void	ftOpticalFlow::setup(int _width, int _height){
		width = _width;
		height = _height;
		
		sourceSwapBuffer.allocate(width, height, GL_R8);
		ftUtil::black(sourceSwapBuffer);
		
		velocityBuffer.allocate(width, height, GL_RG32F);
		ftUtil::black(velocityBuffer);
		
		luminanceBuffer.allocate(width, height, GL_R8);
		ftUtil::black(luminanceBuffer);
			
		bSourceSet = false;
	};
	
	void ftOpticalFlow::update() {
		
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		
		opticalFlowShader.update(velocityBuffer,
								 sourceSwapBuffer.getTexture(),
								 sourceSwapBuffer.getBackTexture(),
								 offset.get(),
								 threshold.get(),
								 strength.get(),
								 doInverseX.get(),
								 doInverseY.get());
		
		ofPopStyle();
	}
	
	void ftOpticalFlow::setSource(ofTexture& _tex){
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		
		sourceSwapBuffer.swap();
		if (ofGetNumChannelsFromGLFormat(_tex.getTextureData().glInternalFormat) != 1) {
			if (luminanceBuffer.getWidth() != _tex.getWidth() || luminanceBuffer.getHeight() != _tex.getHeight()) {
				luminanceBuffer.allocate(_tex.getWidth(), _tex.getHeight(), GL_RGB8) ;
			}
			luminanceShader.update(luminanceBuffer, _tex);
			sourceSwapBuffer.getBuffer()->stretchIntoMe(luminanceBuffer.getTexture());
		}
		else {
			sourceSwapBuffer.getBuffer()->stretchIntoMe(_tex);
		}
		
		if (!bSourceSet) {
			bSourceSet = true;
			sourceSwapBuffer.swap();
			sourceSwapBuffer.getBuffer()->stretchIntoMe(sourceSwapBuffer.getBackTexture());
		}
		
		ofPopStyle();
	}
}
