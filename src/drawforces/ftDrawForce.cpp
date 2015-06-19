/*  ************************************************************************************
 *
 *  ftDrawForce
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

#include "ftDrawForce.h"

namespace flowTools {
	
	ftDrawForce::ftDrawForce() {
		parameters.setName("draw force");
		parameters.add(drawType.set("non/den/vel/tem/prs/ob", 0, 0, 5));
		parameters.add(isTemporary.set("is temporary", true));
		isTemporary.addListener(this, &ftDrawForce::resetonTempSwitch);
		parameters.add(force.set("force", ofVec4f(1,1,1,1), ofVec4f(-1,-1,-1,-1), ofVec4f(1,1,1,1)));
		parameters.add(strength.set("strength", 1, 0, 5));
		parameters.add(radius.set("radius", 0.035, 0, .1));
		parameters.add(edge.set("edge", 1.0, 0, 1));
		parameters.add(doReset.set("reset", false));
	}
		
	void ftDrawForce::setup(int _width, int _height, ftDrawForceType _type, bool _isTemporary) {
		width = _width;
		height = _height;
		type = _type;
		drawType.set(type);
		isTemporary.set(_isTemporary);
			
		forceBuffer.allocate(width, height, GL_RGBA32F);
		forceBuffer.clear();
			
		density = ofFloatColor(1,1,1,1);
		velocity = ofVec2f(0,0);
		temperature = 1;
		pressure = -1;
		obstacle = true;
		
		forceChanged = false;
		forceApplied = false;
	}
		
	void ftDrawForce::applyForce(ofVec2f _normalizedPosition) {
		absolutePosition = _normalizedPosition * ofVec2f(width, height);
		
		absoluteRadius = radius * width;
		
		if (isTemporary && !forceApplied) // && allow for multiple temporal forces
			forceBuffer.clear();
		
//		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
		
		ofVec4f typeForce = force;
		switch (type) {
			case FT_VELOCITY:
				typeForce *= ofVec4f(width, width, 0, 1);
				break;
			case FT_PRESSURE:
				typeForce *= ofVec4f(100, 0, 0, 1);
				break;
			case FT_TEMPERATURE:
				typeForce = ofVec4f(force.get().x, force.get().x, force.get().x, 1);
				break;
			case FT_OBSTACLE:
				typeForce = ofVec4f(force.get().x, force.get().x, force.get().x, 1);
				break;
			default:
				break;
		}
		
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		drawForceShader.update(forceBuffer,
								typeForce,
								absolutePosition,
								absoluteRadius,
								edge);
	
		ofPopStyle();
		forceApplied = true;
	}
	
	void ftDrawForce::update() {
		switch (drawType.get()) {
			case 1:
				if (type != FT_DENSITY) {
					saveValue(type, force);
					setForce(density);
					reset();
				}
				setType(FT_DENSITY);
				break;
			case 2:
				if (type != FT_VELOCITY) {
					saveValue(type, force);
					setForce(velocity);
					reset();
				}
				force.set(ofVec4f(force.get().x, force.get().y, 0, 1));
				setType(FT_VELOCITY);
				break;
			case 3:
				if (type != FT_TEMPERATURE) {
					saveValue(type, force);
					setForce(temperature);
					reset();
				}
				force.set(ofVec4f(force.get().x, 0, 0, 1));
				setType(FT_TEMPERATURE);
				break;
			case 4:
				if (type != FT_PRESSURE) {
					saveValue(type, force);
					setForce(pressure);
					reset();
				}
				force.set(ofVec4f(force.get().x, 0, 0, 1));
				setType(FT_PRESSURE);
				break;
			case 5:
				if (type != FT_OBSTACLE) {
					saveValue(type, force);
					setForce(obstacle);
					reset();
				}
				force.set(ofVec4f((int)abs(force.get().x + 0.5), 0, 0, 1));
				setType(FT_OBSTACLE);
				break;
			default:
				setType(FT_NONE);
				break;
		}
		
		if(doReset) {
			reset();
			doReset.set(false);
		}
		if (forceApplied) forceChanged = true;
		else if (isTemporary) forceChanged = false;
		forceApplied = false;
	}
	
	void ftDrawForce::reset() {
		forceBuffer.clear();
		forceChanged = false;
		forceApplied = true;
	}
	
	void ftDrawForce::saveValue(ftDrawForceType _type, ofVec4f _force) {
		switch (_type) {
			case FT_DENSITY:
				density.set(ofFloatColor(_force.x, _force.y, _force.z, _force.w));
				break;
			case FT_VELOCITY:
				velocity.set(ofVec4f(_force.x, _force.y, 0, 1));
				break;
			case FT_TEMPERATURE:
				temperature.set(_force.x);
				break;
			case FT_PRESSURE:
				pressure.set(_force.x);
				break;
			case FT_OBSTACLE:
				obstacle.set(_force.x);
				break;
			default:
				break;
		}
	}
}

