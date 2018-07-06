
#pragma once

#include "ofMain.h"
#include "ftAreaFlow.h"

namespace flowTools {
	
	class ftAreaFlowNormalized: public ftAreaFlow {
	public:
		
		void	setup(int _width, int _height, ftFlowForceType _type) override  {
			ftAreaFlow::setup(_width, _height, _type);
			parameters.add(pNormalization.set("normalization", .25, .01, 1));
			parameters.add(pComponentBoost.set("boost highest comp.", 0, 0, 5));
		}
		
		void	update() override  {
			ftAreaFlow::update();
			
			if (meanMagnitude > 0) {
				meanMagnitude = meanMagnitude / pNormalization.get();
				meanMagnitude = ofClamp(meanMagnitude, 0, 1);
				
				for (int i=0; i<numChannels; i++) {
					velocity[i] = direction[i] * meanMagnitude;
				}
				
				if (pComponentBoost.get() > 0 && numChannels > 1) {
					// normalize to highest component and apply boost
					float highVelocity = 0;
					float P = 1;
					for (int i=0; i<numChannels; i++) {
						if (fabs(velocity[i]) > highVelocity) {
							highVelocity = fabs(velocity[i]);
							if (velocity[i] < 0) P = -1;
						}
					}
					for (int i=0; i<numChannels; i++) {
						velocity[i] /= highVelocity;
						velocity[i] = powf(fabs(velocity[i]), pComponentBoost.get()) * P;
						velocity[i] *= highVelocity;
					}
				}
			}
			for (int i=0; i<numChannels; i++) {
				pComponents[i] = velocity[i];
			}
			pMeanMagnitude.set(meanMagnitude);
		}
		
		float	getNormalization()				{ return pNormalization.get(); }
		float	getComponentBoost()				{ return pComponentBoost.get(); }
		
		float	setNormalization(float _value)	{ pNormalization.set(_value); }
		void	setComponentBoost(float _value)	{ pComponentBoost.set(_value); }
		
	protected:
		
		ofParameter<float>		pNormalization;
		ofParameter<float>		pComponentBoost;
		
	};
}
