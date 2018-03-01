
#include "ftAreaAverage1f.h"

namespace flowTools {
	
	void ftAreaAverage1f::setup(int _width, int _height, string _name) {
		allocate(_width, _height);
		
		totalMagnitude = 0;
		meanMagnitude = 0;
		stdevMagnitude = 0;
		
		parameters.setName("average " + _name);
		parameters.add(pMeanMagnitude.set("mean mag", 0, 0, 0.1));
		parameters.add(pTotalMagnitude.set("total mag", "0"));
		parameters.add(pStdevMagnitude.set("stdev mag", "0"));
		
		roiParameters.setName("ROI ");
		roiParameters.add(pRoiX.set("x", 0, 0, 1));
		pRoiX.addListener(this, &ftAreaAverage1f::pRoiXListener);
		roiParameters.add(pRoiY.set("y", 0, 0, 1));
		pRoiY.addListener(this, &ftAreaAverage1f::pRoiYListener);
		roiParameters.add(pRoiWidth.set("width", 1, 0, 1));
		roiParameters.add(pRoiHeight.set("height", 1, 0, 1));
		parameters.add(roiParameters);
	}
	
	void ftAreaAverage1f::allocate(int _width, int _height) {
		width = _width;
		height = _height;
		pixelCount = _width * _height;
		
		scaleFbo.allocate(_width, _height, GL_R32F);
		ftUtil::black(scaleFbo);
		pixels.allocate(_width, _height, 1);
		
		magnitudes.resize(pixelCount, 0);
	}
	
	void ftAreaAverage1f::update() {
		// read to pixels
		ofTextureData& texData = scaleFbo.getTexture().getTextureData();
		ofSetPixelStoreiAlignment(GL_PACK_ALIGNMENT,width,4,1);
		glBindTexture(texData.textureTarget, texData.textureID);
		glGetTexImage(texData.textureTarget, 0, GL_R, GL_FLOAT, pixels.getData());
		glBindTexture(texData.textureTarget, 0);
		float* floatPixelData = pixels.getData();
		
		// calculate magnitudes
		totalMagnitude = 0;
		float highMagnitude = 0;
		
		for (int i=0; i<pixelCount; i++) {
			magnitudes[i] = floatPixelData[i];
			totalMagnitude += magnitudes[i];
			highMagnitude = max(highMagnitude, magnitudes[i]);
		}
		
		meanMagnitude = totalMagnitude / pixelCount;
		
		std::vector<float> diff(magnitudes.size());
		std::transform(magnitudes.begin(), magnitudes.end(), diff.begin(),
					   std::bind2nd(std::minus<float>(), meanMagnitude));
		float sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
		stdevMagnitude = std::sqrt(sq_sum / magnitudes.size());
		
		pMeanMagnitude.set(meanMagnitude);
		pTotalMagnitude.set(ofToString(totalMagnitude));
		pStdevMagnitude.set(ofToString(stdevMagnitude));
		
	}
}
