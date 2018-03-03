
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
		
		void			setRoi(float _x, float _y, float _width, float _height) { pRoiX.set(_x); pRoiY.set(_y); pRoiWidth.set(_width); pRoiHeight.set(_height); roi = ofRectangle(pRoiX, pRoiY, pRoiWidth, pRoiHeight); }
		void			setRoi(ofRectangle _rect) { setRoi(_rect.x, _rect.y, _rect.width, _rect.height); }

		int				getNumChannels()		{ return numChannels; }
		
		float			getDirection(int _index){ if (_index < numChannels) { return direction[_index]; } else { return 0; } }
		float			getVelocity(int _index) 	{ if (_index < numChannels) { return velocity[_index];  } else { return 0; } }
		float			getAverageMagnitude()	{ return getMeanMagnitude(); }
		float			getMeanMagnitude()		{ return meanMagnitude; }
		float			getStDevMagnitude()		{ return stdevMagnitude; }

		ofRectangle		getRoi()				{ return roi; }
		ofPoint			getCentre()				{ return ofPoint(roi.x + roi.width / 2, roi.y + roi.height / 2); }

		ofParameterGroup 	parameters;
		ofParameterGroup 	roiParameters;
	protected:
		
		void allocate(ofTexture& _tex);
		
		ofParameter<float>		pMeanMagnitude;
		ofParameter<float>		pStdevMagnitude;
		vector< ofParameter<float> > pVelocity;
		void pFloatListener(float& _value) { _value = int(_value * 100) / 100.0; }
		
		ofParameter<float>		pRoiX;
		void pRoiXListener(float& _value) { pRoiWidth.setMax(1 - _value); if (pRoiWidth.get() > pRoiWidth.getMax()) { pRoiWidth.set(pRoiWidth.getMax());} else { pRoiWidth.set(pRoiWidth.get());} } // last else for update
		ofParameter<float>		pRoiY;
		void pRoiYListener(float& _value) { pRoiHeight.setMax(1 - _value); if (pRoiHeight.get() > pRoiHeight.getMax()) { pRoiHeight.set(pRoiHeight.getMax());}  else { pRoiHeight.set(pRoiHeight.get());} }
		ofParameter<float>		pRoiWidth;
		ofParameter<float>		pRoiHeight;
		
		int 			width;
		int 	height;
		int		numPixels;
		int 	numChannels;
		int 	internalFormat;
		bool			bAllocated;
		
		ofFbo			scaleFbo;
		ofFloatPixels 	pixels;
		ofRectangle 	roi;
		
		float			meanMagnitude;
		float			stdevMagnitude;
		vector<float>	magnitudes;
		
		vector<float> 	direction;
		vector<float> 	velocity;
		
		void getMeanStDev(vector<float> &_v, float &_mean, float &_stDev);
	};
}
