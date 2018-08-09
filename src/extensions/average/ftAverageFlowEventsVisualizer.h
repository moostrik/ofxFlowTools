
#pragma once

#include "ofMain.h"
#include "ftAverageFlowEvents.h"

// INTEGRATE WITH ftAverageFlowEvents.h

namespace flowTools {
	
	class ftAverageFlowEventsVisualizer: public ftAverageFlowEvents {
	public:
		
		void setup(int _width, int _height, ftFlowForceType _type) override  {
			ftAverageFlowEvents::setup(_width, _height, _type);
			
			historySize = 120;
			historyWidth = historySize * 4;
			historyHeight = 256;
			
			prevMeanMagnitude = 0;
			prevComponents.resize(numChannels, 0);
			
			magnitudeColor=	ofFloatColor(.8, .8, .8, 1.); 				// white
			componentColors.push_back(ofFloatColor(.4, .8, .2, 1.)); 	// light green
			componentColors.push_back(ofFloatColor(.2, .4, .8, 1.)); 	// blue
			componentColors.push_back(ofFloatColor(.2, .8, .4, 1.)); 	// dark green
			componentColors.push_back(ofFloatColor(.4, .2, .8, 1.)); 	// purple
			baseColor = 	ofFloatColor(1., .0, .4, .75); 				// orangeish
			thresholdColor= ofFloatColor(1., .4, .0, .75); 				// pinkish
			
			allocateVisualizer();
			parameters.add(pPauze.set("pauze", false));
			bUpdateVisualizer = false;
		}
		void update() override {
			ftAverageFlowEvents::update();
			bUpdateVisualizer = true;
		}
		
		void drawVisualizer(int _x, int _y) { drawVisualizer(_x, _y, historyWidth, historyHeight); }
		
		void drawVisualizer(int _x, int _y, int _w, int _h) {
			ofPushStyle();
			if (bUpdateVisualizer && !pPauze.get()) {
				bUpdateVisualizer = false;
				
				historyFbo.swap();
				historyFbo.begin();
				ofEnableBlendMode(OF_BLENDMODE_ALPHA);
				ofClear(0, 0, 0, 0);
				ofEnableBlendMode(OF_BLENDMODE_DISABLED);
				ofSetColor(255, 255, 255, 255);
				historyFbo.getBackTexture().draw(-4, 0);
				ofEnableBlendMode(OF_BLENDMODE_ADD);
				
				
				int hHeight = historyHeight * .86;
				int eHeight = historyHeight - hHeight;
				float eStep = eHeight / 6.0;
				
				
				ofSetColor(thresholdColor);
				ofDrawLine(historyWidth - 4, (1 - pThreshold.get()) * hHeight, historyWidth, (1 - pThreshold.get()) * hHeight);
				
				ofSetColor(baseColor);
				ofDrawLine(historyWidth - 4, 1 + (1 - prevMeanMagnitude * pBase.get()) * hHeight, historyWidth, (1 - meanMagnitude * pBase.get()) * hHeight);
				
				ofSetColor(magnitudeColor);
				ofDrawLine(historyWidth - 4, 1 + (1 - prevMeanMagnitude) * hHeight, historyWidth, (1 - meanMagnitude) * hHeight);
				prevMeanMagnitude = meanMagnitude;
				if (getMagnitudeEventActive()) {
					ofDrawLine(historyWidth - 4, hHeight + eStep, historyWidth, hHeight + eStep);
				}
				
				for (int i=0; i<numChannels; i++) {
					ofSetColor(componentColors[i]);
					ofDrawLine(historyWidth - 4, (1 - prevComponents[i]) * hHeight, historyWidth, (1 - getComponent(i)) * hHeight);
					prevComponents[i] = getComponent(i);
					if (getComponentEventActive(i)) {
						ofDrawLine(historyWidth - 4, hHeight + (i+2) * eStep, historyWidth, hHeight + (i+2) * eStep ); }
				}
				
				historyFbo.end();
			}
			
			ofEnableBlendMode(OF_BLENDMODE_ALPHA);
			historyOverlayFbo.draw(_x, _y, _w, _h);
			historyFbo.draw(_x, _y, _w, _h);
			ofPopStyle();
		}
		
		int		getVisualizerWidth()	{ return historyWidth; }
		int		getVisualizerHeight()	{ return historyHeight; }
		
	protected:
		float					prevMeanMagnitude;
		vector<float>		 	prevComponents;
		
		int 					historySize;
		ftPingPongFbo			historyFbo;
		ofFbo					historyOverlayFbo;
		ofPoint 				historyPosition;
		bool					bUpdateVisualizer;
		int						historyWidth, historyHeight;
		
		ofFloatColor		 	magnitudeColor;
		vector<ofFloatColor>	componentColors;
		ofFloatColor		 	baseColor, thresholdColor;
		
		ofParameter<bool>		pPauze;
		
		void allocateVisualizer() {
			historyFbo.allocate(historyWidth, historyHeight, GL_RGBA);
			ftUtil::zero(historyFbo);
			makeOverlay(historyWidth, historyHeight);
		}
		
		void makeOverlay(int _width, int _height) {
			historyOverlayFbo.allocate(_width, _height, GL_RGBA);
			ofPushStyle();
			ofEnableBlendMode(OF_BLENDMODE_ALPHA);
			historyOverlayFbo.begin();
			ofClear(0, 0, 0, 0);
			ofEnableBlendMode(OF_BLENDMODE_ADD);
			ofSetColor(0, 0, 0, 196);
			ofDrawRectangle(0, 0, _width, _height);
			ofNoFill();
			ofDrawRectangle(1, 1, _width-1, _height-1);
			
			ofSetColor(magnitudeColor);
			int yStep = 16;
			int yOffset = yStep;
			ofDrawBitmapString("magnitude", 5, yOffset);
			yOffset += yStep + yStep;
			for (int i=0; i<numChannels; i++) {
				ofSetColor(componentColors[i]);
				ofDrawBitmapString(getComponentName(i), 5, yOffset);
				yOffset += yStep;
			}
			
			yOffset += yStep;
			ofSetColor(thresholdColor);
			ofDrawBitmapString("event threshold", 5, yOffset);
			yOffset += yStep;
			ofSetColor(baseColor);
			ofDrawBitmapString("event base", 5, yOffset);
			
			historyOverlayFbo.end();
			ofPopStyle();
		}
	};
}


