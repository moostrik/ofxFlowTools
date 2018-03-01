
#pragma once

#include "ofMain.h"
#include "ftBaseArea.h"

namespace flowTools {
	
	class ftArea4f : public ftBaseArea {
	public:
		ftArea4f(){ ; }
		virtual	~ftArea4f() { ; }
		
		void		setup(int _width = 32, int _height = 32, string _name = "") ;
		void		update();
		
		ofVec4f		getDirection()			{ return direction; }
		ofVec4f		getTotalVelocity()		{ return totalVelocity; }
		vector<ofVec4f>& getVelocities()	{ return velocities; }
		
	private:
		ofVec4f					direction;
		ofVec4f					totalVelocity;
		ofParameter<ofVec4f>	pDirection;
		vector<ofVec4f>			velocities;
		
		void allocate(int _width, int _height);

		void pRoiXListener(float& _value)		{ ftBaseArea::pRoiXListener(_value); }
		void pRoiYListener(float& _value)		{ ftBaseArea::pRoiYListener(_value); }
	};
}
