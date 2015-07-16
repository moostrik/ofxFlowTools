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
		parameters.add(strength.set("strength", 50, 0, 100));
		parameters.add(offset.set("offset", 3, 1, 10));
		parameters.add(lambda.set("lambda", 0.01, 0, 0.1));
		parameters.add(threshold.set("threshold", 0.02, 0, 0.2));
		parameters.add(doInverseX.set("inverse x", false));
		parameters.add(doInverseY.set("inverse y", false));
		parameters.add(doTimeBlurDecay.set("do time blur", true));
		timeBlurParameters.setName("time blur");
		timeBlurParameters.add(timeBlurDecay.set("Decay", 0.1, 0, 1));
		timeBlurParameters.add(timeBlurRadius.set("Decay Blur Radius", 2, 0, 10));
		parameters.add(timeBlurParameters);
		
//		flowVectorsDidUpdate = false;
		lastTime = ofGetElapsedTimef();
	};
	
	void	ftOpticalFlow::setup(int _width, int _height){
		width = _width;
		height = _height;
				
		sourceSwapBuffer.allocate(width, height);
		velocityBuffer.allocate(width, height, GL_RGB32F);
		velocityBuffer.clear();
		
		velocityTexture.allocate(width, height, GL_RGB32F);
		
		decayBuffer.allocate(width, height, GL_RGB32F);
		decayBuffer.clear();
		
//		flowVectors = new ofVec2f[int(width * height)];
//		flowFloats = new float [int(width * height) * 2];
		
		bSourceSet = false;
	};
	
	void ftOpticalFlow::update(float _deltaTime) {
		float time = ofGetElapsedTimef();
		if (_deltaTime != 0)
			deltaTime = _deltaTime;
		else
			deltaTime = time - lastTime;
		lastTime = time;
		timeStep = deltaTime * strength.get();
		
//		flowVectorsDidUpdate = false;
		
		float inverseX = 1;
		if (doInverseX)inverseX = -1;
		float inverseY = 1;
		if (doInverseY) inverseY = -1;
				
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		
		opticalFlowShader.update(velocityBuffer,
								 sourceSwapBuffer.src->getTextureReference(),
								 sourceSwapBuffer.dst->getTextureReference(),
								 timeStep,
								 offset.get(),
								 lambda.get(),
								 threshold.get(),
								 inverseX,
								 inverseY);
		
		decayBuffer.drawIntoMe(velocityBuffer);
		timeBlurShader.update(decayBuffer, timeBlurDecay, timeBlurRadius);
				
		ofPopStyle();
	}
	
	void ftOpticalFlow::setSource(ofTexture& _tex){
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		
		sourceSwapBuffer.swap();
		sourceSwapBuffer.src->stretchIntoMe(_tex);
		
		if (!bSourceSet) { // on start set both buffers
			bSourceSet = true;
			sourceSwapBuffer.dst->stretchIntoMe(_tex);
		}
		
		ofPopStyle();
		
	}
	
/* MOVED TO FTAVERAGEVELOCITY
 
	ofVec2f* ftOpticalFlow::getFlowVectors(){
		if (!flowVectorsDidUpdate) {
			velocityBuffer.bind();
			glReadPixels(0, 0, width, height, GL_RG, GL_FLOAT, flowFloats);
			velocityBuffer.unbind();
			
			for (int i=0; i<width * height; i++) {
				flowVectors[i] = ofVec2f(flowFloats[i * 2], flowFloats[i * 2 + 1]);
			}
		}
		flowVectorsDidUpdate = true;
		
		return flowVectors;
	}
	
	float ftOpticalFlow::getAverageFlow(){
		getFlowVectors();
		float avgFlow = 0;
		for (int i=0; i<width * height; i++) {
			avgFlow += flowVectors[i].length();
		}
		avgFlow /= width * height;
		return avgFlow;
	}
 */
	
}
