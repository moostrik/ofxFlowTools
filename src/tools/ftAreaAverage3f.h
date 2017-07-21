
#pragma once

#include "ofMain.h"
#include "ftFbo.h"
#include "ftAreaAverage2f.h"

namespace flowTools {
	
	class ftAreaAverage3f : public ftAreaAverage2f {
	public:
		ftAreaAverage3f(){ ; }
		virtual	~ftAreaAverage3f() { ; }
		
		void		setup(int _width, int _height, string _name = "") ;
		
		void		setSize(int _width, int _height) ;
		
		void		setTexture(ofTexture _texture);
		void		update();
		void		update(ofTexture _texture) { setTexture(_texture); update(); }
		
		ofVec3f		getDirection()			{ return direction; }
		ofVec3f		getTotalVelocity()		{ return totalVelocity; }
		vector<ofVec3f>& getVelocities()	{ return velocities; }
		
	private:
		ofParameter<ofVec3f>	pDirection;
		vector<ofVec3f>	velocities;
		
		ofVec3f		direction;
		ofVec3f		totalVelocity;
	};
}