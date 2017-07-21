
#include "ftAreaAverage3f.h"

namespace flowTools {
	
	void ftAreaAverage3f::setup(int _width, int _height, string _name) {
		width = _width;
		height = _height;
		
		pixelCount = _width * _height;
		
		scaleFactor = 1.0;
		
		scaleFbo.allocate(_width, _height, GL_RGB32F);
		scaleFbo.black();
		pixels.allocate(_width, _height, 3);
		
		quad.getVertices().resize(4);
		quad.getTexCoords().resize(4);
		quad.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
		
		quad.setVertex(0, ofVec3f(0,0,0));
		quad.setVertex(1, ofVec3f(width,0,0));
		quad.setVertex(2, ofVec3f(width,height,0));
		quad.setVertex(3, ofVec3f(0,height,0));
		
		magnitudes.resize(pixelCount, 0);
		velocities.resize(pixelCount, ofVec3f(0));
		
		direction = ofVec3f(0);
		totalMagnitude = 0;
		meanMagnitude = 0;
		stdevMagnitude = 0;
		
		parameters.setName("average " + _name);
		parameters.add(pMeanMagnitude.set("mean mag", 0, 0, 0.1));
		parameters.add(pDirection.set("direction", ofVec3f(0), ofVec3f(0), ofVec3f(1)));
//		parameters.add(pTotalMagnitude.set("total mag", "0"));
//		parameters.add(pStdevMagnitude.set("stdev mag", "0"));
		
		roiParameters.setName("ROI ");
		roiParameters.add(pRoiX.set("x", 0, 0, 1));
		pRoiX.addListener(this, &ftAreaAverage3f::pRoiXListener);
		roiParameters.add(pRoiY.set("y", 0, 0, 1));
		pRoiY.addListener(this, &ftAreaAverage3f::pRoiYListener);
		roiParameters.add(pRoiWidth.set("width", 1, 0, 1));
		roiParameters.add(pRoiHeight.set("height", 1, 0, 1));
		parameters.add(roiParameters);
		parameters.add(pScaleFactor.set("scale", 0.5, 0.1, 1));
		pScaleFactor.addListener(this, &ftAreaAverage3f::pScaleFactorListener);
	}

	void ftAreaAverage3f::setSize(int _width, int _height) {
		if ( _width != width || _height != height ) {
			width = _width;
			height = _height;
			pixelCount = _width * _height;
			
			scaleFbo.allocate(_width, _height, GL_RGB32F);
			pixels.allocate(_width, _height, 3);
			
			magnitudes.resize(pixelCount, 0);
			velocities.resize(pixelCount, ofVec3f(0));
			
			quad.setVertex(0, ofVec3f(0,0,0));
			quad.setVertex(1, ofVec3f(width,0,0));
			quad.setVertex(2, ofVec3f(width,height,0));
			quad.setVertex(3, ofVec3f(0,height,0));
		}
	}
	
	void ftAreaAverage3f::update() {
		// read to pixels
		ofTextureData& texData = scaleFbo.getTexture().getTextureData();
		ofSetPixelStoreiAlignment(GL_PACK_ALIGNMENT,width,4,3);
		glBindTexture(texData.textureTarget, texData.textureID);
		glGetTexImage(texData.textureTarget, 0, GL_RGB, GL_FLOAT, pixels.getData());
		glBindTexture(texData.textureTarget, 0);
		float* floatPixelData = pixels.getData();
		
		// calculate magnitudes
		totalVelocity = ofVec3f(0);
		totalMagnitude = 0;
		float highMagnitude = 0;
		
		for (int i=0; i<pixelCount; i++) {
			ofVec3f *velocity = &velocities[i];
			velocity->x = floatPixelData[i*3];
			velocity->y = floatPixelData[i*3+1];
			velocity->z = floatPixelData[i*3+2];
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
//		pTotalMagnitude.set(ofToString(totalMagnitude));
//		pStdevMagnitude.set(ofToString(stdevMagnitude));
		
	}
}