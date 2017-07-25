
#pragma once

#include "ofMain.h"
#include "ftFbo.h"
#include "ftAreaAverage.h"

namespace flowTools {
	
	class ftAreaAverage4f : public ftAreaAverage {
	public:
		ftAreaAverage4f(){ ; }
		virtual	~ftAreaAverage4f() { ; }
		
		void		setup(int _scaleFactor = 1, string _name = "") ;
		void		update();
		
		ofVec4f		getDirection()			{ return direction; }
		ofVec4f		getTotalVelocity()		{ return totalVelocity; }
		vector<ofVec4f>& getVelocities()	{ return velocities; }
		
	private:
		ofVec4f					direction;
		ofVec4f					totalVelocity;
		ofParameter<ofVec4f>	pDirection;
		vector<ofVec4f>			velocities;
		
		void allocate(int _width, int _height) ;

		void pRoiXListener(float& _value)		{ ftAreaAverage::pRoiXListener(_value); }
		void pRoiYListener(float& _value)		{ ftAreaAverage::pRoiYListener(_value); }
		void pScaleFactorListener(float& _value){ ftAreaAverage::pScaleFactorListener(_value); }
	};
}