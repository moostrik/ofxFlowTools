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
		
		parameters.setName("draw force");
		parameters.add(pType.set("type", FT_NONE, FT_NONE, FT_OBSTACLE));
		pType.addListener(this, &ftMouseFlow::pTypeListener);
		parameters.add(pTypeName.set("type name","none"));
		parameters.add(pIsTemporary.set("is temporary", true));
		pIsTemporary.addListener(this, &ftMouseFlow::pIsTemporaryListener);
		parameters.add(pForce.set("force", glm::vec4(1,1,1,1), glm::vec4(-1,-1,-1,-1), glm::vec4(1,1,1,1)));
		pForce.addListener(this, &ftMouseFlow::pForceListener);
		parameters.add(strength.set("strength", 1, 0, 5));
		parameters.add(radius.set("radius", 0.035, 0, .1));
		parameters.add(edge.set("edge", 1.0, 0, 1));
		parameters.add(doReset.set("reset", false));
	}
	
	//--------------------------------------------------------------
	void ftMouseFlow::setup(int _width, int _height, ftFlowForceType _type, bool _isTemporary) {
		ftFlow::allocate(_width, _height, GL_RGBA32F);
		pType.set(_type);
		pIsTemporary.set(_isTemporary);
			
		density		= glm::vec4(1,1,1,1);
		input 		= glm::vec4(1,1,1,1);
		velocity 	= glm::vec4(0,0,0,0);
		temperature	= glm::vec4(1,0,0,0);
		pressure 	= glm::vec4(1,0,0,0);
		obstacle 	= glm::vec4(1,0,0,0);
		
		bHasChanged = false;
		bForceSet = false;
		bForceApplied = false;
	}
	
	//--------------------------------------------------------------
	void ftMouseFlow::update() {
		if (bForceApplied || (bForceSet && !pIsTemporary)) {
			bHasChanged = true; }
		else {
			bHasChanged = false;
			if (pIsTemporary) { reset(); }
		}
		bForceApplied = false;
		bForceSet = false;
	}
	
	//--------------------------------------------------------------
	void ftMouseFlow::mouseDragged( ofMouseEventArgs& mouse ) {
		glm::vec2 normalizedMouse;
		
		normalizedMouse = glm::vec2 (mouse.x / (float)ofGetWindowWidth(), mouse.y / (float)ofGetWindowHeight());
		if (!lastNormalizedMouseSet) {
			lastNormalizedMouseSet = true;
			lastNormalizedMouse = normalizedMouse;
			return;
		}
		
		if (type == FT_VELOCITY) {
			pForce = glm::vec4(glm::vec2(normalizedMouse - lastNormalizedMouse), 0.0, 0.0);
		}
		applyForce(normalizedMouse);
		
		lastNormalizedMouse = normalizedMouse;
		
	}
	
	//--------------------------------------------------------------
	void ftMouseFlow::mouseMoved( ofMouseEventArgs& mouse ){
		glm::vec2 normalizedMouse;
		normalizedMouse = glm::vec2(mouse.x / (float)ofGetWindowWidth(), mouse.y / (float)ofGetWindowHeight());
		lastNormalizedMouse = normalizedMouse;
	}
	
	//--------------------------------------------------------------
	void ftMouseFlow::applyForce(glm::vec2 _normalizedPosition) {
		
		glm::vec2 absolutePosition = _normalizedPosition * glm::vec2(width, height);
		float absoluteRadius = radius * width;
		
		glm::vec4 typeForce = glm::vec4(0,0,0,0);
		switch (type) {
			case FT_DENSITY:
				typeForce = pForce.get();
				break;
			case FT_INPUT:
				typeForce = pForce.get();
				break;
			case FT_VELOCITY:
				typeForce = pForce.get() * glm::vec4(width, height, 0, 0);
				break;
			case FT_PRESSURE:
				typeForce = pForce.get() * glm::vec4(100, 0, 0, 0);
				break;
			case FT_TEMPERATURE:
				typeForce = pForce.get();
				break;
			case FT_OBSTACLE:
				typeForce = pForce.get();
				break;
			default:
				break;
		}
		
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		mouseShader.update(outputFbo,
						   typeForce,
						   absolutePosition,
						   absoluteRadius,
						   edge);
	
		ofPopStyle();
		bForceApplied = true;
	}
	
	void ftMouseFlow::pForceListener(glm::vec4 &_value) {
		switch (type) {
			case FT_INPUT:
				_value = glm::vec4(_value.x, _value.y, _value.z, 1.0);
				break;
			case FT_VELOCITY:
				_value *= glm::vec4(1,1,0,0);
				break;
			case FT_PRESSURE:
				_value *= glm::vec4(1,0,0,0);
				break;
			case FT_TEMPERATURE:
				_value *= glm::vec4(1,0,0,0);
				break;
			case FT_OBSTACLE:
				_value = glm::vec4(1,0,0,0);
				break;
			default:
				break;
		}
		bForceSet = true;
	}
	
	//--------------------------------------------------------------
	void ftMouseFlow::pTypeListener(int &_value) {
		ftFlowForceType newType = static_cast<ftFlowForceType>(_value);
		
		pTypeName.set(ftFlowForceNames[_value]);
		if (type != newType) {
			saveValue(type, pForce);
			reset();
			type = newType;
			pForce = loadValue(type);
		}
	}
	
	//--------------------------------------------------------------
	void ftMouseFlow::reset() {
		ftFlow::reset();
		bHasChanged = false;
	}
	
	//--------------------------------------------------------------
	void ftMouseFlow::saveValue(ftFlowForceType _type, glm::vec4 _force) {
		switch (_type) {
			case FT_DENSITY:		density = _force;		break;
			case FT_INPUT:			input = _force; 		break;
			case FT_VELOCITY:		velocity = _force; 		break;
			case FT_TEMPERATURE:	temperature = _force;	break;
			case FT_PRESSURE:		pressure = _force; 		break;
			case FT_OBSTACLE:
			case FT_NONE:
			default:										break;
		}
	}
	
	//--------------------------------------------------------------
	glm::vec4 ftMouseFlow::loadValue(ftFlowForceType _type) {
		switch (_type) {
			case FT_DENSITY:		return density;				break;
			case FT_INPUT:			return input;				break;
			case FT_VELOCITY:		return velocity;			break;
			case FT_TEMPERATURE:	return temperature;			break;
			case FT_PRESSURE:		return pressure;			break;
			case FT_OBSTACLE:		return glm::vec4(1,0,0,0);	break;
			case FT_NONE:
			default:				return glm::vec4(0);		break;
		}
	}
	
}

