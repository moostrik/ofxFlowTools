
#include "ftArea4f.h"

namespace flowTools {
	
	void ftArea4f::setup(int _width, int _height, string _name) {
		parameters.add(pDirection.set("direction", ofVec4f(0), ofVec4f(-1), ofVec4f(1)));
		ftBaseArea::setup(4, _width, _height, _name);
		velocities.resize(pixelCount, ofVec4f(0));
		direction = ofVec4f(0);
		totalVelocity = ofVec4f(0);
	}
	
	void ftArea4f::update() {
		ftBaseArea::readPixels();
		float* floatPixelData = pixels.getData();
		
		// calculate magnitudes
		totalVelocity = ofVec4f(0);
		totalMagnitude = 0;
		highMagnitude = 0;
		
		for (int i=0; i<pixelCount; i++) {
			ofVec4f *velocity = &velocities[i];
			velocity->x = floatPixelData[i*4];
			velocity->y = floatPixelData[i*4+1];
			velocity->z = floatPixelData[i*4+2];
			velocity->w = floatPixelData[i*4+3];
			totalVelocity += *velocity;
			
			magnitudes[i] = velocity->length();
			highMagnitude = max(highMagnitude, magnitudes[i]);
		}
		
		getMeanStDev(magnitudes, meanMagnitude, stdevMagnitude);
		direction = totalVelocity.normalize();
		pDirection.set(direction);
		pMeanMagnitude.set(meanMagnitude);
		pTotalMagnitude.set(ofToString(totalMagnitude));
		pStdevMagnitude.set(ofToString(stdevMagnitude));
	}
}
