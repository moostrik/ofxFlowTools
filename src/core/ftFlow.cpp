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
		addMultipliedShader.update(_dstFbo, _dstFbo.getBackTexture(), _srcTex, 1.0, _strength);
		ofPopStyle();
	}
	
	void ftFlow::set(ftPingPongFbo &_dstFbo, ofTexture &_srcTex) {
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		ftUtil::zero(_dstFbo);
		ftUtil::stretch(_dstFbo, _srcTex);
		ofPopStyle();
	}
	
	void ftFlow::setParameters(ofParameterGroup &_parameters) {
		if (!checkParameters(parameters, _parameters)) {
			ofLogWarning("ftFlow::setParameters") << "parameters \"" << parameters.getName() << "\" and \"" << _parameters.getName() << "\" are not the same";
		}
		else {
			setParameters(_parameters, parameters);
		}
	}
	
	bool ftFlow::checkParameters(ofParameterGroup& _sourceParameters, ofParameterGroup& _destinationParameters) {
		if (_sourceParameters.size() != _destinationParameters.size()) { return false; }
		for (int i=0; i<_sourceParameters.size(); i++) {
			if (_sourceParameters.getName(i) != _destinationParameters.getName(i)) { return false; }
			if (_sourceParameters.getType(i) != _destinationParameters.getType(i)) { return false; }
			if (_sourceParameters.getType(i) == typeid(ofParameterGroup).name()) {
				if (!checkParameters(_sourceParameters.getGroup(i), _destinationParameters.getGroup(i))) { return false; }
			}
		}
		return true;
	}
	
	void ftFlow::setParameters(ofParameterGroup& _sourceParameters, ofParameterGroup& _destinationParameters) {
		for (int i=0; i<_sourceParameters.size(); i++) {
			if (_sourceParameters.getType(i) == typeid(ofParameterGroup).name()) {
				setParameters(_sourceParameters.getGroup(i), _destinationParameters.getGroup(i));
			}
			else {
				// set each parameter
			};
		}
	}
};

