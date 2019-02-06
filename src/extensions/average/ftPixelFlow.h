
#pragma once

#include "ofMain.h"
#include "ftUtil.h"
#include "ftFlow.h"

namespace flowTools {
	
	class ftPixelFlow : public ftFlow {
	public:
		void setup(int _width, int _height, ftFlowForceType _type) {
			type = _type;
			roi = ofRectangle(0,0,1,1);
			
			GLint internalFormat = ftUtil::getInternalFormatFromType(type);
			allocate(_width, _height, internalFormat, _width * roi.width, _height * roi.height, internalFormat);
			
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
			ofPushStyle();
			ofEnableBlendMode(OF_BLENDMODE_DISABLED);
			ftUtil::roi(outputFbo.get(), inputFbo.getTexture(), roi);
			ftUtil::toPixels(outputFbo.get(), floatPixels);
			ofPopStyle();
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
			allocate(inputWidth, inputHeight, inputInternalFormat, inputWidth * w, inputHeight * h, inputInternalFormat);
			
			if (pRoi[0] != x) { pRoi[0].set(x); }
			if (pRoi[1] != y) { pRoi[1].set(y); }
			if (pRoi[2].getMax() != maxW) { pRoi[2].setMax(maxW); pRoi[2].set(w); }
			if (pRoi[3].getMax() != maxH) { pRoi[3].setMax(maxH); pRoi[3].set(h); }
			if (pRoi[2] != w) { pRoi[2].set(w); }
			if (pRoi[3] != h) { pRoi[3].set(h); }
		}
		
		ofRectangle	getRoi()		{ return roi; }
		ofPoint		getRoiCentre()	{ return ofPoint(roi.x + roi.width / 2, roi.y + roi.height / 2); }
		
		int getWidth()	{ return inputWidth; }
		int getHeight()	{ return inputHeight; }
		
		void resize(int _inputWidth, int _inputHeight, int _outputWidth, int _outputHeight) override {
			resize(_inputWidth, _inputHeight);
		}
		void resize(int _width, int _height) override {
			allocate(_width, _height, inputInternalFormat, _width * roi.width, _height * roi.height, outputInternalFormat);
		}
		
		
	protected:
		ftFlowForceType		type;
		int 				numChannels;
		ofFloatPixels		floatPixels;
		
		ofParameterGroup 					roiParameters;
		ofRectangle							roi;
		vector< ofParameter<float> >		pRoi;
		void pRoiListener(float& _value)	{ setRoi(pRoi[0], pRoi[1], pRoi[2], pRoi[3]); }
		
		void allocate(int _inputWidth, int _inputHeight, GLint _inputInternalFormat, int _outputWidth, int _outputHeight, GLint _outputInternalFormat) override {
			
			if (inputWidth != _inputWidth || inputHeight != _inputHeight || inputInternalFormat != _inputInternalFormat) {
				inputWidth = _inputWidth;
				inputHeight = _inputHeight;
				inputInternalFormat = _inputInternalFormat;
				inputFbo.allocate(inputWidth, inputHeight, inputInternalFormat);
				ftUtil::zero(inputFbo);
			}
			
			if (_outputWidth != outputWidth || _outputHeight != outputHeight || _outputInternalFormat != outputInternalFormat) {
				numChannels = ftUtil::getNumChannelsFromInternalFormat(_inputInternalFormat);
				floatPixels.allocate(_outputWidth, _outputHeight, numChannels);
				
				outputWidth = _outputWidth;
				outputHeight = _outputHeight;
				outputInternalFormat = _outputInternalFormat;
				outputFbo.allocate(outputWidth, outputHeight, outputInternalFormat);
				ftUtil::zero(outputFbo);
			}
		}
		
	};
}
