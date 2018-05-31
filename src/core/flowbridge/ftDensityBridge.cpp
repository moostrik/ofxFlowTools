/*  ************************************************************************************
 *
 *  ftDensityBridge
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
 *  ************************************************************************************ */

#include "ftDensityBridge.h"

namespace flowTools {
	
	void ftDensityBridge::setup(int _flowWidth, int _flowHeight, int _densityWidth, int _densityHeight){
		velocitySwapBuffer.allocate(_flowWidth, _flowHeight, GL_RG32F);
		ftUtil::zero(velocitySwapBuffer);
		
		densitySwapBuffer.allocate(_densityWidth, _densityHeight, GL_RGBA32F);
		ftUtil::zero(densitySwapBuffer);
		
		luminanceMaskFbo.allocate(_densityWidth, _densityHeight, GL_R32F);
		ftUtil::zero(luminanceMaskFbo);
		
		drawFbo.allocate(_densityWidth, _densityHeight, GL_RGBA);
		ftUtil::zero(drawFbo);
		
		bVelocityTextureSet = false;
		bDensityTextureSet = false;
		
		parameters.setName("flow density");
		parameters.add(trailWeight.set("trail", .5, 0, .99));
		parameters.add(blurRadius.set("blur", 5, 0, 10)); // blur works funky above 5
		parameters.add(saturation.set("saturation", 1, 0, 3));
//		parameters.add(hue.set("hue", 0, -1, 1)); // does not work properly (does in the minus range?)
		parameters.add(speed.set("speed", 8, 0, 20));
	};
	
	void ftDensityBridge::update(float _deltaTime) {
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		ftUtil::zero(densitySwapBuffer);
		
		if (!bVelocityTextureSet || !bDensityTextureSet) {
			ofLogVerbose("ftDensityBridge: velocity or density texture not set, can't update");
		}
		else {
			velocitySwapBuffer.swap();
			velocityBridgeShader.update(*velocitySwapBuffer.getBuffer(), *velocityTexture, velocitySwapBuffer.getBackTexture(), trailWeight.get());
			if (blurRadius.get() > 0) { blurShader.update(*velocitySwapBuffer.getBuffer(), 1, blurRadius.get()); }
			
			densitySwapBuffer.swap();
			densityBridgeShader.update(*densitySwapBuffer.getBuffer(),
									   *densityTexture,
									   velocitySwapBuffer.getTexture(),
									   speed.get() * _deltaTime);
			densitySwapBuffer.swap();
			HSLShader.update(*densitySwapBuffer.getBuffer(),
							 densitySwapBuffer.getBackTexture(),
							 0, // hue.get();
							 saturation.get(),
							 1);
			
			luminanceShader.update(luminanceMaskFbo, densitySwapBuffer.getTexture());
		}
		
		ofPopStyle();
	}
	
	void ftDensityBridge::draw(int _x, int _y, int _width, int _height, ofBlendMode _blendmode) {
		ofPushStyle();
		ofEnableBlendMode(_blendmode);
		multiplyShader.update(drawFbo, densitySwapBuffer.getTexture(), ofGetFrameRate());
		drawFbo.getTexture().draw(_x, _y, _width, _height);
		ofPopStyle();
	}
	
	void ftDensityBridge::setDensity(ofTexture &tex) {
		densityTexture = &tex;
		bDensityTextureSet = true;
	}
	
	void ftDensityBridge::setVelocity(ofTexture &tex) {
		velocityTexture = &tex;
		bVelocityTextureSet = true;
	}
}
