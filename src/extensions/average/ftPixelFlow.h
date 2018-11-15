
#pragma once

#include "ofMain.h"
#include "ftUtil.h"
#include "ftFlow.h"

namespace flowTools {
	
	class ftPixelFlow : public ftFlow {
	public:
		void setup(int _width, int _height, ftFlowForceType _type) {
			type = _type;
			GLint internalFormat = ftUtil::getInternalFormatFromType(type);
			ftFlow::allocate(_width, _height, internalFormat);
			numChannels = ftUtil::getNumChannelsFromInternalFormat(internalFormat);
			
			floatPixels.allocate(inputWidth, inputHeight, numChannels);
			roi = ofRectangle(0,0,1,1);
			
			parameters.setName("pixels");
			roiParameters.setName("region of interest");
			pRoi.resize(4);
			roiParameters.add(pRoi[0].set("x", 0, 0, 1));
			roiParameters.add(pRoi[1].set("y", 0, 0, 1));
			roiParameters.add(pRoi[2].set("width", 1, 0, 1));
			roiParameters.add(pRoi[3].set("height", 1, 0, 1));
			for (int i=0; i<4; i++) {
				pRoi[i].addListener(this, &ftPixelFlow::pRoiListener);
			}
			parameters.add(roiParameters);
		}
		
		void update() {
			ftUtil::roi(outputFbo.get(), inputFbo.getTexture(), roi);
			ftUtil::toPixels(outputFbo.get(), floatPixels);
		}
		
		ofFloatPixels& getPixels() { return floatPixels; }
		
		void setRoi(float _x, float _y, float _width, float _height) { setRoi(ofRectangle(_x, _y, _width, _height)); }
		void setRoi(ofRectangle _rect) {
			float x = ofClamp(_rect.x, 0, 1);
			float y = ofClamp(_rect.y, 0, 1);
			float maxW = 1.0 - x;
			float maxH = 1.0 - y;
			float w = min(_rect.width, maxW);
			float h = min(_rect.height, maxH);
			
			roi = ofRectangle(x, y, w, h);
			
			if (pRoi[0] != x) { pRoi[0].set(x); }
			if (pRoi[1] != y) { pRoi[1].set(y); }
			if (pRoi[2].getMax() != maxW) { pRoi[2].setMax(maxW); pRoi[2].set(w); }
			if (pRoi[3].getMax() != maxH) { pRoi[3].setMax(maxH); pRoi[3].set(h); }
			if (pRoi[2] != w) { pRoi[2].set(w); }
			if (pRoi[3] != h) { pRoi[3].set(h); }
		}
		
		ofRectangle	getRoi()		{ return roi; }
		ofPoint		getRoiCentre()	{ return ofPoint(roi.x + roi.width / 2, roi.y + roi.height / 2); }
		
		
	protected:
		ftFlowForceType		type;
		int 				numChannels;
		ofFloatPixels		floatPixels;
		
		ofParameterGroup 					roiParameters;
		ofRectangle							roi;
		vector< ofParameter<float> >		pRoi;
		void pRoiListener(float& _value)	{ setRoi(pRoi[0], pRoi[1], pRoi[2], pRoi[3]); }
		
	};
}
