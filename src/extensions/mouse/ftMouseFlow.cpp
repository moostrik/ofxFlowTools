/*  ************************************************************************************
 *
 *  ftMouseFlow
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

#include "ftMouseFlow.h"

namespace flowTools {
	
	ftMouseFlow::ftMouseFlow() {
		ofAddListener(ofEvents().mouseMoved, this, &ftMouseFlow::mouseMoved);
		ofAddListener(ofEvents().mouseDragged, this, &ftMouseFlow::mouseDragged);
	}
	
	ftMouseFlow::~ftMouseFlow() {
		ofRemoveListener(ofEvents().mouseMoved, this, &ftMouseFlow::mouseMoved);
		ofRemoveListener(ofEvents().mouseDragged, this, &ftMouseFlow::mouseDragged);
	}
	
	//--------------------------------------------------------------
	void ftMouseFlow::setup(int _width, int _height, ftFlowForceType _type) {
		if (_type < FT_DENSITY || _type > FT_OBSTACLE) {
			ofLogWarning("ftMouseFlow") << "Type " << ftFlowForceNames[_type] << " not supported";
			return;
		}
		type = _type;
		ftFlow::allocate(_width, _height, ftUtil::getInternalFormatFromType(type));
		
		mousePositionsSet = false;
		force = glm::vec4(0,0,0,0);
		
		parameters.add(pSpeed.set("speed", 50, 0, 100));
		parameters.add(pPersistent.set("persistent", false));
		pPersistent.addListener(this, &ftMouseFlow::pPersistentListener);
		parameters.add(pRadius.set("radius", 0.035, 0, .1));
		parameters.add(pSmooth.set("smooth", 1.0, 0, 1));
		parameters.add(pInverse.set("inverse", false));
		switch (type) {
			case FT_DENSITY:
				parameters.setName("density mouse");
				parameters.add(pColor.set("color", ofFloatColor(.5,.5,.5,.5), ofFloatColor(0,0,0,0), ofFloatColor(1,1,1,1)));
				break;
			case FT_VELOCITY:
				parameters.setName("velocity mouse");
				break;
			case FT_PRESSURE:
				parameters.setName("pressure mouse");
				break;
			case FT_TEMPERATURE:
				parameters.setName("temperature mouse");
				break;
			case FT_OBSTACLE:
				parameters.setName("obstacle mouse");
				pSmooth.set(0);
				parameters.remove("inverse");
				parameters.remove("smooth");
				parameters.remove("speed");
				break;
			default:
				break;
		}

		bMouseDragged = false;
		bFlowChanged = false;
	}
	
	//--------------------------------------------------------------
	void ftMouseFlow::update(float _deltaTime) {
		bFlowChanged = false;
		if (!pPersistent) { ftFlow::reset(); }
		if (bMouseDragged) {
			float inv = pInverse? -1: 1;
			glm::vec2 vel = (mousePositions[mps] - mousePositions[!mps]) * inv;
			float mag = glm::length(vel) * inv;
			
			switch (type) {
				case FT_DENSITY:
					force = glm::vec4(pColor->r, pColor->g, pColor->b, pColor->a) * mag;
					break;
				case FT_VELOCITY:
					force = glm::vec4(vel, 0, 0);
					break;
				case FT_PRESSURE:
				case FT_TEMPERATURE:
					force = glm::vec4(mag, 0, 0, 0);
					break;
				case FT_OBSTACLE:
					force = pInverse? glm::vec4(0, 0, 0, 0): glm::vec4(1, 0, 0, 0);
					break;
				default:
					break;
			}
			
			glm::vec2 startPosition = mousePositions[!mps] * glm::vec2(inputWidth, inputHeight);
			glm::vec2 endPosition = mousePositions[mps] * glm::vec2(inputWidth, inputHeight);
			float radius = pRadius.get() * inputWidth;
			
			ofPushStyle();
			ofEnableBlendMode(OF_BLENDMODE_DISABLED);
			inputFbo.swap();
			mouseShader.update(inputFbo,
							   force,
							   endPosition,
							   radius,
							   pSmooth.get());
			
			if (pPersistent) {
				addInput(inputFbo.getBackTexture());
			} else {
				resetOutput();
				addOutput(inputFbo.getTexture(), pSpeed.get() * _deltaTime * ofGetFrameRate());
			}
			
			ofPopStyle();
			
			bMouseDragged = false;
			bFlowChanged = true;
		}
		if (pPersistent) {
			resetOutput();
			addOutput(inputFbo.getTexture(), pSpeed.get() * _deltaTime);
			bFlowChanged = true;
		}
	}
	
	//--------------------------------------------------------------
	void ftMouseFlow::mouseDragged( ofMouseEventArgs& _mouse ) {
		mouseMoved(_mouse);
		bMouseDragged = true;
	}
	
	//--------------------------------------------------------------
	void ftMouseFlow::mouseMoved( ofMouseEventArgs& _mouse ){
		glm::vec2 normalizedMouse = glm::vec2 (_mouse.x / (float)ofGetWindowWidth(), _mouse.y / (float)ofGetWindowHeight());
		if (!mousePositionsSet) {
			mousePositionsSet = true;
			mousePositions[mps] = normalizedMouse;
		}
		mps = !mps;
		mousePositions[mps] = normalizedMouse;
	}
	
	//--------------------------------------------------------------
	void ftMouseFlow::reset() {
		ftFlow::reset();
		bMouseDragged = false;
		bFlowChanged = false;
	}
	
}

