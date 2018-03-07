#include "ftAreaEvent.h"

namespace flowTools {
	
	void ftAreaEvent::setup(int _width, int _height, string _name) {
		ftAreaNormalized::setup(_width, _height, _name);
		
		eventParameters.setName("events");
		eventParameters.add(pTreshold.set("threshold", .25, .1, .3));
		eventParameters.add(pBase.set("base", .6, .5, .75));
		eventParameters.add(pMagnitudeEvent.set("magnitude", 0, -1, 1));
		pVelocityEvent.resize(4);
		for (int i=0; i<4; i++) {
			eventParameters.add(pVelocityEvent[i].set("velocity " + ofToString(i), false));
		}
		parameters.add(eventParameters);
		pTreshold.addListener(this, &ftAreaEvent::pFloatListener);
		pBase.addListener(this, &ftAreaEvent::pFloatListener);
	}
	
	void ftAreaEvent::update(ofTexture& _texture) {
		ftAreaNormalized::update(_texture);
		if (!bAllocated) { return; }
		if (event.size() != numChannels + 1) {
			event.clear();
			event.resize(numChannels + 1, 0);
			activeHigh.clear();
			activeHigh.resize(numChannels + 1, 0);
			inActiveLow.clear();
			inActiveLow.resize(numChannels + 1, 0);
			eventActive.clear();
			eventActive.resize(numChannels + 1, 0);
		}
				
		for (int i=0; i<numChannels + 1; i++) {
			event[i] = 0;
			float eV;
			if (i==0) {eV = normalizedMagnitude; }
			else { eV = fabs(normalizedVelocity[i - 1]); }
			
			if (!eventActive[i]) {
				if (eV < inActiveLow[i]) { inActiveLow[i] = eV; }
				
				if (eV > inActiveLow [i] + pTreshold.get() && eV > normalizedMagnitude * pBase.get()) {
					event[i] = 1;
					eventActive[i] = true;
					activeHigh[i] = eV;
				}
			}
			
			if (eventActive[i]) {
				if (eV > activeHigh[i]) { activeHigh[i] = eV; }
				
				if (eV < activeHigh[i] * pTreshold.get()) {
					event[i] = -1;
					eventActive[i] = false;
					inActiveLow[i] = eV;
				}
			}
			
			if (i==0) { pMagnitudeEvent = (eventActive[i])? true : false; }
			else { pVelocityEvent[i-1] = (eventActive[i])? true : false; }
		}
		
		
	}
}
