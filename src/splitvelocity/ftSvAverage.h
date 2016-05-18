
#pragma once

#include "ofMain.h"
#include "ftFbo.h"

namespace flowTools {
	
	class ftSvAverage {
	public:
		ftSvAverage() { ; }
		virtual	~ftSvAverage() { ; }
		
		void		setup(int _width, int _height, string _name = "none") ;
		
		void		update(ofTexture _texture);
		
		float		getMagnitude()			{ return totalMagnitude; }
		float		getAverageMagnitude()	{ return averageMagnitude; }
		float		getActiveMagnitude()	{ return activeMagnitude; }
		float		getHighMagnitude()		{ return highMagnitude; }
		ofVec4f		getDirection()			{ return direction; }
		
		ofVec4f		getTotalVelocity()		{ return totalVelocity; }
		
		ofParameterGroup parameters;
		
	private:
		ofParameter<ofVec4f>	pDirection;
		ofParameter<float>		pMagnitude;
		ofParameter<float>		pAverageMagnitude;
		
		ftFbo		scaleFbo;
		ofFloatPixels pixels;
		
		float*		magnitudes;
		
		ofVec4f		direction;
		ofVec4f		totalVelocity;
		float		totalMagnitude;
		float		averageMagnitude;
		float		activeMagnitude;
		float		highMagnitude;
		
		int width;
		int	height;
		int	pixelCount;
		
		
	};
}