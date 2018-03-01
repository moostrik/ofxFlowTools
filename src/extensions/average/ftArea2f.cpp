
#include "ftArea2f.h"

namespace flowTools {
	
	void ftArea2f::setup(int _width, int _height, string _name) {
		allocate(_width, _height);
		
		direction = ofVec2f(0);
		totalMagnitude = 0;
		meanMagnitude = 0;
		stdevMagnitude = 0;
		
		parameters.setName("average " + _name);
		parameters.add(pMeanMagnitude.set("mean mag", 0, 0, 0.1));
		parameters.add(pDirection.set("direction", ofVec2f(0), ofVec2f(-1), ofVec2f(1)));
		parameters.add(pTotalMagnitude.set("total mag", "0"));
		parameters.add(pStdevMagnitude.set("stdev mag", "0"));
		
		roiParameters.setName("ROI ");
		roiParameters.add(pRoiX.set("x", 0, 0, 1));
		pRoiX.addListener(this, &ftArea2f::pRoiXListener);
		roiParameters.add(pRoiY.set("y", 0, 0, 1));
		pRoiY.addListener(this, &ftArea2f::pRoiYListener);
		roiParameters.add(pRoiWidth.set("width", 1, 0, 1));
		roiParameters.add(pRoiHeight.set("height", 1, 0, 1));
		parameters.add(roiParameters);
	}
	
	void ftArea2f::allocate(int _width, int _height) {
		width = _width;
		height = _height;
		pixelCount = _width * _height;
		
		scaleFbo.allocate(_width, _height, GL_RG32F);
		ftUtil::black(scaleFbo);
		pixels.allocate(_width, _height, 2);
		
		magnitudes.resize(pixelCount, 0);
		velocities.resize(pixelCount, ofVec2f(0));
	}
	
	void ftArea2f::update() {
		// read to pixels
		ofTextureData& texData = scaleFbo.getTexture().getTextureData();
		ofSetPixelStoreiAlignment(GL_PACK_ALIGNMENT,width,4,2);
		glBindTexture(texData.textureTarget, texData.textureID);
		glGetTexImage(texData.textureTarget, 0, GL_RG, GL_FLOAT, pixels.getData());
		glBindTexture(texData.textureTarget, 0);
		float* floatPixelData = pixels.getData();
		
		// calculate magnitudes
		totalVelocity = ofVec2f(0);
		totalMagnitude = 0;
		float highMagnitude = 0;
		
		for (int i=0; i<pixelCount; i++) {
			ofVec2f *velocity = &velocities[i];
			velocity->x = floatPixelData[i*2];
			velocity->y = floatPixelData[i*2+1];
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