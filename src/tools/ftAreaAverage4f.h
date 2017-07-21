
#pragma once

#include "ofMain.h"
#include "ftFbo.h"
#include "ftAreaAverage2f.h"

namespace flowTools {
	
	class ftAreaAverage4f : public ftAreaAverage2f {
	public:
		ftAreaAverage4f(){ ; }
		virtual	~ftAreaAverage4f() { ; }
		
		void		setup(int _width, int _height, string _name = "") ;
		
		void		setSize(int _width, int _height) ;
		
		void		setTexture(ofTexture _texture);
		void		update();
		void		update(ofTexture _texture) { setTexture(_texture); update(); }
		
		ofVec4f		getDirection()			{ return direction; }
		ofVec4f		getTotalVelocity()		{ return totalVelocity; }
		vector<ofVec4f>& getVelocities()	{ return velocities; }
		
	private:
		ofParameter<ofVec4f>	pDirection;
		vector<ofVec4f>	velocities;
		
		ofVec4f		direction;
		ofVec4f		totalVelocity;
	};
}