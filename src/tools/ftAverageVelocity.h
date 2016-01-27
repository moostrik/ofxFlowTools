
#pragma once

#include "ofMain.h"
#include "ftFbo.h"

namespace flowTools {

class ftAverageVelocity {
public:
	ftAverageVelocity() { ; }
	virtual	~ftAverageVelocity() { ; }

	void		setup(int _width, int _height, string _name = "none") ;
	
	void		setSize(int _width, int _height) ;
	
	void		setTexture(ofTexture& _textureReference);
	void		update();
	
	ofVec2f		getDirection() { return averageDirection; }
	float		getMagnitude()	{ return averageMagnitude; }
	
	ofParameterGroup parameters;
	
private:
	ofParameter<float>		pMagnitude;
	ofParameter<ofVec2f>	pDirection;
	
//	void pWidthListner(int& _width) { setSize(_width, averageFbo.getHeight()); }
//	void pHeightListner(int& _height) { setSize(averageFbo.getHeight(), _height); }
	
	ftFbo		averageFbo;
	float*		floatPixelData;
	
	ofVec2f		averageDirection;
	float		averageMagnitude;
	
	
};
}