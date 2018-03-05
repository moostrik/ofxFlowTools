
#pragma once

#include "ofMain.h"
#include "ftUtil.h"

namespace flowTools {
	
	class ftArea {
	public:
		ftArea() { ; }
		virtual	~ftArea() { ; }
		
		void			setup(int _width = 32, int _height = 32, string _name = "");
		void			update(ofTexture& _texture);
		
		void			setWidth(int _value)	{ allocate(_value, height); }
		void			setHeight(int _value)	{ allocate(width, _value); }

		void			setRoi(float _x, float _y, float _width, float _height) { setRoi(ofRectangle(_x, _y, _width, _height)); }
		void			setRoi(ofRectangle _rect);

		int				getNumChannels()		{ return numChannels; }

		float			getDirection(int _index){ if (_index < numChannels) { return direction[_index]; } else { return 0; } }
		float			getVelocity(int _index) { if (_index < numChannels) { return velocity[_index];  } else { return 0; } }
		float			getAverageMagnitude()	{ return getMeanMagnitude(); }
		float			getMeanMagnitude()		{ return meanMagnitude; }
		float			getStDevMagnitude()		{ return stdevMagnitude; }
//
		int				getWidth()				{ return width; }
		int				getHeight()				{ return height; }

		ofRectangle		getRoi()				{ return roi; }
		ofPoint			getRoiCentre()			{ return ofPoint(roi.x + roi.width / 2, roi.y + roi.height / 2); }
		
		ofParameterGroup 	parameters;
		ofParameterGroup 	roiParameters;
	protected:
		
		int 			width;
		int 			height;
//		int				numPixels;
		int 			numChannels;
		int 			internalFormat;
		bool			bAllocated;
		
		ofFbo			scaleFbo;
		ofFloatPixels 	pixels;
		ofRectangle 	roi;
		
		float			meanMagnitude;
		float			stdevMagnitude;
		vector<float>	magnitudes;
		
		vector<float> 	direction;
		vector<float> 	velocity;
		
			//		void allocate(ofTexture& _tex);
		void allocate(int _width, int _height) { allocate(_width, _height, numChannels);}
		void allocate(int _width, int _height, int _numChannels);
		
		void getMeanStDev(vector<float> &_v, float &_mean, float &_stDev);
		
		ofParameter<float>		pMeanMagnitude;
		ofParameter<float>		pStdevMagnitude;
		vector< ofParameter<float> > pVelocity;
		void pFloatListener(float& _value) { _value = int(_value * 100) / 100.0; }
		
		vector< ofParameter<float> > pRoi;
		void pRoiListener(float& _value) { setRoi(pRoi[0], pRoi[1], pRoi[2], pRoi[3]); }
	};
}
