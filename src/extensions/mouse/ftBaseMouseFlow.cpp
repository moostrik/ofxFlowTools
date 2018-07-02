/*  ************************************************************************************
 *
 *  ftBaseMouseFlow
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

#include "ftBaseMouseFlow.h"

namespace flowTools {
	
	ftBaseMouseFlow::ftBaseMouseFlow() {
		ofAddListener(ofEvents().mouseMoved, this, &ftBaseMouseFlow::mouseMoved);
		ofAddListener(ofEvents().mouseDragged, this, &ftBaseMouseFlow::mouseDragged);
	}
	
	//--------------------------------------------------------------
	void ftBaseMouseFlow::setup(int _width, int _height) {
		ftFlow::allocate(_width, _height, GL_RGBA32F);
		
		mousePositionsSet = false;
		force = glm::vec4(0,0,0,0);
		
		parameters.setName("mouse");
		parameters.add(doReset.set("reset", false));
		parameters.add(pIsTemporary.set("is temporary", true));
		pIsTemporary.addListener(this, &ftBaseMouseFlow::pIsTemporaryListener);
		parameters.add(strength.set("strength", 1, 0, 5));
		parameters.add(radius.set("radius", 0.035, 0, .1));
		parameters.add(edge.set("edge", 1.0, 0, 1));
		
		bDraw = false;
		bStrengthUpdated = false;
		bFlowChanged = false;
	}
	
	//--------------------------------------------------------------
	void ftBaseMouseFlow::update() {
		bFlowChanged = false;
		if (pIsTemporary) { reset(); }
		if (bDraw) {
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
	void ftBaseMouseFlow::mouseDragged( ofMouseEventArgs& _mouse ) {
		mouseMoved(_mouse);
		bDraw = true;
	}
	
	//--------------------------------------------------------------
	void ftBaseMouseFlow::mouseMoved( ofMouseEventArgs& _mouse ){
		glm::vec2 normalizedMouse = glm::vec2 (_mouse.x / (float)ofGetWindowWidth(), _mouse.y / (float)ofGetWindowHeight());
		if (!mousePositionsSet) {
			mousePositionsSet = true;
			mousePositions[mps] = normalizedMouse;
		}
		mps = !mps;
		mousePositions[mps] = normalizedMouse;
	}
	
	//--------------------------------------------------------------
	void ftBaseMouseFlow::drawForce(glm::vec2 _startPosition, glm::vec2 _endPosition) {
		glm::vec2 absoluteStartPosition = _startPosition * glm::vec2(width, height);
		glm::vec2 absoluteEndPosition = _endPosition * glm::vec2(width, height);
		float absoluteRadius = radius * width;
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		mouseShader.update(outputFbo,
						   force,
						   absoluteEndPosition,
						   absoluteRadius,
						   edge);
		ofPopStyle();
	}
	
	//--------------------------------------------------------------
	void ftBaseMouseFlow::reset() {
		ftFlow::reset();
		bDraw = false;
		bStrengthUpdated = false;
		bFlowChanged = false;
	}
	
}

