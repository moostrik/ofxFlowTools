
#pragma once

#include "ofMain.h"
#include "ftAreaAverageFlow.h"

namespace flowTools {
	
	class ftAreaEventFlow: public ftAreaAverageFlow {
	public:
		
		void setup(int _width, int _height, ftFlowForceType _type) override  {
			ftAreaAverageFlow::setup(_width, _height, _type);

			magnitudeEvent = 0;
			magnitudeEventActive = false;
			magnitudeActiveHigh= 0;
			magnitudeInActiveLow = 0;

			componentEvents.resize(numChannels, 0);
			componentEventActives.resize(numChannels, 0);
			componentActiveHighs.resize(numChannels, 0);
			componentInActiveLows.resize(numChannels, 0);

			eventParameters.setName("events");
			eventParameters.add(pThreshold.set("threshold", .25, .1, .3));
			eventParameters.add(pBase.set("base", .6, .5, .75));
			eventParameters.add(pMagnitudeEvent.set("magnitude", 0, -1, 1));
			pComponentEvents.resize(4);
			for (int i=0; i<4; i++) {
				eventParameters.add(pComponentEvents[i].set("component " + ofToString(i), false));
			}
			parameters.add(eventParameters);
		}
		
		void update() override {
			ftAreaAverageFlow::update();
			
			float eV = meanMagnitude;
			if (!magnitudeEventActive) {
				if (eV < magnitudeInActiveLow) { magnitudeInActiveLow = eV; }

				if (eV > magnitudeInActiveLow + pThreshold.get() && eV > meanMagnitude * pBase.get()) {
					magnitudeEvent = 1;
					magnitudeEventActive = true;
					magnitudeActiveHigh = eV;
				}
			}
			if (magnitudeEventActive) {
				if (eV > magnitudeActiveHigh) {
					magnitudeActiveHigh = eV;
				}
				if (eV < magnitudeActiveHigh * pThreshold.get()) {
					magnitudeEvent = -1;
					magnitudeEventActive = false;
					magnitudeInActiveLow = eV;
				}
			}
			pMagnitudeEvent = (magnitudeEventActive)? true : false;
			
			for (int i=0; i<numChannels; i++) {
				componentEvents[i] = 0;
				eV = fabs(components[i]);
				if (!componentEventActives[i]) {
					if (eV < componentInActiveLows[i]) { componentInActiveLows[i] = eV; }

					if (eV > componentInActiveLows [i] + pThreshold.get() && eV > meanMagnitude * pBase.get()) {
						componentEvents[i] = 1;
						componentEventActives[i] = true;
						componentActiveHighs[i] = eV;
					}
				}
				if (componentEventActives[i]) {
					if (eV > componentActiveHighs[i]) {
						componentActiveHighs[i] = eV;
					}
					if (eV < componentActiveHighs[i] * pThreshold.get()) {
						componentEvents[i] = -1;
						componentEventActives[i] = false;
						componentInActiveLows[i] = eV;
					}
				}
				pComponentEvents[i] = (componentEventActives[i])? true : false;
			}
		}
		
		float	getMagnitudeEvent()			{ return magnitudeEvent; }
		float 	getMagnitudeEventActive()	{ return magnitudeEventActive; }
		float 	getMagnitudeEventStart()	{ return (magnitudeEvent == 1); }
		float 	getMagnitudeEventEnd()		{ return (magnitudeEvent == -1); }
		
		vector<int> 	getComponentEvents()		{ return componentEvents; }
		vector<float> 	getComponentActiveHighs()	{ return componentActiveHighs; }
		vector<float> 	getComponentInActiveLows()	{ return componentInActiveLows; }
		vector<bool> 	getComponentEventActives()	{ return componentEventActives; }
		
		int		getComponentEvent(int _index)		{ if (_index < numChannels) { return componentEvents[_index]; } else { return 0; } }
		float	getComponentEventActive(int _index)	{ if (_index < numChannels) { return componentEventActives[_index]; } else {  return 0; } }
		float	getComponentEventStart(int _index)	{ return getComponentEvent(_index) == 1; }
		float	getComponentEventEnd(int _index)	{ return getComponentEvent(_index) == -1; }
		
		float	getThreshold()				{ return pThreshold.get(); }
		float	getBase()					{ return pBase.get(); }
		
		void	setThreshold(float _value)	{ pThreshold.set(_value); }
		void	setBase(float _value)		{ pBase.set(_value); }
		
	protected:
		ofParameterGroup	eventParameters;
		int					magnitudeEvent;
		bool		 		magnitudeEventActive;
		float				magnitudeActiveHigh;
		float		 		magnitudeInActiveLow;
		
		vector<int> 		componentEvents;
		vector<bool> 		componentEventActives;
		vector<float> 		componentActiveHighs;
		vector<float> 		componentInActiveLows;
		
		ofParameter<float>	pThreshold;
		ofParameter<float>	pBase;
		ofParameter<bool> 	pMagnitudeEvent;
		vector< ofParameter<bool> > pComponentEvents;
		
	};
}

