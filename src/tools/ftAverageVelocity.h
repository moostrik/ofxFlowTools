
#pragma once

#include "ofMain.h"
#include "ftFbo.h"

namespace flowTools {
	
	class ftAverageVelocity {
	public:
		ftAverageVelocity() { ; }
		virtual	~ftAverageVelocity() { ; }
		
		void		setup(int _width, int _height, string _name = "") ;
		
		void		setSize(int _width, int _height) ;
		
		void		setTexture(ofTexture _texture);
		void		update();
		void		update(ofTexture _texture) { setTexture(_texture); update(); }
		
		float		getTotalMagnitude()		{ return totalMagnitude; }
		float		getAverageMagnitude()	{ return getMeanMagnitude(); }
		float		getMeanMagnitude()		{ return meanMagnitude; }
		float		getSt_devMagnitude()	{ return stdevMagnitude; }
		float		getHighMagnitude()		{ return highMagnitude; }
		
		vector<float>& getMagnitudes()		{ return magnitudes; }
		
		ofVec2f		getDirection()			{ return direction; }
		ofVec2f		getTotalVelocity()		{ return totalVelocity; }
		vector<ofVec2f>& getVelocities()	{ return velocities; }
		
		int			getSize()				{ return pixelCount; }
		
		ofParameterGroup parameters;
		
	private:
		ofParameter<ofVec2f>	pDirection;
		ofParameter<string>		pTotalMagnitude;
		ofParameter<string>		pMeanMagnitude;
		ofParameter<string>		pStdevMagnitude;
		
		ftFbo		scaleFbo;
		ofFloatPixels pixels;
		
		vector<float>	magnitudes;
		vector<ofVec2f>	velocities;
		
		ofVec2f		direction;
		ofVec2f		totalVelocity;
		float		totalMagnitude;
		float		meanMagnitude;
		float		stdevMagnitude;
		float		highMagnitude;
		
		int width;
		int	height;
		int	pixelCount;
		
	};
}