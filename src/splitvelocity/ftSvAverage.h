
#pragma once

#include "ofMain.h"
#include "ftFbo.h"

namespace flowTools {
	
	class ftSvAverage {
	public:
		ftSvAverage() { ; }
		virtual	~ftSvAverage() { ; }
		
		void		setup(int _width, int _height, string _name = "none") ;
		
		
		void		setTexture(ofTexture _texture);
		void		update();
		void		update(ofTexture _texture) { setTexture(_texture); update(); }
		
		float		getTotalMagnitude()		{ return totalMagnitude; }
		float		getAverageMagnitude()	{ return getMeanMagnitude(); }
		float		getMeanMagnitude()		{ return meanMagnitude; }
		float		getSt_devMagnitude()	{ return stdevMagnitude; }
		
		vector<float>& getMagnitudes()		{ return magnitudes; }
		
		ofVec4f		getDirection()			{ return direction; }
		ofVec4f		getTotalVelocity()		{ return totalVelocity; }
		vector<ofVec4f>& getVelocities()	{ return velocities; }
		
		int			getSize()				{ return pixelCount; }
		
		ofParameterGroup parameters;
		
	private:
		ofParameter<ofVec4f>	pDirection;
		ofParameter<string>		pTotalMagnitude;
		ofParameter<string>		pMeanMagnitude;
		ofParameter<string>		pStdevMagnitude;
		
		ftFbo		scaleFbo;
		ofFloatPixels pixels;
		
		
		vector<float>	magnitudes;
		vector<ofVec4f>	velocities;
		
		ofVec4f		direction;
		ofVec4f		totalVelocity;
		float		totalMagnitude;
		float		meanMagnitude;
		float		stdevMagnitude;
		
		int width;
		int	height;
		int	pixelCount;
		
		
	};
}