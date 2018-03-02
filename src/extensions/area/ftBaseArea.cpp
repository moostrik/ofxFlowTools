#include "ftBaseArea.h"

namespace flowTools {
	
	void ftBaseArea::setup(int _numchannels, int _width, int _height, string _name) {
		
		numChannels = _numchannels;
		
		switch ( _numchannels ) {
			case 1:
				internalFormat = GL_R32F;
				internalReadFormat = GL_R;
				break;
			case 2:
				internalFormat = GL_RG32F;
				internalReadFormat = GL_RG;
				break;
			case 3:
				internalFormat = GL_RGB32F;
				internalReadFormat = GL_RGB;
				break;
			case 4:
				internalFormat = GL_RGBA32F;
				internalReadFormat = GL_RGBA;
				break;
		}
		
		width = _width;
		height = _height;
		pixelCount = _width * _height;
		roi = ofRectangle(0,0,1,1);
		
		pixels.allocate(_width, _height, numChannels);
		scaleFbo.allocate(_width, _height, internalFormat);
		ftUtil::black(scaleFbo);
		
		magnitudes.resize(pixelCount, 0);
		
		totalMagnitude = 0;
		meanMagnitude = 0;
		stdevMagnitude = 0;
		
		parameters.setName("average " + _name);
		parameters.add(pMeanMagnitude.set("mean mag", 0, 0, 0.1));
		parameters.add(pTotalMagnitude.set("total mag", "0"));
		parameters.add(pStdevMagnitude.set("stdev mag", "0"));
		
		roiParameters.setName("ROI " + _name);
		roiParameters.add(pRoiX.set("x", 0, 0, 1));
		pRoiX.addListener(this, &ftBaseArea::pRoiXListener);
		roiParameters.add(pRoiY.set("y", 0, 0, 1));
		pRoiY.addListener(this, &ftBaseArea::pRoiYListener);
		roiParameters.add(pRoiWidth.set("width", 1, 0, 1));
		roiParameters.add(pRoiHeight.set("height", 1, 0, 1));
		parameters.add(roiParameters);
	}
	
	void ftBaseArea::readPixels() {
		ftUtil::toPixels(scaleFbo, pixels);
	}
	
	void ftBaseArea::setTexture(ofTexture& _texture) {
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		ftUtil::black(scaleFbo);
		ftUtil::roi(scaleFbo, _texture, roi);
		ofPopStyle();
	}
	
	void ftBaseArea::setMask(ofTexture& _texture) {
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_MULTIPLY);
		ftUtil::stretch(scaleFbo, _texture);
		ofPopStyle();
	}
	
	void ftBaseArea::getMeanStDev(vector<float> &_v, float &_mean, float &_stDev) {
		float mean = accumulate(_v.begin(), _v.end(), 0.0) / (float)_v.size();
		std::vector<float> diff(_v.size());
		std::transform(_v.begin(), _v.end(), diff.begin(), std::bind2nd(std::minus<float>(), mean));
		float sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
		float stDev = std::sqrt(sq_sum / _v.size());
		
		_mean = mean;
		_stDev = stDev;
	}
}
