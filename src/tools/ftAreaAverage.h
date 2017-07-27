
#pragma once

#include "ofMain.h"
#include "ftFbo.h"

namespace flowTools {
	
	class ftAreaAverage {
	public:
		ftAreaAverage() { ; }
		virtual	~ftAreaAverage() { ; }
		
		virtual void		setup(float _scaleFactor = 1, string _name = "") = 0;
		virtual void		update() = 0;
		
//		ofVec2f		getDirection()			{ return direction; }
//		ofVec2f		getTotalVelocity()		{ return totalVelocity; }
//		vector<ofVec2f>& getVelocities()	{ return velocities; }
		
		
		void		setScaleFactor(float _value) { pScaleFactor.set(_value); }
		void		setRoi(float _x, float _y, float _width, float _height) { pRoiX.set(_x); pRoiY.set(_y); pRoiWidth.set(_width); pRoiHeight.set(_height); }
		void		setRoi(ofRectangle _rect) { setRoi(_rect.x, _rect.y, _rect.width, _rect.height); }
		
		void		setTexture(ofTexture _texture) {
			
			ofPushStyle();
			ofEnableBlendMode(OF_BLENDMODE_DISABLED);
			scaleFbo.black();
			
			scaleFactor = pScaleFactor.get();
			
			int mw = max(_texture.getWidth() * pRoiWidth.get() * scaleFactor, 16.f);
			int mh = max(_texture.getHeight() * pRoiHeight.get() * scaleFactor, 16.f);
			
			if (mw != scaleFbo.getWidth() ||
				mh != scaleFbo.getHeight()) {
				allocate(mw, mh);
			}
			
			//		scaleFbo.stretchIntoMe(_texture);
			
			int x,y,w,h;
			x = pRoiX.get() * _texture.getWidth();
			y = pRoiY.get() * _texture.getHeight();
			w = pRoiWidth.get() * _texture.getWidth();
			h = pRoiHeight.get() * _texture.getHeight();
			
			quad.setTexCoord(0, ofVec2f(x, y));
			quad.setTexCoord(1, ofVec2f(x+w, y));
			quad.setTexCoord(2, ofVec2f(x+w, y+h));
			quad.setTexCoord(3, ofVec2f(x, y+h));
			
			scaleFbo.begin();
			_texture.bind();
			quad.draw();
			_texture.unbind();
			scaleFbo.end();
			
			ofPopStyle();
		}
		
		void		update(ofTexture _texture) { setTexture(_texture); update(); }
		
		ofTexture&		getTexture() { return scaleFbo.getTexture(); }
		
		float		getTotalMagnitude()		{ return totalMagnitude; }
		float		getAverageMagnitude()	{ return getMeanMagnitude(); }
		float		getMeanMagnitude()		{ return meanMagnitude; }
		float		getSt_devMagnitude()	{ return stdevMagnitude; }
		float		getHighMagnitude()		{ return highMagnitude; }
		
		vector<float>& getMagnitudes()		{ return magnitudes; }
		
		
		int			getSize()				{ return pixelCount; }
		
		float		getScaleFactor()		{ return scaleFactor; }
		ofRectangle	getRoi()				{ return ofRectangle(pRoiX.get(), pRoiY.get(), pRoiWidth.get(), pRoiHeight.get()) ; }
		
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
		ofParameter<float>		pScaleFactor;
		void pScaleFactorListener(float& _value) { scaleFactor = _value; }
		
		ftFbo		scaleFbo;
		ofFloatPixels pixels;
		ofMesh		quad;
		
		vector<float>	magnitudes;
		float		totalMagnitude;
		float		meanMagnitude;
		float		stdevMagnitude;
		float		highMagnitude;
		
		float scaleFactor;
		int width;
		int height;
		int	pixelCount;
		
		int internalformat;
		
	private:
//		ofParameter<ofVec2f>	pDirection;
//		vector<ofVec2f>			velocities;
//		
//		ofVec2f		direction;
//		ofVec2f		totalVelocity;
	};
}