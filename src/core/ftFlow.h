
#pragma once

#include "ofMain.h"
#include "ftUtil.h"
#include "ftSwapFbo.h"
#include "ftAddMultipliedShader.h"
#include "ftDisplayScalar.h"
#include "ftVelocityField.h"

namespace flowTools {
	
	class ftFlow {
	public:
		ftFlow() {
			ofAddListener(ofEvents().update, this, &ftFlow::internalUpdate);
		}

		virtual ~ftFlow() {;}

		void internalUpdate(ofEventArgs &_args) {
			;
		}

		void allocate(int _width, int _height, int _internalFormat) {
			width = _width;
			height = _height;
			internalFormat = _internalFormat;
			inputFbo.allocate(width, height, internalFormat);
			outputFbo.allocate(width, height, internalFormat);
			displayScalar.setup(width, height);
			displayField.setup(width / 4, height / 4);
			doScalar = true;
			doField = true;
//			numChannels = ftUtil::getNumChannelsFromInternalFormat(internalFormat);
//			isFloat = ftUtil::isFloat(internalFormat);
			bInputSet = false;
		}
		
		void reset() { resetInput(); resetOutput(); }
		void resetInput() { ftUtil::zero(inputFbo); bInputSet = false; }
		void resetOutput() { ftUtil::zero(outputFbo); }

		void setInput(ofTexture &_inputTex) {
			ftUtil::zero(inputFbo);
			ftUtil::stretch(inputFbo, _inputTex);
			bInputSet = true;
		}

		void addInput(ofTexture &_inputTex, float _strength = 1.0) {
			inputFbo.swap();
			AddMultipliedShader.update(inputFbo, inputFbo.getBackTexture(), _inputTex, 1.0, _strength);
			bInputSet = true;
		}
		
		void setOutput(ofTexture &_inputTex) {
			ftUtil::zero(outputFbo);
			ftUtil::stretch(outputFbo, _inputTex);
			bInputSet = true;
		}
		
		void addOutput(ofTexture &_inputTex, float _strength = 1.0) {
			outputFbo.swap();
			AddMultipliedShader.update(outputFbo, outputFbo.getBackTexture(), _inputTex, 1.0, _strength);
			bInputSet = true;
		}

		ofTexture&	getOutput()		{ return outputFbo.getTexture(); }
		bool		getInputSet() 	{ return bInputSet; }
		
		ofTexture&	getInput()		{ return inputFbo.getTexture(); }

		ofParameterGroup&	getParameters() 	{ return parameters; }
		
		void draw(int _x, int _y, int _w, int _h, ofBlendMode _blendMode = OF_BLENDMODE_ALPHA) {
			if (doScalar) { drawScalar(_x, _y, _w, _h, _blendMode); }
			if (doField)  { drawField(_x, _y, _w, _h, _blendMode); }
		}
		
	protected:
		
		void drawScalar(int _x, int _y, int _w, int _h, ofBlendMode _blendMode = OF_BLENDMODE_ALPHA) {
			ofPushStyle();
			ofEnableBlendMode(_blendMode);
			if (ftUtil::isFloat(internalFormat)) {
				displayScalar.draw(outputFbo.getTexture(), _x, _y, _w, _h);
			} else {
				outputFbo.draw(_x, _y, _w, _h);
			}
			ofPopStyle();
		}
		void drawField(int _x, int _y, int _w, int _h, ofBlendMode _blendMode = OF_BLENDMODE_ALPHA){
			if (ftUtil::isFloat(internalFormat)) {
				ofPushStyle();
				ofEnableBlendMode(_blendMode);
				displayField.draw(outputFbo.getTexture(), _x, _y, _w, _h);
				ofPopStyle();
			}
		}
		
		ofParameterGroup	parameters;
		int			width;
		int			height;
		int 		internalFormat;
		int			numChannels;
		bool		isFloat;
		
		ftSwapFbo 	inputFbo;
		bool		bInputSet;
		
		ftSwapFbo	outputFbo;
		ftAddMultipliedShader	AddMultipliedShader;
		
		
		ftDisplayScalar		displayScalar;
		bool				doScalar;
		ftVelocityField		displayField;
		bool				doField;
	};
	
}

