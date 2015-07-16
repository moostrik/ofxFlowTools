
#pragma once

#include "ofMain.h"
#include "ftFbo.h"

using namespace flowTools;

class ftAverageVelocity {
public:
	ftAverageVelocity() { ; }
	virtual	~ftAverageVelocity() { ; }

	void		setup(int _width, int _height, string _name = "none") ;
	
	void		setSize(int _width, int _height) ;
	
	void		setTexture(ofTexture& _textureReference);
	void		update();
	
	ofVec2f		getDirection() { return direction; }
	float		getAmplitude()  { return amplitude; }
	float		getArea()	{ return area; }
	
	ofParameterGroup parameters;
	
private:
	ofParameter<int>	pWidth;
	ofParameter<int>	pHeight;
	ofParameter<ofVec2f> pDirection;
	ofParameter<float>	pAmplitudeMultiplier;
	ofParameter<float>	pAmplitude;
	ofParameter<float>	pAreaTreshold;
	ofParameter<float>	pArea;
	
	void pWidthListner(int& _width) { setSize(_width, averageFbo.getHeight()); }
	void pHeightListner(int& _height) { setSize(averageFbo.getHeight(), _height); }
	
	ftFbo		averageFbo;
	float*		averageFloats;
	
	ofVec2f		direction;
	float		amplitude;
	float		area;
	
	
};