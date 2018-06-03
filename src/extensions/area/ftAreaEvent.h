
#pragma once

#include "ofMain.h"
#include "ftAreaNormalized.h"

namespace flowTools {
	
	class ftAreaEvent: public ftAreaNormalized {
	public:
		ftAreaEvent() { ; }
		virtual	~ftAreaEvent() { ; }
		
		void	setup(int _width = 32, int _height = 32, string _name = "");
		void	update(ofTexture& _srcTex);
		
		float	getMagnitudeEvent()			{ return (bAllocated)? event[0] : 0; }
		float 	getMagnitudeEventStart()	{ return (bAllocated)? (event[0] == 1) : 0; }
		float 	getMagnitudeEventEnd()		{ return (bAllocated)? (event[0] == -1) : 0; }
		float 	getMagnitudeEventActive()	{ return (bAllocated)? eventActive[0] : 0; }
		
		float	getVelocityEvent(int _index)		{ if (bAllocated && _index < numChannels) { return event[_index+1]; } else { return 0; } }
		float	getVelocityEventActive(int _index)	{ if (bAllocated && _index < numChannels) { return eventActive[_index+1]; } else {  return 0; } }
		float	getVelocityEventStart(int _index)	{ return getVelocityEvent(_index) == 1; }
		float	getVelocityEventEnd(int _index)		{ return getVelocityEvent(_index) == 0; }
		
		float	getThreshold()				{ return pTreshold.get(); }
		float	getBase()					{ return pBase.get(); }
		
		void	setThreshold(float _value)	{ pTreshold.set(_value); }
		void	setBase(float _value)		{ pBase.set(_value); }
		
	protected:
		ofParameterGroup	eventParameters;
		vector<int> 		event;
		vector<float> 		activeHigh;
		vector<float> 		inActiveLow;
		vector<bool> 		eventActive;
		
		ofParameter<float>	pTreshold;
		ofParameter<float>	pBase;
		ofParameter<bool> 	pMagnitudeEvent;
		vector< ofParameter<bool> > pVelocityEvent;
		
		void pFloatListener(float& _value) { ftArea::pFloatListener(_value); }
		
	};
}
