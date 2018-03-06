#include "ftNormalizedArea.h"

namespace flowTools {
	
	void ftNormalizedArea::setup(int _width, int _height, string _name) {
		ftArea::setup(_width, _height, _name);
		
		parameters.add(pNormalization.set("normalization", 2, 0, 100));
		parameters.add(pComponentBoost.set("boost high component", 0, 0, 3));
	}
	
	void ftNormalizedArea::update(ofTexture& _texture) {
		ftArea::update(_texture);
		
		if (meanMagnitude > 0) {
			normalizedMagnitude = meanMagnitude / pNormalization.get();
			normalizedMagnitude = ofClamp(normalizedMagnitude, 0, 1);
			
			for (int i=0; i<numChannels; i++) {
				normalizedVelocity[i] = direction[i] * normalizedMagnitude;
			}
			
			if (pComponentBoost.get() > 0 && numChannels > 1) {
				// normalize to highest component and apply boost
				float highVelocity = 0;
				for (int i=0; i<numChannels; i++) {
					highVelocity = max(highVelocity, normalizedVelocity[i]);
				}
				for (int i=0; i<numChannels; i++) {
					normalizedVelocity[i] /= highVelocity;
					normalizedVelocity[i] = powf(normalizedVelocity[i], pComponentBoost.get());
					normalizedVelocity[i] *= highVelocity;
				}
			}
		}
		else {
			for (int i=0; i<numChannels; i++) {
				normalizedVelocity[i] = 0;
			}
		}
	}
	
	void ftNormalizedArea::allocate(int _width, int _height, int _numChannels) {
		ftArea::allocate(_width, _height, _numChannels);
		
		normalizedVelocity.clear();
		normalizedVelocity.resize(numChannels, 0);
	}
}
