//
//  ftFlow.cpp
//  example_core
//
//  Created by Ties East on 28/06/2018.
//

#include "ftFlow.h"

namespace flowTools {
	
	void ftFlow::allocate(int _inputWidth, int _inputHeight, GLint _inputInternalFormat, int _outputWidth, int _outputHeight, GLint _outputInternalFormat) {
		inputWidth = _inputWidth;
		inputHeight = _inputHeight;
		inputInternalFormat = _inputInternalFormat;
		outputWidth = _outputWidth;
		outputHeight = _outputHeight;
		outputInternalFormat = _outputInternalFormat;
		
		inputFbo.allocate(inputWidth, inputHeight, inputInternalFormat);
		ftUtil::zero(inputFbo);
		
		outputFbo.allocate(outputWidth, outputHeight, outputInternalFormat);
		ftUtil::zero(outputFbo);
		
		visualizationField.setup(outputWidth, outputHeight);
	}
	
	void ftFlow::add(ftPingPongFbo &_dstFbo, ofTexture &_srcTex, float _strength) {
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		_dstFbo.swap();
		AddMultipliedShader.update(_dstFbo, _dstFbo.getBackTexture(), _srcTex, 1.0, _strength);
		ofPopStyle();
	}
	
	void ftFlow::set(ftPingPongFbo &_dstFbo, ofTexture &_srcTex) {
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		ftUtil::zero(_dstFbo);
		ftUtil::stretch(_dstFbo, _srcTex);
		ofPopStyle();
	}
};

