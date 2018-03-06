#include "ftAreaEvent.h"

namespace flowTools {
	
	void ftAreaEvent::setup(int _width, int _height, string _name) {
		ftAreaNormalized::setup(_width, _height, _name);
		
		eventParameters.setName("events");
		eventParameters.add(pTreshold.set("event threshold", .25, .1, .3));
		eventParameters.add(pBaseFactor.set("event base", .6, .5, .75));
		pEvent.resize(4);
		for (int i=0; i<4; i++) {
			eventParameters.add(pEvent[i].set("event " + ofToString(i), 0, -1, 1));
		}
		parameters.add(eventParameters);
		pTreshold.addListener(this, &ftAreaEvent::pFloatListener);
		pBaseFactor.addListener(this, &ftAreaEvent::pFloatListener);
	}
	
	void ftAreaEvent::update(ofTexture& _texture) {
		ftAreaNormalized::update(_texture);
		if (!bAllocated) { return; }
		if (event.size() != numChannels) {
			event.clear();
			event.resize(numChannels, 0);
			activeHigh.clear();
			activeHigh.resize(numChannels, 0);
			inActiveLow.clear();
			inActiveLow.resize(numChannels, 0);
			eventActive.clear();
			eventActive.resize(numChannels, 0);
		}
				
		for (int i=0; i<numChannels; i++) {
			event[i] = 0;
			float nV = fabs(normalizedVelocity[i]);
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
		
		for (int i=0; i<numChannels; i++) {
			pEvent[i] = event[i];
		}
		
		for (int i=numChannels; i<pVelocity.size(); i++) {
			pEvent[i] = 0;
		}
	}
}
