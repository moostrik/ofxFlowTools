
#pragma once

#include "ofMain.h"
#include "ftUtil.h"

namespace flowTools {
	
	class ftBaseArea {
	public:
		ftBaseArea() { ; }
		virtual	~ftBaseArea() { ; }
		
		virtual void	setup(int _width = 32, int _height = 32, string _name = "") = 0;
		virtual void	update() = 0;
		
		void			setRoi(float _x, float _y, float _width, float _height) { pRoiX.set(_x); pRoiY.set(_y); pRoiWidth.set(_width); pRoiHeight.set(_height); }
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
		
		ofRectangle		getRoi()				{ return ofRectangle(pRoiX.get(), pRoiY.get(), pRoiWidth.get(), pRoiHeight.get()); }
		
		ofParameterGroup parameters;
		
	protected:
		ofParameter<string>		pTotalMagnitude;
		ofParameter<float>		pMeanMagnitude;
		ofParameter<string>		pStdevMagnitude;
		
		virtual void			allocate(int _width, int _height) = 0;
		
		ofParameterGroup		roiParameters;
		ofParameter<float>		pRoiX;
		void pRoiXListener(float& _value) { pRoiWidth.setMax(1 - _value); if (pRoiWidth.get() > pRoiWidth.getMax()) { pRoiWidth.set(pRoiWidth.getMax());} else { pRoiWidth.set(pRoiWidth.get());} } // last else for update
		ofParameter<float>		pRoiY;
		void pRoiYListener(float& _value) { pRoiHeight.setMax(1 - _value); if (pRoiHeight.get() > pRoiHeight.getMax()) { pRoiHeight.set(pRoiHeight.getMax());}  else { pRoiHeight.set(pRoiHeight.get());} }
		ofParameter<float>		pRoiWidth;
		//		void pRoiWidthListener(float& _value) { pRoiX.setMax(1 - _value); if (pRoiX.get() > pRoiX.getMax()) { pRoiX.set(pRoiX.getMax());} }
		ofParameter<float>		pRoiHeight;
		//		void pRoiHeightListener(float& _value) { pRoiY.setMax(1 - _value); if (pRoiY.get() > pRoiY.getMax()) { pRoiY.set(pRoiY.getMax());} }
		
		ofFbo					scaleFbo;
		ofFloatPixels 			pixels;
		
		vector<float>	magnitudes;
		float			totalMagnitude;
		float			meanMagnitude;
		float			stdevMagnitude;
		float			highMagnitude;
		
		int width;
		int height;
		int	pixelCount;
		
		int internalformat;
	};
}
