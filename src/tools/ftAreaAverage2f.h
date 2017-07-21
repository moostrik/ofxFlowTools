
#pragma once

#include "ofMain.h"
#include "ftFbo.h"

namespace flowTools {
	
	class ftAreaAverage2f {
	public:
		ftAreaAverage2f() { ; }
		virtual	~ftAreaAverage2f() { ; }
		
		void		setup(int _width, int _height, string _name = "") ;
		
		void		setSize(int _width, int _height) ;
//		void		setRoi(int _x, int _y, int _width, int _height) { pRoiX.set(_x); pRoiY.set(_y); pRoiWidth.set(_width); pRoiHeight.set(_height); }
//		void		setRoi(ofRectangle _rect) { setRoi(_rect.x, _rect.y, _rect.width, _rect.height); }
		
		void		setTexture(ofTexture _texture);
		void		update();
		void		update(ofTexture _texture) { setTexture(_texture); update(); }
		
		ofTexture&		getTexture() { return scaleFbo.getTexture(); }
		
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
		
	protected:
		ofParameter<string>		pTotalMagnitude;
		ofParameter<string>		pMeanMagnitude;
		ofParameter<string>		pStdevMagnitude;
		
		ofParameterGroup		roiParameters;
		ofParameter<float>		pRoiX;
		void pRoiXListener(float& _value) { pRoiWidth.setMax(1 - _value); if (pRoiWidth.get() > pRoiWidth.getMax()) { pRoiWidth.set(pRoiWidth.getMax());} else { pRoiWidth.set(pRoiWidth.get());} } // last else for update
		ofParameter<float>		pRoiY;
		void pRoiYListener(float& _value) { pRoiHeight.setMax(1 - _value); if (pRoiHeight.get() > pRoiHeight.getMax()) { pRoiHeight.set(pRoiHeight.getMax());}  else { pRoiHeight.set(pRoiHeight.get());} }
		ofParameter<float>		pRoiWidth;
//		void pRoiWidthListener(float& _value) { pRoiX.setMax(1 - _value); if (pRoiX.get() > pRoiX.getMax()) { pRoiX.set(pRoiX.getMax());} }
		ofParameter<float>		pRoiHeight;
//		void pRoiHeightListener(float& _value) { pRoiY.setMax(1 - _value); if (pRoiY.get() > pRoiY.getMax()) { pRoiY.set(pRoiY.getMax());} }
		
		ftFbo		scaleFbo;
		ofFloatPixels pixels;
		ofMesh		quad;
		
		vector<float>	magnitudes;
		float		totalMagnitude;
		float		meanMagnitude;
		float		stdevMagnitude;
		float		highMagnitude;
		
		int width;
		int	height;
		int	pixelCount;
		
	private:
		ofParameter<ofVec2f>	pDirection;
		vector<ofVec2f>	velocities;
		
		ofVec2f		direction;
		ofVec2f		totalVelocity;
	};
}