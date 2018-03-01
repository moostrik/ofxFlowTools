
#include "ftArea4f.h"

namespace flowTools {
	
	void ftArea4f::setup(int _width, int _height, string _name) {
		allocate(_width, _height);
		
		direction = ofVec4f(0);
		totalMagnitude = 0;
		meanMagnitude = 0;
		stdevMagnitude = 0;
		
		parameters.setName("average " + _name);
		parameters.add(pMeanMagnitude.set("mean mag", 0, 0, 0.1));
		parameters.add(pDirection.set("direction", ofVec4f(0), ofVec4f(0), ofVec4f(1)));
		parameters.add(pTotalMagnitude.set("total mag", "0"));
		parameters.add(pStdevMagnitude.set("stdev mag", "0"));
		
		roiParameters.setName("ROI ");
		roiParameters.add(pRoiX.set("x", 0, 0, 1));
		pRoiX.addListener(this, &ftArea4f::pRoiXListener);
		roiParameters.add(pRoiY.set("y", 0, 0, 1));
		pRoiY.addListener(this, &ftArea4f::pRoiYListener);
		roiParameters.add(pRoiWidth.set("width", 1, 0, 1));
		roiParameters.add(pRoiHeight.set("height", 1, 0, 1));
		parameters.add(roiParameters);
	}
	
	void ftArea4f::allocate(int _width, int _height) {
		width = _width;
		height = _height;
		pixelCount = _width * _height;
		
		scaleFbo.allocate(_width, _height, GL_RGBA32F);
		ftUtil::black(scaleFbo);
		pixels.allocate(_width, _height, 4);
		
		magnitudes.resize(pixelCount, 0);
		velocities.resize(pixelCount, ofVec4f(0));
	}
	
	void ftArea4f::update() {
		// read to pixels
		ofTextureData& texData = scaleFbo.getTexture().getTextureData();
		ofSetPixelStoreiAlignment(GL_PACK_ALIGNMENT,width,4,4);
		glBindTexture(texData.textureTarget, texData.textureID);
		glGetTexImage(texData.textureTarget, 0, GL_RGBA, GL_FLOAT, pixels.getData());
		glBindTexture(texData.textureTarget, 0);
		float* floatPixelData = pixels.getData();
		
		// calculate magnitudes
		totalVelocity = ofVec4f(0);
		totalMagnitude = 0;
		float highMagnitude = 0;
		
		for (int i=0; i<pixelCount; i++) {
			ofVec4f *velocity = &velocities[i];
			velocity->x = floatPixelData[i*4];
			velocity->y = floatPixelData[i*4+1];
			velocity->z = floatPixelData[i*4+2];
			velocity->w = floatPixelData[i*4+3];
			totalVelocity += *velocity;
			
			magnitudes[i] = velocity->length();
			totalMagnitude += magnitudes[i];
			highMagnitude = max(highMagnitude, magnitudes[i]);
		}
		
		direction = totalVelocity.normalize();
		meanMagnitude = totalMagnitude / pixelCount;
		
		std::vector<float> diff(magnitudes.size());
		std::transform(magnitudes.begin(), magnitudes.end(), diff.begin(),
					   std::bind2nd(std::minus<float>(), meanMagnitude));
		float sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
		stdevMagnitude = std::sqrt(sq_sum / magnitudes.size());
		
		pMeanMagnitude.set(meanMagnitude);
		pDirection.set(direction);
		pTotalMagnitude.set(ofToString(totalMagnitude));
		pStdevMagnitude.set(ofToString(stdevMagnitude));
	}
}
