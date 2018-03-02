
#pragma once

#include "ofMain.h"
#include "ftUtil.h"

namespace flowTools {
	
	class ftBaseArea {
	public:
		ftBaseArea() { ; }
		virtual	~ftBaseArea() { ; }
		
		void			setRoi(float _x, float _y, float _width, float _height) { pRoiX.set(_x); pRoiY.set(_y); pRoiWidth.set(_width); pRoiHeight.set(_height); roi = ofRectangle(pRoiX, pRoiY, pRoiWidth, pRoiHeight); }
		void			setRoi(ofRectangle _rect) { setRoi(_rect.x, _rect.y, _rect.width, _rect.height); }
		
		void			setTexture(ofTexture& _texture);
		void 			setMask(ofTexture& _texture);
	
		ofTexture&		getTexture() 			{ return scaleFbo.getTexture(); }
		
		float			getTotalMagnitude()		{ return totalMagnitude; }
		float			getAverageMagnitude()	{ return getMeanMagnitude(); }
		float			getMeanMagnitude()		{ return meanMagnitude; }
		float			getSt_devMagnitude()	{ return stdevMagnitude; }
		float			getHighMagnitude()		{ return highMagnitude; }
		
		vector<float>& 	getMagnitudes()			{ return magnitudes; }
		
		int				getSize()				{ return pixelCount; }
		
		ofRectangle		getRoi()				{ return roi; }
		ofPoint			getCentre()				{ return ofPoint(roi.x + roi.width / 2, roi.y + roi.height / 2); }
		
		ofParameterGroup 	parameters;
		ofParameterGroup 	roiParameters;
		
	protected:
		
		void	setup(int _numchannels, int _width = 32, int _height = 32, string _name = "");
		void	update();
		
		ofParameter<string>		pTotalMagnitude;
		ofParameter<float>		pMeanMagnitude;
		ofParameter<string>		pStdevMagnitude;
		
		ofParameter<float>		pRoiX;
		void pRoiXListener(float& _value) { pRoiWidth.setMax(1 - _value); if (pRoiWidth.get() > pRoiWidth.getMax()) { pRoiWidth.set(pRoiWidth.getMax());} else { pRoiWidth.set(pRoiWidth.get());} } // last else for update
		ofParameter<float>		pRoiY;
		void pRoiYListener(float& _value) { pRoiHeight.setMax(1 - _value); if (pRoiHeight.get() > pRoiHeight.getMax()) { pRoiHeight.set(pRoiHeight.getMax());}  else { pRoiHeight.set(pRoiHeight.get());} }
		ofParameter<float>		pRoiWidth;
		ofParameter<float>		pRoiHeight;
		
		ofFbo					scaleFbo;
		ofFloatPixels 			pixels;
		
		ofRectangle 			roi;
		
		vector<float>	magnitudes;
		float			totalMagnitude;
		float			meanMagnitude;
		float			stdevMagnitude;
		float			highMagnitude;
		
		int width;
		int height;
		int	pixelCount;
		
		int numChannels;
		int internalFormat;
		int internalReadFormat;
		
		void readPixels();
		
		void getMeanStDev(vector<float> &_v, float &_mean, float &_stDev);
	};
}
