
#include "ftArea1f.h"

namespace flowTools {
	
	void ftArea1f::setup(int _width, int _height, string _name) {
		ftBaseArea::setup(1, _width, _height, _name);
		
		
		
		
	}
	
	void ftArea1f::update() {
		ftBaseArea::readPixels();
		float* floatPixelData = pixels.getData();
		
		// calculate magnitudes
		
		totalMagnitude = 0;
		highMagnitude = 0;
		
		for (int i=0; i<pixelCount; i++) {
			magnitudes[i] = floatPixelData[i];
			
			
			
			
			
			
			
			highMagnitude = max(highMagnitude, magnitudes[i]);
		}
		
		getMeanStDev(magnitudes, meanMagnitude, stdevMagnitude);
		
		
		pMeanMagnitude.set(meanMagnitude);
		pTotalMagnitude.set(ofToString(totalMagnitude));
		pStdevMagnitude.set(ofToString(stdevMagnitude));
	}
}
