
#include "ftArea2f.h"

namespace flowTools {
	
	void ftArea2f::setup(int _width, int _height, string _name) {
		parameters.add(pDirection.set("direction", ofVec2f(0), ofVec2f(-1), ofVec2f(1)));
		ftBaseArea::setup(2, _width, _height, _name);
		velocities.resize(pixelCount, ofVec2f(0));
		direction = ofVec2f(0);
		totalVelocity = ofVec2f(0);
	}
	
	void ftArea2f::update() {
		ftBaseArea::readPixels();
		float* floatPixelData = pixels.getData();
		
		// calculate magnitudes
		totalVelocity = ofVec2f(0);
		totalMagnitude = 0;
		highMagnitude = 0;
		
		for (int i=0; i<pixelCount; i++) {
			ofVec2f *velocity = &velocities[i];
			velocity->x = floatPixelData[i*2];
			velocity->y = floatPixelData[i*2+1];
			
			
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
