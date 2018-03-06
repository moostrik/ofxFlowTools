#include "ftAreaNormalized.h"

namespace flowTools {
	
	void ftAreaNormalized::setup(int _width, int _height, string _name) {
		parameters.add(pNormalization.set("normalization", .1, 0, 1));
		parameters.add(pComponentBoost.set("boost high component", 0, 0, 3));
		pNormalization.addListener(this, &ftAreaNormalized::pFloatListener);
		pComponentBoost.addListener(this, &ftAreaNormalized::pFloatListener);
		
		ftArea::setup(_width, _height, _name);
	}
	
	void ftAreaNormalized::update(ofTexture& _texture) {
		ftArea::update(_texture);
		if (!bAllocated) { return; }
		if (normalizedVelocity.size() != numChannels) {
			normalizedVelocity.clear();
			normalizedVelocity.resize(numChannels, 0);
		}
		
		if (meanMagnitude > 0) {
			normalizedMagnitude = meanMagnitude / pNormalization.get();
			normalizedMagnitude = ofClamp(normalizedMagnitude, 0, 1);
			
			for (int i=0; i<numChannels; i++) {
				normalizedVelocity[i] = direction[i] * normalizedMagnitude;
			}
			
			if (pComponentBoost.get() > 0 && numChannels > 1) {
					// normalize to highest component and apply boost
				float highVelocity = 0;
				float P = 1;
				for (int i=0; i<numChannels; i++) {
					if (fabs(normalizedVelocity[i]) > highVelocity) {
						highVelocity = fabs(normalizedVelocity[i]);
						if (normalizedVelocity[i] < 0) P = -1;
					}
				}
				for (int i=0; i<numChannels; i++) {
					normalizedVelocity[i] /= highVelocity;
					normalizedVelocity[i] = powf(fabs(normalizedVelocity[i]), pComponentBoost.get()) * P;
					normalizedVelocity[i] *= highVelocity;
				}
			}
		}
		else {
			for (int i=0; i<numChannels; i++) {
				normalizedVelocity[i] = 0;
			}
		}
		
		for (int i=0; i<numChannels; i++) {
			pVelocity[i] = normalizedVelocity[i];
		}
		
		pMeanMagnitude.set(normalizedMagnitude);
	}
}

