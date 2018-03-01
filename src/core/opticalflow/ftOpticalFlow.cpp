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
		
		parameters.add(strength.set("strength", 3, 1, 13));
//		parameters.add(offset.set("offset", 1, 1, 10));
		offset.set("offset", 1, 1, 10);
//		parameters.add(lambda.set("lambda", 0.01, 0.1, 1));
		lambda.set("lambda", 0.01, 0.1, 1);
		parameters.add(threshold.set("threshold", 0.02, 0, 0.2));
		parameters.add(doInverseX.set("inverse x", false));
		parameters.add(doInverseY.set("inverse y", false));
		parameters.add(doCombinedFlow.set("combined flow", false));
		doCombinedFlow.addListener(this, &ftOpticalFlow::doCombinedFlowListener);
	};
	
	void	ftOpticalFlow::setup(int _width, int _height){
		width = _width;
		height = _height;
		
		sourceSwapBuffer.allocate(width, height);
		sourceSwapBufferHalf.allocate(width / 2, height / 2);
		sourceSwapBufferQuarter.allocate(width / 4, height / 4);
		sourceSwapBufferEighth.allocate(width / 8, height / 8);
		
		velocityBufferCombined.allocate(width, height, GL_RGB32F);
		velocityBuffer.allocate(width, height, GL_RGB32F);
		velocityBufferHalf.allocate(width / 2, height / 2, GL_RGB32F);
		velocityBufferQuarter.allocate(width /4, height / 4, GL_RGB32F);
		velocityBufferEighth.allocate(width / 8, height / 8, GL_RGB32F);
		
		velocityBuffer.black();
		velocityBufferHalf.black();
		velocityBufferQuarter.black();
		velocityBufferEighth.black();
		velocityBufferCombined.black();
			
		bSourceSet = false;
	};
	
	void ftOpticalFlow::update() {
		
		float inverseX = 1;
		if (doInverseX)inverseX = -1;
		float inverseY = 1;
		if (doInverseY) inverseY = -1;
				
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		
		
		
		opticalFlowShader.update(velocityBuffer,
								 sourceSwapBuffer.getTexture(),
								 sourceSwapBuffer.getBackTexture(),
								 strength.get(),
								 offset.get(),
								 lambda.get(),
								 threshold.get(),
								 inverseX,
								 inverseY);
		
		if (doCombinedFlow.get()) {
			velocityBufferCombined.black();
			
			addShader.update(velocityBufferCombined, velocityBufferCombined.getTexture(), velocityBuffer.getTexture(), 0.25);
			
			opticalFlowShader.update(velocityBufferHalf,
									 sourceSwapBufferHalf.getTexture(),
									 sourceSwapBufferHalf.getBackTexture(),
									 strength.get(),
									 offset.get(),
									 lambda.get(),
									 threshold.get(),
									 inverseX,
									 inverseY);
			addShader.update(velocityBufferCombined, velocityBufferCombined.getTexture(), velocityBufferHalf.getTexture(), 0.25);
			
			opticalFlowShader.update(velocityBufferQuarter,
									 sourceSwapBufferQuarter.getTexture(),
									 sourceSwapBufferQuarter.getBackTexture(),
									 strength.get(),
									 offset.get(),
									 lambda.get(),
									 threshold.get(),
									 inverseX,
									 inverseY);
			addShader.update(velocityBufferCombined, velocityBufferCombined.getTexture(), velocityBufferQuarter.getTexture(), 0.25);
			
			opticalFlowShader.update(velocityBufferEighth,
									 sourceSwapBufferEighth.getTexture(),
									 sourceSwapBufferEighth.getBackTexture(),
									 strength.get(),
									 offset.get(),
									 lambda.get(),
									 threshold.get(),
									 inverseX,
									 inverseY);
			addShader.update(velocityBufferCombined, velocityBufferCombined.getTexture(), velocityBufferEighth.getTexture(), 0.25);
		}

		ofPopStyle();
	}
	
	void ftOpticalFlow::setSource(ofTexture& _tex){
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		
		sourceSwapBuffer.swap();
		sourceSwapBuffer.getBuffer()->stretchIntoMe(_tex);
		
		if (doCombinedFlow.get()) {
			sourceSwapBufferHalf.swap();
			sourceSwapBufferHalf.getBuffer()->stretchIntoMe(_tex);
			
			sourceSwapBufferQuarter.swap();
			sourceSwapBufferQuarter.getBuffer()->stretchIntoMe(_tex);
			
			sourceSwapBufferEighth.swap();
			sourceSwapBufferEighth.getBuffer()->stretchIntoMe(_tex);
		}
		
		if (!bSourceSet) { // on start set both buffers
			bSourceSet = true;
			sourceSwapBuffer.swap();
			sourceSwapBuffer.getBuffer()->stretchIntoMe(_tex);
			
			sourceSwapBufferHalf.swap();
			sourceSwapBufferHalf.getBuffer()->stretchIntoMe(_tex);
			
			sourceSwapBufferQuarter.swap();
			sourceSwapBufferQuarter.getBuffer()->stretchIntoMe(_tex);
			
			sourceSwapBufferEighth.swap();
			sourceSwapBufferEighth.getBuffer()->stretchIntoMe(_tex);
		}
		
		ofPopStyle();
		
	}
	
	
}
