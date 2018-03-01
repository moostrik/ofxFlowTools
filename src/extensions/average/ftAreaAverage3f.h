
#pragma once

#include "ofMain.h"
#include "ftAreaAverage.h"

namespace flowTools {
	
	class ftAreaAverage3f : public ftAreaAverage {
	public:
		ftAreaAverage3f(){ ; }
		virtual	~ftAreaAverage3f() { ; }
		
		void		setup(int _width = 32, int _height = 32, string _name = "");
		void		update();
		
		ofVec3f		getDirection()			{ return direction; }
		ofVec3f		getTotalVelocity()		{ return totalVelocity; }
		vector<ofVec3f>& getVelocities()	{ return velocities; }
		
	private:
		ofVec3f					direction;
		ofVec3f					totalVelocity;
		ofParameter<ofVec3f>	pDirection;
		vector<ofVec3f>			velocities;
		
		void allocate(int _width, int _height) ;
		
		void pRoiXListener(float& _value)		{ ftAreaAverage::pRoiXListener(_value); }
		void pRoiYListener(float& _value)		{ ftAreaAverage::pRoiXListener(_value); }
	};
}
