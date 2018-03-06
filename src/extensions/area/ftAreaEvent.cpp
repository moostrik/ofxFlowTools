#include "ftAreaEvent.h"

namespace flowTools {
	
	void ftAreaEvent::setup(int _width, int _height, string _name) {
		ftNormalizedArea::setup(_width, _height, _name);
				
		parameters.add(pTreshold.set("event threshold", 2, 0, 100));
		parameters.add(pBaseFactor.set("event base", 0, 0, 3));
	}
	
	void ftAreaEvent::update(ofTexture& _texture) {
		ftNormalizedArea::update(_texture);
				
		for (int i=0; i<numChannels; i++) {
			event[i] = 0;
			float nV = normalizedVelocity[i];
			if (!eventActive[i]) {
				if (nV < inActiveLow[i]) { inActiveLow[i] = nV; }
				
				if (nV > inActiveLow [i]+ pTreshold.get() && nV > normalizedMagnitude * pBaseFactor.get()) {
					event[i] = 1;
					eventActive[i] = true;
					activeHigh[i] = nV;
				}
			}
			
			if (eventActive[i]) {
				if (nV > activeHigh[i]) { activeHigh[i] = nV; }
				
				if (nV < activeHigh[i] * pTreshold.get()) {
					event[i] = -1;
					eventActive[i] = false;
					inActiveLow[i] = nV;
				}
			}
		}
	}
	
	void ftAreaEvent::allocate(int _width, int _height, int _numChannels) {
		ftNormalizedArea::allocate(_width, _height, _numChannels);
		
		event.clear();
		event.resize(numChannels, 0);
		activeHigh.clear();
		activeHigh.resize(numChannels, 0);
		inActiveLow.clear();
		inActiveLow.resize(numChannels, 0);
		eventActive.clear();
		eventActive.resize(numChannels, 0);
	}
}
