#include "ftAverageFlow.h"

namespace flowTools {
	
	int ftAverageFlow::averageFlowCount = 0;
	
	//--------------------------------------------------------------
	void ftAverageFlow::setup(int _width, int _height, ftFlowForceType _type) {
		averageFlowCount++;
		type = _type;
		GLint internalFormat = ftUtil::getInternalFormatFromType(type);
		ftFlow::allocate(_width, _height, internalFormat);
		numChannels = ftUtil::getNumChannelsFromInternalFormat(internalFormat);
		
		bInputChanged = false;
		inputPixels.allocate(inputWidth, inputHeight, numChannels);
		roiPixels.allocate(inputWidth, inputHeight, numChannels);
		roi = ofRectangle(0,0,1,1);
		
		magnitudes.clear();
		magnitudes.reserve(inputWidth * inputHeight);
		magnitudes.resize(inputWidth * inputHeight, 0);
		direction.clear();
		direction.resize(numChannels, 0);
		areas.clear();
		areas.resize(numChannels, 0);
		components.clear();
		components.resize(numChannels, 0);
		
		prevComponents.clear();
		prevComponents.resize(numChannels, 0);
		
		meanMagnitude = 0;
		normalizedMagnitude = 0;
		prevNormalizedMagnitude = 0;
		stdevMagnitude = 0;
		
		// move draw graph out of average;
		magnitudeColor = ofFloatColor(1, 1, 1, 1.);
		componentColors.push_back(ofFloatColor(.6, 1, .2, 1.));	// light green
		componentColors.push_back(ofFloatColor(.4, .8, 1, 1.));	// blue
		componentColors.push_back(ofFloatColor(.2, 1, .6, 1.));	// dark green
		componentColors.push_back(ofFloatColor(.8, .4, 1, 1.));	// purple
		outputFbo.allocate(16, 16);
		overlayFbo.allocate(16, 16);
		bUpdateVisualizer = false;
		// move draw graph out of average;
		
		setupParameters();
		
	}
	
	void ftAverageFlow::setupParameters() {
		
		string name = "average " + ftFlowForceNames[type];
		if (averageFlowCount > 1) name += " " + ofToString(averageFlowCount - 1);
		parameters.setName(name);
		parameters.add(pNormalizationMax.set("normalization", .025, .01, .1));
		parameters.add(pNormalizedMagnitude.set("magnitude", 0, 0, 1));
		//		parameters.add(pStdevMagnitude.set("stdev mag", 0, 0, 1));
		
		if (type == FT_VELOCITY_SPLIT) {
			parameters.add(pHighComponentBoost.set("boost directionality", 0, 0, 5));
		}	else { pHighComponentBoost.set(0); }
		pComponents.resize(numChannels);
		pDirection.resize(numChannels);
		if (numChannels > 1) {
			componentParameters.setName("components");
			for (int i=0; i<numChannels; i++) {
				componentParameters.add(pComponents[i].set(getComponentName(i), 0, -1, 1));
			}
			parameters.add(componentParameters);
		} else {
			parameters.add(pComponents[0].set(getComponentName(0), 0, -1, 1));
			parameters.add(pDirection[0].set(getComponentName(0), 0, -1, 1));
		}
		
		pAreas.resize(numChannels);
		if (numChannels > 1) {
			areaParameters.setName("areas");
			for (int i=0; i<numChannels; i++) {
				areaParameters.add(pAreas[i].set(getComponentName(i), 0, 0, 1));
			}
			parameters.add(areaParameters);
		} else {
			parameters.add(pAreas[0].set(getComponentName(0), 0, 0, 1));
		}
		
		roiParameters.setName("region of interest");
		pRoi.resize(4);
		roiParameters.add(pRoi[0].set("x", 0, 0, 1));
		roiParameters.add(pRoi[1].set("y", 0, 0, 1));
		roiParameters.add(pRoi[2].set("width", 1, 0, 1));
		roiParameters.add(pRoi[3].set("height", 1, 0, 1));
		for (int i=0; i<4; i++) {
			pRoi[i].addListener(this, &ftAverageFlow::pRoiListener);
		}
		parameters.add(roiParameters);
	}
	
	//--------------------------------------------------------------
	void ftAverageFlow::update() {
		if (bInputChanged) {
			ftUtil::toPixels(inputFbo, inputPixels);
			bInputChanged = false;
		}
		
		ofRectangle dnRoi = ofRectangle(roi.x * inputWidth, roi.y * inputHeight, roi.width * inputWidth, roi.height * inputHeight);

		int numRoiPixels = dnRoi.width * dnRoi.height;

		getRoiData(inputPixels, roiPixels, dnRoi);

		float* pixelData = roiPixels.getData();
		
		vector<float> totalVelocity;
		totalVelocity.resize(numChannels, 0);
		vector<int> areaCounter;
		areaCounter.resize(numChannels, 0);

		magnitudes.resize(numRoiPixels);
		for (int i=0; i<numRoiPixels; i++) {
			float mag = 0;
			for (int j=0; j<numChannels; j++) {
				float vel = pixelData[i * numChannels + j];
				if (vel > 0) {
					areaCounter[j]++;
				}
				totalVelocity[j] += vel;
				mag += vel * vel;
			}
			magnitudes[i] = sqrt(mag);
		}
		getMeanStDev(magnitudes, meanMagnitude, stdevMagnitude);
		
		normalizedMagnitude = meanMagnitude / pNormalizationMax.get();
		normalizedMagnitude = ofClamp(normalizedMagnitude, 0, 1);
		
		float totalMagnitude = 0;
		for (auto tv : totalVelocity) { totalMagnitude += tv * tv; }
		totalMagnitude = sqrt(totalMagnitude);
		
		for (int i=0; i<numChannels; i++) {
			if (totalMagnitude > 0) { direction[i] = totalVelocity[i] / totalMagnitude; }
			else { direction[i] = 0; }
			components[i] = direction[i] * normalizedMagnitude;
			areas[i] = areaCounter[i] / (float)numRoiPixels;
		}
		
		// normalize to highest component and apply boost
		if (pHighComponentBoost.get() > 0 && type == FT_VELOCITY_SPLIT) {
			float highVelocity = 0;
			float P = 1;
			for (int i=0; i<numChannels; i++) {
				if (fabs(components[i]) > highVelocity) {
					highVelocity = fabs(components[i]);
					if (components[i] < 0) P = -1;
				}
			}
			for (int i=0; i<numChannels; i++) {
				components[i] /= highVelocity;
				components[i] = powf(fabs(components[i]), pHighComponentBoost.get()) * P;
				components[i] *= highVelocity;
			}
		}
		
		// use only 2 decimals
		for (int i=0; i<numChannels; i++) {
			pComponents[i] = int(components[i] * 100) / 100.0;
			pDirection[i] = int(direction[i] * 100) / 100.0;
			pAreas[i] = int(areas[i] * 100) / 100.0;
		}
		pNormalizedMagnitude.set(int(normalizedMagnitude * 100) / 100.0);
		pStdevMagnitude.set(int(stdevMagnitude * 100) / 100.0);
		
		bUpdateVisualizer = true;
	}
	
	void ftAverageFlow::getRoiData(ofFloatPixels &_pixels, ofFloatPixels& _subPixels, ofRectangle _section) {
		
		int x = _section.x;
		int y = _section.y;
		int w = _section.width;
		int h = _section.height;
		int c = numChannels;
		float* srcPixelData = _pixels.getData();
		float* subPixelData = _subPixels.getData();
		
		for (int iY=0; iY<h; iY++) {
			for (int iX=0; iX<w; iX++) {
				for (int iC=0; iC<c; iC++) {
					int subI = ((iY * w) + iX) * c + iC;
					int srcI = (((y+iY) * inputPixels.getWidth()) + (x+iX)) * c + iC;
					subPixelData[subI] = srcPixelData[srcI];
				}
			}
		}
	}
	
	//--------------------------------------------------------------
	void ftAverageFlow::setInput(ofTexture &_tex){
		ftFlow::setInput(_tex);
		bInputChanged = true;
	}
	
	//--------------------------------------------------------------
	void ftAverageFlow::addInput(ofTexture &_tex, float _strength) {
		ftFlow::addInput(_tex, _strength);
		bInputChanged = true;
	}
	
	//--------------------------------------------------------------
	void ftAverageFlow::drawOutput(int _x, int _y, int _w, int _h) {
		int x = _x + roi.x * _w;
		int y = _y + roi.y * _h;
		int w = roi.width * _w;
		int h = roi.height * _h;
		
		drawVisualizer(x, y, w, h);
	}
	
	//--------------------------------------------------------------
	void ftAverageFlow::drawROI(int _x, int _y, int _w, int _h) {
		int x = _x + roi.x * _w;
		int y = _y + roi.y * _h;
		int w = roi.width * _w;
		int h = roi.height * _h;
		
		drawBackground(x, y, w, h);
	}
	
	//--------------------------------------------------------------
	void ftAverageFlow::drawVisualizer(int _x, int _y, int _w, int _h) {
		drawBackground(_x, _y, _w, _h);
		drawGraph(_x, _y, _w, _h);
	}
	
	//--------------------------------------------------------------
	void ftAverageFlow::drawBackground(int _x, int _y, int _w, int _h) {
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_ALPHA);
		ofSetColor(0, 0, 0, 63);
		ofDrawRectangle(_x, _y, _w, _h);
		ofNoFill();
		ofSetColor(0, 0, 0, 255);
		ofDrawRectangle(_x-1, _y-1, _w+2, _h+2);
		ofPopStyle();
	}
	
	//--------------------------------------------------------------
	void ftAverageFlow::drawGraph(int _x, int _y, int _w, int _h) {
		if (bUpdateVisualizer) {
			if (outputFbo.getWidth() != _w || outputFbo.getHeight() != _h) {
				outputFbo.allocate(_w, _h);
				ftUtil::zero(outputFbo);
				createGraphOverlay(_w, _h);
			}
			
			// graph
			ofPushStyle();
			outputFbo.swap();
			outputFbo.begin();
			ofEnableBlendMode(OF_BLENDMODE_ALPHA);
			ofClear(0, 0, 0, 0);
			ofEnableBlendMode(OF_BLENDMODE_DISABLED);
			ofSetColor(255, 255, 255, 255);
			outputFbo.getBackTexture().draw(-4, 0);
			ofEnableBlendMode(OF_BLENDMODE_ALPHA);
			
			int halfH = _h * .5;
			if (type == FT_VELOCITY_SPLIT) { halfH = (_h); }
			
			ofSetColor(magnitudeColor);
			ofDrawLine(_w - 4, (1 - prevNormalizedMagnitude) * halfH, _w, (1 - normalizedMagnitude) * halfH);
			ofDrawLine(_w - 4, 1 + (1 - prevNormalizedMagnitude) * halfH, _w, 1 + (1 - normalizedMagnitude) * halfH);
			prevNormalizedMagnitude = normalizedMagnitude;
			for (int i=0; i<numChannels; i++) {
				ofSetColor(componentColors[i]);
				ofDrawLine(_w - 4, (1 - prevComponents[i]) * halfH, _w, (1 - getComponent(i)) * halfH);
				prevComponents[i] = getComponent(i);
			}
			outputFbo.end();
			outputFbo.draw(_x, _y, _w, _h);
			
			// overlay
			overlayFbo.draw(_x, _y, _w, _h);
			
			ofPopStyle();
		}
		bUpdateVisualizer = false;
	}
	
	//--------------------------------------------------------------
	void ftAverageFlow::createGraphOverlay(int _w, int _h) {
		overlayFbo.allocate(_w, _h);
		ftUtil::zero(overlayFbo);
		
		overlayFbo.begin();
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_ALPHA);
		ofSetColor(255, 255, 255, 255);
		
		int yStep = 16;
		
		int yOffset = yStep;
		ofDrawBitmapStringHighlight(parameters.getName(), 5, yOffset);
		yOffset += yStep * 1.5;
		ofSetColor(magnitudeColor);
		ofDrawBitmapString("magnitude",5, yOffset);
		yOffset += yStep;
		
		for (int i=0; i<numChannels; i++) {
			ofSetColor(componentColors[i]);
			ofDrawBitmapString(getComponentName(i), 5, yOffset);
			yOffset += yStep;
		}
		
		if (type != FT_VELOCITY_SPLIT) {
			ofDrawBitmapString("1",  _w - 10, yStep);
			ofDrawBitmapString("0",  _w - 10, (_h * 0.5) + yStep);
			ofDrawBitmapString("-1", _w - 18, _h - yStep * .5);
		} else {
			ofDrawBitmapString("1", _w - 10, yStep);
			ofDrawBitmapString("0", _w - 10, _h - yStep * .5);
		}
		ofPopStyle();
		
		overlayFbo.end();
	}
	
	//--------------------------------------------------------------
	void ftAverageFlow::setRoi(ofRectangle _rect) {
		float x = ofClamp(_rect.x, 0, 1);
		float y = ofClamp(_rect.y, 0, 1);
		float maxW = 1.0 - x;
		float maxH = 1.0 - y;
		float w = min(_rect.width, maxW);
		float h = min(_rect.height, maxH);
		
		roi = ofRectangle(x, y, w, h);
		
		if (pRoi[0] != x) { pRoi[0].setWithoutEventNotifications(x); }
		if (pRoi[1] != y) { pRoi[1].setWithoutEventNotifications(y); }
		if (pRoi[2].getMax() != maxW) { pRoi[2].setMax(maxW); pRoi[2].setWithoutEventNotifications(w); }
		if (pRoi[3].getMax() != maxH) { pRoi[3].setMax(maxH); pRoi[3].setWithoutEventNotifications(h); }
		if (pRoi[2] != w) { pRoi[2].setWithoutEventNotifications(w); }
		if (pRoi[3] != h) { pRoi[3].setWithoutEventNotifications(h); }
	}
	
	//--------------------------------------------------------------
	void ftAverageFlow::getMeanStDev(vector<float> &_v, float &_mean, float &_stDev) {
		float mean = accumulate(_v.begin(), _v.end(), 0.0) / (float)_v.size();
		std::vector<float> diff(_v.size());
		std::transform(_v.begin(), _v.end(), diff.begin(), std::bind2nd(std::minus<float>(), mean));
		float sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
		float stDev = std::sqrt(sq_sum / _v.size());
		
		_mean = mean;
		_stDev = stDev;
	}
	
	//--------------------------------------------------------------
	string ftAverageFlow::getComponentName(int _index)  {
		vector<string> componentNames;
		switch (type) {
			case FT_VELOCITY:
			case FT_VELOCITY_NORM:
				componentNames = {"x", "y"};
				break;
			case FT_VELOCITY_SPLIT:
				componentNames = {"right", "down", "left", "up"};
				break;
			case FT_DENSITY:
				componentNames = {"red", "green", "blue", "alpha"};
				break;
			case FT_PRESSURE:
				componentNames = {"pressure"};
				break;
			case FT_TEMPERATURE:
				componentNames = {"temperature"};
				break;
			default:
				componentNames = {"unknown 0", "unknown 1", "unknown 2", "unknown 3"};
				break;
		}
		
		if (_index < componentNames.size()) {
			return componentNames[_index];
		}
		return "unknown";
		
	}
}

