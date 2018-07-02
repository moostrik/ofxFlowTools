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
	
	//--------------------------------------------------------------
	void ftMouseFlow::setup(int _width, int _height, ftFlowForceType _type) {
		type = _type;
		ftFlow::allocate(_width, _height, ftUtil::getInternalFormatFromType(type));
//		ftFlow::allocate(_width, _height, GL_RGBA32F);
		
		mousePositionsSet = false;
		force = glm::vec4(0,0,0,0);
		
		parameters.add(pIsTemporary.set("is temporary", true));
		pIsTemporary.addListener(this, &ftMouseFlow::pIsTemporaryListener);
		parameters.add(pStrength.set("speed", 50, 0, 100));
		parameters.add(pRadius.set("radius", 0.035, 0, .1));
		parameters.add(pEdge.set("edge", 1.0, 0, 1));
		parameters.add(pInverse.set("inverse", false));
		switch (type) {
			case FT_INPUT:
				parameters.setName("input mouse");
				parameters.add(pColor.set("color", ofFloatColor(.5,.5,.5,.5), ofFloatColor(0,0,0,0), ofFloatColor(1,1,1,1)));
				break;
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
				break;
			default:
				break;
		}

		bDraw = false;
		bStrengthUpdated = false;
		bFlowChanged = false;
	}
	
	//--------------------------------------------------------------
	void ftMouseFlow::update(float _deltaTime) {
		bFlowChanged = false;
		if (pIsTemporary) { ftFlow::reset(); }
		if (bDraw) {
			switch (type) {
				case FT_INPUT:
					break;
				case FT_DENSITY:
					force = glm::vec4(pColor->r, pColor->g, pColor->b, pColor->a) * pStrength.get() * _deltaTime;
					break;
				case FT_VELOCITY:
					force = glm::vec4((mousePositions[mps] - mousePositions[!mps]) * pStrength.get() * _deltaTime * ofGetFrameRate(), 0, 0);
					break;
				default:
					break;
			}
			
			drawForce(mousePositions[mps], mousePositions[!mps]);
			bDraw = false;
			bFlowChanged = true;
		}
		if (bStrengthUpdated && !pIsTemporary) {
			bStrengthUpdated = false;
			bFlowChanged = true;
		}
	}
	
	//--------------------------------------------------------------
	void ftMouseFlow::mouseDragged( ofMouseEventArgs& _mouse ) {
		mouseMoved(_mouse);
		bDraw = true;
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
	void ftMouseFlow::drawForce(glm::vec2 _startPosition, glm::vec2 _endPosition) {
		glm::vec2 absoluteStartPosition = _startPosition * glm::vec2(width, height);
		glm::vec2 absoluteEndPosition = _endPosition * glm::vec2(width, height);
		float absoluteRadius = pRadius.get() * width;
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		mouseShader.update(outputFbo,
						   force,
						   absoluteEndPosition,
						   absoluteRadius,
						   pEdge.get());
		ofPopStyle();
	}
	
	//--------------------------------------------------------------
	void ftMouseFlow::reset() {
		ftFlow::reset();
		bDraw = false;
		bStrengthUpdated = false;
		bFlowChanged = false;
	}
	
}

