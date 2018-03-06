
#pragma once

#include "ofMain.h"
#include "ftArea.h"

namespace flowTools {
	
	class ftNormalizedArea: public ftArea {
	public:
		ftNormalizedArea() { ; }
		virtual	~ftNormalizedArea() { ; }
		
		void	setup(int _width = 32, int _height = 32, string _name = "");
		void	update(ofTexture& _texture);
				
		float 	getNormalizedMagnitude()	{ return normalizedMagnitude; }
		float	getNormalizedVelocity(int _index) { if (_index < numChannels) { return normalizedVelocity[_index];  } else { return 0; } }
		
		float	getNormalization()		{ return pNormalization.get(); }
		float	getComponentBoost()		{ return pComponentBoost.get(); }
		
		float	setNormalization(float _value)	{ pNormalization.set(_value); }
		void	setComponentBoost(float _value)	{ pComponentBoost.set(_value); }
		
	protected:
		
		ofParameter<float>		pNormalization;
		ofParameter<float>		pComponentBoost;
		
		float 					normalizedMagnitude;
		vector<float> 			normalizedVelocity;
		
		void allocate(int _width, int _height, int _numChannels);
		
	};
}
