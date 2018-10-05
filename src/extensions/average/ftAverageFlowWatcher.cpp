#include "ftAverageFlowWatcher.h"

namespace flowTools {
	
	void ftAverageFlowWatcher::setup(int _width, int _height, flowTools::ftFlowForceType _type)   {
		
		ftAverageFlow::setup(_width, _height, _type);
		
		magnitudeEvent = 0;
		magnitudeActiveHigh= 0;
		magnitudeInActiveLow = 0;
		
		componentEvents.resize(numChannels, 0);
//		componentEventActives.resize(numChannels, 0);
		componentActiveHighs.resize(numChannels, 0);
		componentInActiveLows.resize(numChannels, 0);
		
		baseColor =			ofFloatColor(1., .0, .4, .85);	// orangeish
		thresholdColor =	ofFloatColor(1., .4, .0, .85);	// pinkish
		
		eventParameters.setName("events");
		eventParameters.add(pThreshold.set("threshold", .25, .0, .3));
		eventParameters.add(pBase.set("base", .6, .5, .75));
		eventParameters.add(pMagnitudeEvent.set("magnitude", 0, -1, 1));
		pComponentEvents.resize(numChannels);
		for (int i=0; i<numChannels; i++) {
			eventParameters.add(pComponentEvents[i].set("component " + ofToString(i), false));
		}
		parameters.add(eventParameters);
		
		baseMesh.setMode(OF_PRIMITIVE_LINE_STRIP);
		baseMesh.addIndices(magnitudeMesh.getIndices());
		baseMesh.addVertices(magnitudeMesh.getVertices());
		vector<ofFloatColor> colors;
		colors.resize(graphSize, baseColor);
		baseMesh.addColors(colors);
	}
	
	//--------------------------------------------------------------
	void ftAverageFlowWatcher::update(ofFloatPixels& _pixels)  {
		ftAverageFlow::update(_pixels);
		
		float eV = normalizedMagnitude;
		if (!magnitudeEvent) {
			if (eV < magnitudeInActiveLow) { magnitudeInActiveLow = eV; }
			
			if (eV > magnitudeInActiveLow + pThreshold.get()) {
				magnitudeEvent = true;
				magnitudeActiveHigh = eV;
			}
		}
		if (magnitudeEvent) {
			if (eV > magnitudeActiveHigh) {
				magnitudeActiveHigh = eV;
			}
			if (eV < magnitudeActiveHigh * pThreshold.get()) {
				magnitudeEvent = false;
				magnitudeInActiveLow = eV;
			}
		}
		pMagnitudeEvent = magnitudeEvent;
		
		for (int i=0; i<numChannels; i++) {
			eV = fabs(components[i]);
			if (componentEvents[i] == 0) {
				if (eV < componentInActiveLows[i]) { componentInActiveLows[i] = eV; }
				
				if (eV > componentInActiveLows [i] + pThreshold.get() && eV > normalizedMagnitude * pBase.get()) {
					
					componentEvents[i] = (components[i] > 0)? 1 : -1;
					componentActiveHighs[i] = eV;
				}
			}
			if (componentEvents[i] != 0) {
				if (eV > componentActiveHighs[i]) {
					componentActiveHighs[i] = eV;
				}
				if (eV < componentActiveHighs[i] * pThreshold.get()) {
					componentEvents[i] = 0;
					componentInActiveLows[i] = eV;
				}
			}
			pComponentEvents[i] = componentEvents[i] != 0;
		}
	}
	
	//--------------------------------------------------------------
	void ftAverageFlowWatcher::drawGraph(int _x, int _y, int _w, int _h) {
		if (bUpdateVisualizer) {
			for (int i=0; i<graphSize-1; i++) {
				baseMesh.setVertex(i, glm::vec3(baseMesh.getVertex(i).x, baseMesh.getVertex(i+1).y, 0));
			}
			float b = (type == FT_VELOCITY_SPLIT)? 1.0 - normalizedMagnitude * pBase.get() : 0.5 + normalizedMagnitude * pBase.get() * -.5;
			baseMesh.setVertex(graphSize-1, glm::vec3(baseMesh.getVertex(graphSize-1).x, b, 0));
			// DRAW EVENTS HERE
		}
		
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_ALPHA);
		ofPushView();
		ofTranslate(_x, _y);
		ofScale(_w, _h);
		ofSetColor(thresholdColor);
		if (type != FT_VELOCITY_SPLIT) {
			float t = 0.5 + pThreshold.get() * .5;
			ofDrawLine(0, t, 1, t);
			t = 0.5 + pThreshold.get() * -.5;
			ofDrawLine(0, t, 1, t);
		}
		else {
			float t = 1.0 - pThreshold.get();
			ofDrawLine(0, t, 1, t);
		}
		baseMesh.draw();
		ofPopView();
		ofPopStyle();
		
		ftAverageFlow::drawGraph(_x, _y, _w, _h);
		
//
//			ofSetColor(magnitudeColor);
//			ofDrawLine(_w - 4, (1 - prevNormalizedMagnitude) * halfH, _w, (1 - normalizedMagnitude) * halfH);
//			ofDrawLine(_w - 4, 1 + (1 - prevNormalizedMagnitude) * halfH, _w, 1 + (1 - normalizedMagnitude) * halfH);
//			prevNormalizedMagnitude = normalizedMagnitude;
//			if (getMagnitudeEvent()) {
//				ofSetColor(ofFloatColor(magnitudeColor.r, magnitudeColor.g, magnitudeColor.b, .75));
//				if (type != FT_VELOCITY_SPLIT) {
//					ofDrawRectangle(_w - 4, halfH - eventH * .5, 4, eventH);
//				}
//				else {
//					ofDrawRectangle(_w - 4, halfH - eventH * 1.5 , 4, eventH);
//				}
//			}
//
//			for (int i=0; i<numChannels; i++) {
//				ofSetColor(componentColors[i]);
//				ofDrawLine(_w - 4, (1 - prevComponents[i]) * halfH, _w, (1 - getComponent(i)) * halfH);
//				prevComponents[i] = getComponent(i);
//				ofSetColor(ofFloatColor(componentColors[i].r, componentColors[i].g, componentColors[i].b, .75));
//				if (getComponentEvent(i) == 1) {
//					ofDrawRectangle(_w - 4, eventH * (i * 1.5 + 0.5), 4, eventH);
//				}
//				if (getComponentEvent(i) == -1) {
//					ofDrawRectangle(_w - 4, _h - eventH * (i * 1.5 + 1.5), 4, eventH);
//				}
//			}
//			outputFbo.end();
//			ofPopStyle();
//
//		}
//		bUpdateVisualizer = false;
//
//		outputFbo.draw(_x, _y);
//		overlayFbo.draw(_x, _y);
	}
	
	//--------------------------------------------------------------
	void ftAverageFlowWatcher::drawGraphOverlay(int _x, int _y, int _w, int _h) {
		ofPushStyle();
		ofPushView();
		ofTranslate(_x, _y);
		
		int yStep = 16;
		int yOffset = yStep;
		ofDrawBitmapStringHighlight(parameters.getName(), 5, yOffset);
		yOffset += yStep * 1.5;
		ofSetColor(magnitudeColor);
		ofDrawBitmapString("magnitude",5, yOffset);
		yOffset += yStep;
		
		for (int i=0; i<numChannels; i++) {
			ofSetColor(componentColors[i]);
			ofDrawBitmapString(ftUtil::getComponentName(type, i), 5, yOffset);
			yOffset += yStep;
		}
		
		ofSetColor(thresholdColor);
		ofDrawBitmapString("event threshold", 5, yOffset);
		yOffset += yStep;
		ofSetColor(baseColor);
		ofDrawBitmapString("event base", 5, yOffset);
		
		ofSetColor(255,255,255,255);
		if (type != FT_VELOCITY_SPLIT) {
			ofDrawBitmapString("1",  _w - 10, yStep);
			ofDrawBitmapString("0",  _w - 10, (_h * 0.5) + yStep);
			ofDrawBitmapString("-1", _w - 18, _h - yStep * .5);
			ofSetColor(255,255,255,195);
			ofDrawLine(0, 0.5 * _h, _w, 0.5 * _h);
		} else {
			ofDrawBitmapString("1", _w - 10, yStep);
			ofDrawBitmapString("0", _w - 10, _h - yStep * .5);
		}
		
		ofPopView();
		ofPopStyle();
	}
	
};
