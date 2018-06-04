//
//  ftDrawForces.cpp
//  ofxFlowToolsExample
//
//  Created by Ties East on 18/6/15.
//
//

#include "ftDrawMouseForces.h"


namespace flowTools {
	
	ftDrawMouseForces::ftDrawMouseForces() {
		ofAddListener(ofEvents().mouseMoved, this, &ftDrawMouseForces::mouseMoved);
		ofAddListener(ofEvents().mouseDragged, this, &ftDrawMouseForces::mouseDragged);
	}
    
    ftDrawMouseForces::~ftDrawMouseForces() {
        ofRemoveListener(ofEvents().mouseMoved, this, &ftDrawMouseForces::mouseMoved);
        ofRemoveListener(ofEvents().mouseDragged, this, &ftDrawMouseForces::mouseDragged);
    }
	
	void ftDrawMouseForces::setup(int _simulationWidth, int _simulationHeight, int _densityWidth, int _densityHeight) {
		simulationWidth = _simulationWidth;
		simulationHeight = _simulationHeight;
		densityWidth = (!_densityWidth)? simulationWidth : _densityWidth;
		densityHeight = (!_densityHeight)? simulationHeight: _densityHeight;
		
		numDrawForces = 6;
		drawForces = new ftDrawForce[numDrawForces];
		drawForces[0].setup(densityWidth, densityHeight, FT_FLUID_DENSITY, true);
		drawForces[0].setName("draw full res");
		drawForces[1].setup(simulationWidth, simulationHeight, FT_FLUID_VELOCITY, true);
		drawForces[1].setName("draw flow res 1");
		drawForces[2].setup(simulationWidth, simulationHeight, FT_FLUID_TEMPERATURE, true);
		drawForces[2].setName("draw flow res 2");
		drawForces[3].setup(densityWidth, densityHeight, FT_FLUID_DENSITY, false);
		drawForces[3].setName("draw full res");
		drawForces[4].setup(simulationWidth, simulationHeight, FT_FLUID_VELOCITY, false);
		drawForces[4].setName("draw flow res 1");
		drawForces[5].setup(simulationWidth, simulationHeight, FT_FLUID_TEMPERATURE, false);
		drawForces[5].setName("draw flow res 2");
		
		leftButtonParameters.setName("left");
		leftButtonParameters.add(doResetDrawForces.set("reset", false));
		rightButtonParameters.setName("right");
		rightButtonParameters.add(doResetDrawForces.set("reset", false));
		doResetDrawForces.addListener(this, &ftDrawMouseForces::resetDrawForcesListner);
		for (int i=0; i<3; i++) {
			leftButtonParameters.add(drawForces[i].parameters);
			rightButtonParameters.add(drawForces[i+3].parameters);
		}
		
		parameters.setName("mouse draw");
		parameters.add(leftButtonParameters);
		parameters.add(rightButtonParameters);
	}
	
	void ftDrawMouseForces::update(float _deltaTime) {
		deltaTime = _deltaTime;
		
		for (int i=0; i<numDrawForces; i++) {
			drawForces[i].update();
		}
	}
	
	void ftDrawMouseForces::draw(int _x, int _y, int _w, int _h, ofBlendMode _blendmode) {
		ofPushStyle();
		ofEnableBlendMode(_blendmode);
		for(int i=0; i<getNumForces(); i++) {
			ofEnableBlendMode(_blendmode);
			if (didChange(i)) {
				ftFlowType dfType = getType(i);
				if (dfType == FT_FLUID_DENSITY)
					getTextureReference(i).draw(_x, _y, _w, _h);
			}
		}
		
		ofPopStyle();
		
	}
	
	//--------------------------------------------------------------
	bool ftDrawMouseForces::didChange(int _index) {
		if (_index < 0 || _index >= numDrawForces) {
			ofLogWarning("ftDrawMouseForces::getDrawForceType: index out of range");
			return false;
		}
		else
			return drawForces[_index].didChange();
	}
	
	//--------------------------------------------------------------
	ftFlowType ftDrawMouseForces::getType(int _index) {
		if (_index < 0 || _index >= numDrawForces) {
			ofLogWarning("ftDrawMouseForces::getDrawForceType: index out of range");
			return FT_NONE;
		}
		else
			return drawForces[_index].getType();
	}
	
	//--------------------------------------------------------------
	ofTexture& ftDrawMouseForces::getTextureReference(int _index) {
		if (_index < 0 || _index >= numDrawForces) {
			ofLogError("ftDrawMouseForces::getTexture: index out of range");
		}
		else
			return drawForces[_index].getTexture();
		
	}
	
	//--------------------------------------------------------------
	float ftDrawMouseForces::getStrength(int _index) {
		if (_index < 0 || _index >= numDrawForces) {
			ofLogWarning("ftDrawMouseForces::getStrength: index out of range");
			return 0;
		}
		else {
			if (drawForces[_index].getIsTemporary()) {
				return drawForces[_index].getStrength();
			}
			else {
				return drawForces[_index].getStrength() * deltaTime;;
			}
		}
	}
	
	//--------------------------------------------------------------
	void ftDrawMouseForces::mouseDragged( ofMouseEventArgs& mouse ) {
		glm::vec2 normalizedMouse;
		
		normalizedMouse = glm::vec2 (mouse.x / (float)ofGetWindowWidth(), mouse.y / (float)ofGetWindowHeight());
		
		glm::vec2 velocity = normalizedMouse - lastNormalizedMouse;
		
		if (mouse.button == 0) {
			
			for (int i=0; i<3; i++) {
				if (drawForces[i].getType() == FT_FLUID_VELOCITY)
					drawForces[i].setForce(velocity);
				drawForces[i].applyForce(normalizedMouse);
			}
		}
		else {
			
			for (int i=3; i<numDrawForces; i++) {
				if (drawForces[i].getType() == FT_FLUID_VELOCITY)
					drawForces[i].setForce(velocity);
				drawForces[i].applyForce(normalizedMouse);
			}
		}
		lastNormalizedMouse = normalizedMouse;
		
	}
	
	//--------------------------------------------------------------
	void ftDrawMouseForces::mouseMoved( ofMouseEventArgs& mouse ){
		glm::vec2 normalizedMouse;
		normalizedMouse = glm::vec2(mouse.x / (float)ofGetWindowWidth(), mouse.y / (float)ofGetWindowHeight());
		lastNormalizedMouse = normalizedMouse;
	}
	
}
