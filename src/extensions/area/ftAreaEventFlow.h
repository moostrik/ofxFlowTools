
#pragma once

#include "ofMain.h"
#include "ftAreaFlow.h"

namespace flowTools {
	
	class ftAreaEventFlow: public ftAreaFlow {
	public:
		
		void setup(int _width, int _height, ftFlowForceType _type) override  {
			ftAreaFlow::setup(_width, _height, _type);
			
			event.clear();
			event.resize(numChannels + 1, 0);
			activeHigh.clear();
			activeHigh.resize(numChannels + 1, 0);
			inActiveLow.clear();
			inActiveLow.resize(numChannels + 1, 0);
			eventActive.clear();
			eventActive.resize(numChannels + 1, 0);
			
			eventParameters.setName("events");
			eventParameters.add(pThreshold.set("threshold", .25, .1, .3));
			eventParameters.add(pBase.set("base", .6, .5, .75));
			eventParameters.add(pMagnitudeEvent.set("magnitude", 0, -1, 1));
			pVelocityEvent.resize(4);
			for (int i=0; i<4; i++) {
				eventParameters.add(pVelocityEvent[i].set("velocity " + ofToString(i), false));
			}
			parameters.add(eventParameters);
		}
		void update() override {
			ftAreaFlow::update();
			
			for (int i=0; i<numChannels + 1; i++) {
				event[i] = 0;
				float eV;
				if (i==0) {eV = meanMagnitude; }
				else { eV = fabs(velocity[i - 1]); }
				
				if (!eventActive[i]) {
					if (eV < inActiveLow[i]) { inActiveLow[i] = eV; }
					
					if (eV > inActiveLow [i] + pThreshold.get() && eV > meanMagnitude * pBase.get()) {
						event[i] = 1;
						eventActive[i] = true;
						activeHigh[i] = eV;
					}
				}
				
				if (eventActive[i]) {
					if (eV > activeHigh[i]) {
						activeHigh[i] = eV;
					}
					if (eV < activeHigh[i] * pThreshold.get()) {
						event[i] = -1;
						eventActive[i] = false;
						inActiveLow[i] = eV;
					}
				}
				
				if (i==0) { pMagnitudeEvent = (eventActive[i])? true : false; }
				else { pVelocityEvent[i-1] = (eventActive[i])? true : false; }
			}
		}
		
		float	getMagnitudeEvent()			{ return event[0]; }
		float 	getMagnitudeEventStart()	{ return (event[0] == 1); }
		float 	getMagnitudeEventEnd()		{ return (event[0] == -1); }
		float 	getMagnitudeEventActive()	{ return eventActive[0]; }
		
		float	getVelocityEvent(int _index)		{ if (_index < numChannels) { return event[_index+1]; } else { return 0; } }
		float	getVelocityEventActive(int _index)	{ if (_index < numChannels) { return eventActive[_index+1]; } else {  return 0; } }
		float	getVelocityEventStart(int _index)	{ return getVelocityEvent(_index) == 1; }
		float	getVelocityEventEnd(int _index)		{ return getVelocityEvent(_index) == 0; }
		
		float	getThreshold()				{ return pThreshold.get(); }
		float	getBase()					{ return pBase.get(); }
		
		void	setThreshold(float _value)	{ pThreshold.set(_value); }
		void	setBase(float _value)		{ pBase.set(_value); }
		
	protected:
		ofParameterGroup	eventParameters;
		vector<int> 		event;
		vector<float> 		activeHigh;
		vector<float> 		inActiveLow;
		vector<bool> 		eventActive;
		
		ofParameter<float>	pThreshold;
		ofParameter<float>	pBase;
		ofParameter<bool> 	pMagnitudeEvent;
		vector< ofParameter<bool> > pVelocityEvent;
		
	};
}

