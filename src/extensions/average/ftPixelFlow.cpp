#include "ftPixelFlow.h"


namespace flowTools {
	
	void ftPixelFlow::setup(int _width, int _height, ftFlowForceType _type) {
		type = _type;
		roi = ofRectangle(0,0,1,1);
		
		GLint internalFormat = ftUtil::getInternalFormatFromType(type);
		allocate(_width, _height, internalFormat, _width * roi.width, _height * roi.height, internalFormat);
		
		parameters.setName("pixels");
		parameters.add(pDuration.set("pixel conversion (ms)", 0, 0, 10));
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
	
	void ftPixelFlow::update() {
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		ftUtil::roi(outputFbo.get(), inputFbo.getTexture(), roi);
		float bT = ofGetElapsedTimeMillis();
		ftUtil::toPixels(outputFbo.get(), floatPixels);
		pDuration.set(ofGetElapsedTimeMillis() - bT);
		
		ofPopStyle();
	}
	
	void ftPixelFlow::setRoi(ofRectangle _rect) {
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
	
	void ftPixelFlow::allocate(int _inputWidth, int _inputHeight, GLint _inputInternalFormat, int _outputWidth, int _outputHeight, GLint _outputInternalFormat) {
		
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
}
