#include "ftAverageFlowWatcher.h"

namespace flowTools {
	
	void ftAverageFlowWatcher::setup(ftFlowForceType _type)   {
		type = _type;
		GLint internalFormat = ftUtil::getInternalFormatFromType(type);
		numChannels = ftUtil::getNumChannelsFromInternalFormat(internalFormat);
		
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
	}
	
	//--------------------------------------------------------------
	void ftAverageFlowWatcher::update()  {
//		ftAverageFlow::update();
//
////		cout << magnitudeEvent << " " << endl;
//
//		float eV = normalizedMagnitude;
//		if (!magnitudeEvent) {
//			if (eV < magnitudeInActiveLow) { magnitudeInActiveLow = eV; }
//
//			if (eV > magnitudeInActiveLow + pThreshold.get() && eV > normalizedMagnitude * pBase.get()) {
//				magnitudeEvent = true;
//				magnitudeActiveHigh = eV;
//			}
//		}
//		if (magnitudeEvent) {
//			if (eV > magnitudeActiveHigh) {
//				magnitudeActiveHigh = eV;
//			}
//			if (eV < magnitudeActiveHigh * pThreshold.get()) {
//				magnitudeEvent = false;
//				magnitudeInActiveLow = eV;
//			}
//		}
//		pMagnitudeEvent = magnitudeEvent;
//
//		for (int i=0; i<numChannels; i++) {
//			eV = fabs(components[i]);
//			if (componentEvents[i] == 0) {
//				if (eV < componentInActiveLows[i]) { componentInActiveLows[i] = eV; }
//
//				if (eV > componentInActiveLows [i] + pThreshold.get() && eV > normalizedMagnitude * pBase.get()) {
//
//					componentEvents[i] = (components[i] > 0)? 1 : -1;
//					componentActiveHighs[i] = eV;
//				}
//			}
//			if (componentEvents[i] != 0) {
//				if (eV > componentActiveHighs[i]) {
//					componentActiveHighs[i] = eV;
//				}
//				if (eV < componentActiveHighs[i] * pThreshold.get()) {
//					componentEvents[i] = 0;
//					componentInActiveLows[i] = eV;
//				}
//			}
//			pComponentEvents[i] = componentEvents[i] != 0;
//		}
	}
	
	//--------------------------------------------------------------
	void ftAverageFlowWatcher::drawGraph(int _x, int _y, int _w, int _h) {
//		if (bUpdateVisualizer) {
//			if (outputFbo.getWidth() != _w || outputFbo.getHeight() != _h) {
//				outputFbo.allocate(_w, _h);
//				ftUtil::zero(outputFbo);
//				createGraphOverlay(_w, _h);
//			}
//
//			// graph
//			ofPushStyle();
//			outputFbo.swap();
//			outputFbo.begin();
//			ofEnableBlendMode(OF_BLENDMODE_ALPHA);
//			ofClear(0, 0, 0, 0);
//			ofEnableBlendMode(OF_BLENDMODE_DISABLED);
//			ofSetColor(255, 255, 255, 255);
//			outputFbo.getBackTexture().draw(-4, 0);
//			ofEnableBlendMode(OF_BLENDMODE_ALPHA);
//
//			int halfH = (_h) * .5;
//			int eventH = _h / ((numChannels * 2 + 1) * 1.5 + 0.5);
//			if (type == FT_VELOCITY_SPLIT) {
//				halfH = (_h);
//				eventH = _h / ((numChannels + 1) * 1.5 + 0.5);
//			}
//
//			ofSetColor(thresholdColor);
//			ofDrawLine(_w - 4, (1 - pThreshold.get()) * halfH, _w, (1 - pThreshold.get()) * halfH);
//			if (type != FT_VELOCITY_SPLIT) { ofDrawLine(_w - 4, _h - (1 - pThreshold.get()) * halfH, _w, _h - (1 - pThreshold.get()) * halfH); }
//
//			ofSetColor(baseColor);
//			ofDrawLine(_w - 4, (1 - prevNormalizedMagnitude * pBase.get()) * halfH, _w, (1 - normalizedMagnitude * pBase.get()) * halfH);
//			if (type != FT_VELOCITY_SPLIT) { ofDrawLine(_w - 4, _h - (1 - prevNormalizedMagnitude * pBase.get()) * halfH, _w, _h - (1 - normalizedMagnitude * pBase.get()) * halfH); }
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
	void ftAverageFlowWatcher::createGraphOverlay(int _w, int _h) {
//		overlayFbo.allocate(_w, _h);
//		ftUtil::zero(overlayFbo);
//		
//		overlayFbo.begin();
//		ofPushStyle();
//		ofEnableBlendMode(OF_BLENDMODE_ALPHA);
//		ofSetColor(255, 255, 255, 255);
//		int yStep = 16;
//		if (type != FT_VELOCITY_SPLIT) {
//			ofDrawBitmapString("1",  _w - 10, yStep);
//			ofDrawBitmapString("0",  _w - 10, (_h * 0.5) + yStep);
//			ofDrawBitmapString("-1", _w - 18, _h - yStep * .5);
//		} else {
//			ofDrawBitmapString("1", _w - 10, yStep);
//			ofDrawBitmapString("0", _w - 10, _h - yStep * .5);
//		}
//		
//		int yOffset = yStep;
//		ofSetColor(magnitudeColor);
//		ofDrawBitmapString("magnitude",5, yOffset);
//		yOffset += yStep;
//		
//		for (int i=0; i<numChannels; i++) {
//			ofSetColor(componentColors[i]);
//			ofDrawBitmapString(getComponentName(i), 5, yOffset);
//			yOffset += yStep;
//		}
//		
//		yOffset += yStep;
//		ofSetColor(thresholdColor);
//		ofDrawBitmapString("event threshold", 5, yOffset);
//		yOffset += yStep;
//		ofSetColor(baseColor);
//		ofDrawBitmapString("event base", 5, yOffset);
//		
//		ofPopStyle();
//		
//		overlayFbo.end();
	}
	
};
