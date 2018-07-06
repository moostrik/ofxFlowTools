#include "ftAreaFlow.h"

namespace flowTools {
	
	int ftAreaFlow::areaCount = 0;
	
	void ftAreaFlow::setup(int _width, int _height, ftFlowForceType _type) {
		areaCount++;
		type = _type;
		GLint internalFormat = ftUtil::getInternalFormatFromType(type);
		ftFlow::allocate(_width, _height, internalFormat);
		roiFbo.allocate(_width, _height, internalFormat);
		ftUtil::zero(roiFbo);
		numChannels = ftUtil::getNumChannelsFromInternalFormat(internalFormat);
		
		direction.clear();
		direction.resize(numChannels, 0);
		velocity.clear();
		velocity.resize(numChannels, 0);
		
		inputPixels.allocate(inputWidth, inputHeight, numChannels);
		magnitudes.resize(inputWidth * inputHeight, 0);
		
		roi = ofRectangle(0,0,1,1);
		
		meanMagnitude = 0;
		stdevMagnitude = 0;
		
		parameters.setName("area " + ftFlowForceNames[type] + " " + ofToString(areaCount));
		parameters.add(pMeanMagnitude.set("mean mag", 0, 0, 1));
//		parameters.add(pStdevMagnitude.set("stdev mag", 0, 0, 1));
		
		vector<string> componentNames;
		switch (_type) {
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
			default:
				componentNames = {"unknown 0", "unknown 1", "unknown 2", "unknown 3"};
				break;
		}
		if (numChannels > 1) {
			pComponents.resize(numChannels);
			pDirection.resize(numChannels);
			componentParameters.setName("components");
			directionParameters.setName("direction");
			for (int i=0; i<numChannels; i++) {
				componentParameters.add(pComponents[i].set(componentNames[i], 0, -1, 1));
				directionParameters.add(pDirection[i].set(componentNames[i], 0, -1, 1));
			}
			parameters.add(componentParameters);
//			parameters.add(directionParameters);
		}
		
		roiParameters.setName("ROI");
		pRoi.resize(4);
		roiParameters.add(pRoi[0].set("x", 0, 0, 1));
		roiParameters.add(pRoi[1].set("y", 0, 0, 1));
		roiParameters.add(pRoi[2].set("width", 1, 0, 1));
		roiParameters.add(pRoi[3].set("height", 1, 0, 1));
		for (int i=0; i<4; i++) {
			pRoi[i].addListener(this, &ftAreaFlow::pRoiListener);
		}
		parameters.add(roiParameters);
	}
	
	void ftAreaFlow::setInput(ofTexture &_tex){
		resetInput();
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		ftUtil::roi(inputFbo, _tex, roi);
		ofPopStyle();
		bInputSet = true;
	}
	
	void ftAreaFlow::addInput(ofTexture &_tex, float _strength) {
		resetInput();
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		ftUtil::roi(roiFbo, _tex, roi);
		ofPopStyle();
		ftFlow::addInput(roiFbo.getTexture(), _strength);
		bInputSet = true;
	}
//
	void ftAreaFlow::update() {
		ftUtil::toPixels(inputFbo, inputPixels);
		float* floatPixelData = inputPixels.getData();
		
		vector<float> totalVelocity;
		totalVelocity.resize(numChannels, 0);
		
		int numPixels = inputWidth * inputHeight;
		for (int i=0; i<numPixels; i++) {
			float mag = 0;
			for (int j=0; j<numChannels; j++) {
				float vel = floatPixelData[i * numChannels + j];
				totalVelocity[j] += vel;
				mag += vel * vel;
			}
			magnitudes[i] = sqrt(mag);
		}
		getMeanStDev(magnitudes, meanMagnitude, stdevMagnitude);
		
		if (numChannels > 1) {
			float length;
			for (int i=0; i<numChannels; i++) {
				length += totalVelocity[i] * totalVelocity[i];
			}
			length = sqrt(length);
			for (int i=0; i<numChannels; i++) {
				direction[i] = totalVelocity[i] / length;
				velocity[i] = direction[i] * meanMagnitude;
			}
		}
		else {
			direction[0] = 1; // 1 for normalization with one component
			velocity[0] = meanMagnitude;
		}
		
		if (numChannels > 1) {
			for (int i=0; i<numChannels; i++) {
				pComponents[i] = velocity[i];
				pDirection[i] = direction[i];
			}
		}
		
		pMeanMagnitude.set(meanMagnitude);
		pStdevMagnitude.set(stdevMagnitude);
		
	}
	
	void ftAreaFlow::draw(int _x, int _y, int _w, int _h) {
		int x = _x + roi.x * _w;
		int y = _y + roi.y * _h;
		int w = roi.width * _w;
		int h = roi.height * _h;
		ofPushStyle();
		ofSetColor(128, 128, 128, 64);
		ofDrawRectangle(x, y, w, h);
		ofPopStyle();
	}
	
	void ftAreaFlow::setRoi(ofRectangle _rect) {
		float x = _rect.x;
		float y = _rect.y;
		float maxW = 1.0 - x;
		float maxH = 1.0 - y;
		float w = min(_rect.width, maxW);
		float h = min(_rect.height, maxH);
		
		roi = ofRectangle(x, y, w, h);
		
		if (pRoi[0] != x) { pRoi[0].set(x); }
		if (pRoi[1] != y) { pRoi[1].set(y); }
		if (pRoi[2].getMax() != maxW) { pRoi[2].setMax(maxW); pRoi[2].set(w); }
		if (pRoi[3].getMax() != maxH) { pRoi[3].setMax(maxH); pRoi[3].set(h); }
		if (pRoi[2] != w) { pRoi[2].set(w); }
		if (pRoi[3] != h) { pRoi[3].set(h); }
	}
	
	void ftAreaFlow::getMeanStDev(vector<float> &_v, float &_mean, float &_stDev) {
		float mean = accumulate(_v.begin(), _v.end(), 0.0) / (float)_v.size();
		std::vector<float> diff(_v.size());
		std::transform(_v.begin(), _v.end(), diff.begin(), std::bind2nd(std::minus<float>(), mean));
		float sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
		float stDev = std::sqrt(sq_sum / _v.size());
		
		_mean = mean;
		_stDev = stDev;
	}
}
