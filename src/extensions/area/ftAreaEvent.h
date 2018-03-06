
#pragma once

#include "ofMain.h"
#include "ftNormalizedArea.h"

namespace flowTools {
	
	class ftAreaEvent: public ftNormalizedArea {
	public:
		ftAreaEvent() { ; }
		virtual	~ftAreaEvent() { ; }
		
		void	setup(int _width = 32, int _height = 32, string _name = "");
		void	update(ofTexture& _texture);
		
		float	getEvent(int _index) { if (_index < numChannels) { return event[_index];  } else { return 0; } }
		
		float	getThreshold()			{ return pTreshold.get(); }
		float	getBasefactor()			{ return pBaseFactor.get(); }
		
		void	setThreshold(float _value)		{ pTreshold.set(_value); }
		void	setBaseFactor(float _value)		{ pBaseFactor.set(_value); }
		
	protected:
		vector<int> 	event;
		vector<float> 	activeHigh;
		vector<float> 	inActiveLow;
		vector<bool> 	eventActive;
		
		
		ofParameter<float>		pTreshold;
		ofParameter<float>		pBaseFactor;
		
		void allocate(int _width, int _height, int _numChannels);
		
	};
}
