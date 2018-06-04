
#pragma once

#include "ofMain.h"
#include "ftUtil.h"
#include "ftSwapFbo.h"
#include "ftAddMultipliedShader.h"

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
			inputFbo.allocate(width, height, _internalFormat);
			outputFbo.allocate(width, height, _internalFormat);
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
		
	protected:
		ofParameterGroup	parameters;
		int			width;
		int			height;
		int 		internalFormat;
		
		ftSwapFbo 	inputFbo;
		bool		bInputSet;
		
		ftSwapFbo	outputFbo;
		ftAddMultipliedShader	AddMultipliedShader;
	};
	
}

