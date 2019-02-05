
#pragma once

#include "ofMain.h"
#include "ftFluidFlow.h"


namespace flowTools {
	
	class ftSimpleFluidFlow : public ftFluidFlow{
	public:
		ftSimpleFluidFlow() {
			parameters.clear();
			
			speed.addListener(this, &ftSimpleFluidFlow::speedListener);
			dissipation.addListener(this, &ftSimpleFluidFlow::dissipationListener);
			
			parameters.add(speed.set(			"speed"				, 0.3, 0.0, 1.0));
			parameters.add(dissipation.set(		"dissipation"		, 0.1, 0.0, 1.0));
			
			velVorticity.set(0.0);
			velViscosity.set(0.0);
			prsSpeed.set(0.0);
			prsDissipation.set(0.3);
			tmpSpeed.set(0.0);
			tmpDissipation.set(0.0);
			tmpBuoyancy.set(0.0);
			tmpWeight.set(0.0);
			tmpAmbient.set(0.0);
		}
	protected:
		
		ofParameter<float>		speed;
		ofParameter<float>		dissipation;
		
		void speedListener(float &_value)		{ setSpeed(_value); }
		void dissipationListener(float &_value)	{ setDissipation(_value); }
		
	};
}




