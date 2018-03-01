
#pragma once

#include "ofMain.h"
#include "ftBaseArea.h"

namespace flowTools {
	
	class ftArea3f : public ftBaseArea {
	public:
		ftArea3f(){ ; }
		virtual	~ftArea3f() { ; }
		
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
		
		void pRoiXListener(float& _value)		{ ftBaseArea::pRoiXListener(_value); }
		void pRoiYListener(float& _value)		{ ftBaseArea::pRoiXListener(_value); }
	};
}
