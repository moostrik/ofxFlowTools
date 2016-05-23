

#include "ftSvAverage.h"

// TO FTSPILTVELOCITYAVERAGE

namespace flowTools {
	
	void ftSvAverage::setup(int _width, int _height, string _name) {
		width = _width;
		height = _height;
		pixelCount = width * height;
		
		scaleFbo.allocate(width, height, GL_RGBA32F);
		pixels.allocate(_width, _height, 4);
		
		magnitudes.resize(pixelCount, 0);
		velocities.resize(pixelCount, ofVec4f(0,0,0,0));
//		velocities.resize(pixelCount, ofVec4f());
//		magnitudes = new float [pixelCount];
		
		direction = ofVec4f(0,0,0,0);
		totalMagnitude = 0;
		meanMagnitude = 0;
		stdevMagnitude = 0;
		
		parameters.setName("average " + _name);
		parameters.add(pDirection.set("direction", ofVec2f(0,0), ofVec2f(-1,-1), ofVec2f(1,1)));
		parameters.add(pTotalMagnitude.set("total mag", "0"));
		parameters.add(pMeanMagnitude.set("mean mag", "0"));
		parameters.add(pStdevMagnitude.set("stdev mag", "0"));
		
	}
	
	void ftSvAverage::setTexture(ofTexture _texture) {
		// remove style and blendmode
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		scaleFbo.black();
		scaleFbo.stretchIntoMe(_texture);
		ofPopStyle();
	}
	
	void ftSvAverage::update() {
		ofTextureData& texData = scaleFbo.getTexture().getTextureData();
		// read to pixels
		ofSetPixelStoreiAlignment(GL_PACK_ALIGNMENT,width,4,4);
		glBindTexture(texData.textureTarget, texData.textureID);
		glGetTexImage(texData.textureTarget, 0, GL_RGBA, GL_FLOAT, pixels.getData());
		glBindTexture(texData.textureTarget, 0);
		float* floatPixelData = pixels.getData();
		
		// calculate magnitudes
		
		totalVelocity = ofVec4f(0,0,0,0);
		
		totalMagnitude = 0;
		
		for (int i=0; i<pixelCount; i++) {
			ofVec4f *velocity = &velocities[i];
			velocity->x = floatPixelData[i*4];
			velocity->y = floatPixelData[i*4+1];
			velocity->z = floatPixelData[i*4+2];
			velocity->w = floatPixelData[i*4+3];
			totalVelocity += *velocity;
			
			magnitudes[i] = velocity->length();
			totalMagnitude += magnitudes[i];
		}
		
		direction = totalVelocity.normalize();
		meanMagnitude = totalMagnitude / pixelCount;
		
		std::vector<float> diff(magnitudes.size());
		std::transform(magnitudes.begin(), magnitudes.end(), diff.begin(),
					   std::bind2nd(std::minus<float>(), meanMagnitude));
		float sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
		stdevMagnitude = std::sqrt(sq_sum / magnitudes.size());
		
		pDirection.set(direction);
		pTotalMagnitude.set(ofToString(totalMagnitude));
		pMeanMagnitude.set(ofToString(meanMagnitude));
		pStdevMagnitude.set(ofToString(stdevMagnitude));
		
	}
}