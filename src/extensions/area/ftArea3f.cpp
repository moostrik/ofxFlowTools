
#include "ftArea3f.h"

namespace flowTools {
	
	void ftArea3f::setup(int _width, int _height, string _name) {
		parameters.add(pDirection.set("direction", ofVec3f(0), ofVec3f(-1), ofVec3f(1)));
		ftBaseArea::setup(3, _width, _height, _name);
		velocities.resize(pixelCount, ofVec3f(0));
		direction = ofVec3f(0);
		totalVelocity = ofVec3f(0);
	}
	
	void ftArea3f::update() {
		ftBaseArea::readPixels();
		float* floatPixelData = pixels.getData();
		
		// calculate magnitudes
		totalVelocity = ofVec3f(0);
		totalMagnitude = 0;
		highMagnitude = 0;
		
		for (int i=0; i<pixelCount; i++) {
			ofVec3f *velocity = &velocities[i];
			velocity->x = floatPixelData[i*3];
			velocity->y = floatPixelData[i*3+1];
			velocity->z = floatPixelData[i*3+2];
			
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
