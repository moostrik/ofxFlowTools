
#pragma once

#include "ofMain.h"
#include "ftUtil.h"
#include "ftFlow.h"

namespace flowTools {
	
	class ftPixelFlow : public ftFlow {
	public:
		void	setup(int _width, int _height, ftFlowForceType _type) {
			type = _type;
			GLint internalFormat = ftUtil::getInternalFormatFromType(type);
			ftFlow::allocate(_width, _height, internalFormat);
			numChannels = ftUtil::getNumChannelsFromInternalFormat(internalFormat);
			
			floatPixels.allocate(inputWidth, inputHeight, numChannels);
			roi = ofRectangle(0,0,1,1);
			
			parameters.setName("pixelFlow");
		}
		
		void	update() {
			ftUtil::roi(outputFbo, inputFbo, roi);
			ftUtil::toPixels(inputFbo, floatPixels);
		}
		
		ofFloatPixels& getPixels() { return floatPixels; }
		
		void		setRoi(float _x, float _y, float _width, float _height) { setRoi(ofRectangle(_x, _y, _width, _height)); }
		void		setRoi(ofRectangle _rect) {
			float x = ofClamp(_rect.x, 0, 1);
			float y = ofClamp(_rect.y, 0, 1);
			float maxW = 1.0 - x;
			float maxH = 1.0 - y;
			float w = min(_rect.width, maxW);
			float h = min(_rect.height, maxH);
			
			roi = ofRectangle(x, y, w, h);
			
			if (pRoi[0] != x) { pRoi[0].setWithoutEventNotifications(x); }
			if (pRoi[1] != y) { pRoi[1].setWithoutEventNotifications(y); }
			if (pRoi[2].getMax() != maxW) { pRoi[2].setMax(maxW); pRoi[2].setWithoutEventNotifications(w); }
			if (pRoi[3].getMax() != maxH) { pRoi[3].setMax(maxH); pRoi[3].setWithoutEventNotifications(h); }
			if (pRoi[2] != w) { pRoi[2].setWithoutEventNotifications(w); }
			if (pRoi[3] != h) { pRoi[3].setWithoutEventNotifications(h); }
		}
		
		ofRectangle	getRoi()			{ return roi; }
		ofPoint		getRoiCentre()		{ return ofPoint(roi.x + roi.width / 2, roi.y + roi.height / 2); }
		
		
	protected:
		
		ofFloatPixels	floatPixels;
		ofRectangle		roi;
		
		ftFlowForceType	type;
		int 			numChannels;
		
		vector< ofParameter<float> >		pRoi;
		void pRoiListener(float& _value)	{ setRoi(pRoi[0], pRoi[1], pRoi[2], pRoi[3]); }
		
	};
}
