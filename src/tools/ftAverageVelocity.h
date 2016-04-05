
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
	
	ofVec2f		getDirection() { return direction; }
	float		getMagnitude()	{ return totalMagnitude; }
	float		getAverageMagnitude()	{ return averageMagnitude; }
	float		getHighMagnitude()	{ return highMagnitude; }
	
	ofParameterGroup parameters;
	
private:
	ofParameter<ofVec2f>	pDirection;
	ofParameter<float>		pMagnitude;
	ofParameter<float>		pAverageMagnitude;
	
//	void pWidthListner(int& _width) { setSize(_width, averageFbo.getHeight()); }
//	void pHeightListner(int& _height) { setSize(averageFbo.getHeight(), _height); }
	
	ftFbo		averageFbo;
	float*		floatPixelData;
	
	ofVec2f		direction;
	float		totalMagnitude;
	float		averageMagnitude;
	float		highMagnitude;
	
	
};
}