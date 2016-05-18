
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
		
		float		getMagnitude()			{ return totalMagnitude; }
		float		getAverageMagnitude()	{ return averageMagnitude; }
		float		getHighMagnitude()		{ return highMagnitude; }
		float		getActiveMagnitude()	{ return activeMagnitude; } // the average of all magnitudes > 0;
		vector<float>& getMagnitudes()		{ return magnitudes; }
		
		ofVec4f		getDirection()			{ return direction; }
		ofVec4f		getTotalVelocity()		{ return totalVelocity; }
		vector<ofVec4f>& getVelocities()	{ return velocities; }
		
		int			getSize()				{ return pixelCount; }
		
		ofParameterGroup parameters;
		
	private:
		ofParameter<ofVec4f>	pDirection;
		ofParameter<float>		pMagnitude;
		ofParameter<float>		pAverageMagnitude;
		
		ftFbo		scaleFbo;
		ofFloatPixels pixels;
		
		vector<float>	magnitudes;
		vector<ofVec4f>	velocities;
		
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