/*
 *  ofxFlowTools.h
 *
 *  Created by Matthias Oostrik on 03/16/14.
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

#pragma once

#include "ftFlow.h"
#include "ftSplitVelocityShader.h"
#include "ftSvVisualization.h"
#include "ftSvVisualizationField.h"

namespace flowTools {
	
	class ftSplitVelocityFlow : public ftFlow {
	public:
		ftSplitVelocityFlow() { parameters.setName("split velocity flow"); }
		
		void setup(int _width, int _height){
			ftFlow::allocate(_width, _height, GL_RG32F, _width, _height, GL_RGBA32F);
			visualizationField.setup(_width, _height);
		};
		
		void update() {
			ofPushStyle();
			ofEnableBlendMode(OF_BLENDMODE_DISABLED);
			ftFlow::resetOutput();
			splitVelocityShader.update(outputFbo, inputFbo.getTexture(), 1.0);
			ofPopStyle();
		}
		
		void setVelocity(ofTexture& _tex) { setInput(_tex); }
		void addVelocity(ofTexture& _tex, float _strength = 1.0) { addInput(_tex, _strength); }
		ofTexture& getVelocity() { return getOutput(); }
		
		void drawOutput(int _x, int _y, int _w, int _h) override {
			visualizationField.draw(outputFbo.getTexture(), _x, _y, _w, _h);
		}
		
		void setVisualizationScale(float _value) override			{ visualizationField.setScale(_value); }
		void setVisualizationFieldSize(glm::vec2 _value) override	{ visualizationField.setupField(_value.x, _value.y); }
		bool setVisualizationToggleScalar(bool _value) override		{ visualizationField.setToggleScalar(_value); }
		
	protected:
		ftSplitVelocityShader	splitVelocityShader;
		ftSvVisualizationField		visualizationField;
		
	};
}


