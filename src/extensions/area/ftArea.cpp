#include "ftArea.h"

namespace flowTools {
	
	void ftArea::setup(int _width, int _height, string _name) {
		
		numChannels = 0;
		bAllocated = false;
	
		width = _width;
		height = _height;
		numPixels = _width * _height;
		roi = ofRectangle(0,0,1,1);
		
		meanMagnitude = 0;
		stdevMagnitude = 0;
		magnitudes.resize(numPixels, 0);
		
		parameters.setName("area " + _name);
		parameters.add(pMeanMagnitude.set("mean mag", 0, 0, 1));
		parameters.add(pStdevMagnitude.set("stdev mag", 0, 0, 1));
		pMeanMagnitude.addListener(this, &ftArea::pFloatListener);
		pStdevMagnitude.addListener(this, &ftArea::pFloatListener);
		pVelocity.resize(4);
		for (int i=0; i<4; i++) {
			parameters.add(pVelocity[i].set("velocity " + ofToString(i), 0, -1, 1));
			pVelocity[i].addListener(this, &ftArea::pFloatListener);
		}
		
		roiParameters.setName("ROI " + _name);
		roiParameters.add(pRoiX.set("x", 0, 0, 1));
		pRoiX.addListener(this, &ftArea::pRoiXListener);
		roiParameters.add(pRoiY.set("y", 0, 0, 1));
		pRoiY.addListener(this, &ftArea::pRoiYListener);
		roiParameters.add(pRoiWidth.set("width", 1, 0, 1));
		roiParameters.add(pRoiHeight.set("height", 1, 0, 1));
		parameters.add(roiParameters);
	}
	
	void ftArea::allocate(ofTexture &_tex) {
		ofTextureData& texData = _tex.getTextureData();
		
		internalFormat = texData.glInternalFormat;
		bAllocated = true;
		
		switch(internalFormat){
			case GL_R32F: 		numChannels = 1; break;
			case GL_RG32F: 		numChannels = 2; break;
			case GL_RGB32F: 	numChannels = 3; break;
			case GL_RGBA32F:	numChannels = 4; break;
			default:
				numChannels = 0;
				bAllocated = false;
				ofLogWarning("ftArea") << "allocate: " << "ftArea works with float textures only";
				return;
		}
		
		scaleFbo.allocate(width, height, internalFormat);
		ftUtil::black(scaleFbo);
		
		direction.clear();
		direction.resize(numChannels, 0);
		velocity.clear();
		velocity.resize(numChannels, 0);
		
		pixels.allocate(width, height, numChannels);
	}
	
	void ftArea::update(ofTexture& _texture) {
		if (!bAllocated || ftUtil::getInternalFormat(_texture) != ftUtil::getInternalFormat(scaleFbo)) { allocate(_texture); }
		
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		ftUtil::black(scaleFbo);
		ftUtil::roi(scaleFbo, _texture, roi);
		ofPopStyle();
		
		ftUtil::toPixels(scaleFbo, pixels);
		float* floatPixelData = pixels.getData();
		
		vector<float> totalVelocity;
		totalVelocity.resize(numChannels, 0);
		for (int i=0; i<numPixels; i++) {
			float mag = 0;
			for (int j=0; j<numChannels; j++) {
				float vel = floatPixelData[i * numChannels + j] * 10.0 ; // times 10 should give values closer to 1
				totalVelocity[j] += vel;
				mag += vel * vel;
			}
			magnitudes[i] = sqrt(mag);
		}
		getMeanStDev(magnitudes, meanMagnitude, stdevMagnitude);
		
		float length;
		for (int i=0; i<numChannels; i++) {
			length += totalVelocity[i] * totalVelocity[i];
		}
		length = sqrt(length);
		for (int i=0; i<numChannels; i++) {
			direction[i] = totalVelocity[i] / length; // normalized velocity
			velocity[i] = direction[i] * meanMagnitude;
		}
		
		for (int i=0; i<numChannels; i++) {
			pVelocity[i] = velocity[i];
		}
		
		for (int i=numChannels; i<pVelocity.size(); i++) {
			pVelocity[i] = 0;
		}
		
		pMeanMagnitude.set(meanMagnitude);
		pStdevMagnitude.set(stdevMagnitude);
	}
	
	void ftArea::getMeanStDev(vector<float> &_v, float &_mean, float &_stDev) {
		float mean = accumulate(_v.begin(), _v.end(), 0.0) / (float)_v.size();
		std::vector<float> diff(_v.size());
		std::transform(_v.begin(), _v.end(), diff.begin(), std::bind2nd(std::minus<float>(), mean));
		float sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
		float stDev = std::sqrt(sq_sum / _v.size());
		
		_mean = mean;
		_stDev = stDev;
	}
}
