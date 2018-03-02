
#pragma once

#include "ofMain.h"
#include "ftBaseArea.h"

namespace flowTools {
	
	class ftArea2f : public ftBaseArea {
	public:
		ftArea2f(){ ; }
		virtual	~ftArea2f() { ; }
		
		void		setup(int _width = 32, int _height = 32, string _name = "");
		void		update();
		
		ofVec2f		getDirection()			{ return direction; }
		ofVec2f		getTotalVelocity()		{ return totalVelocity; }
		vector<ofVec2f>& getVelocities()	{ return velocities; }
		
	private:
		ofVec2f					direction;
		ofVec2f					totalVelocity;
		ofParameter<ofVec2f>	pDirection;
		vector<ofVec2f>			velocities;
	};
}
