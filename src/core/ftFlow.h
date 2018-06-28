
#pragma once

#include "ofMain.h"
#include "ftUtil.h"
#include "ftSwapFbo.h"
#include "ftAddMultipliedShader.h"
#include "ftDisplayScalar.h"
#include "ftDisplayField.h"

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
			internalFormat = _internalFormat;
			width = _width;
			height = _height;
			inputFbo.allocate(width, height, internalFormat);
			outputFbo.allocate(width, height, internalFormat);
			displayScalar.setup(width, height);
			displayField.setup(width / 2, height / 2);
			drawField = false;
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
		
		ofTexture&	getInput()		{ return inputFbo.getTexture(); }
		bool		getInputSet() 	{ return bInputSet; }
		
		ofParameterGroup&	getParameters() 	{ return parameters; }
		
		void drawOutput(int _x, int _y, int _w, int _h, ofBlendMode _blendMode = OF_BLENDMODE_ALPHA) {
			draw(_x, _y, _w, _h, _blendMode);
		}
		
		void draw(int _x, int _y, int _w, int _h, ofBlendMode _blendMode = OF_BLENDMODE_ALPHA) {
			ofPushStyle();
			ofEnableBlendMode(_blendMode);
			if (internalFormat == GL_R32F || internalFormat == GL_RG32F) {
				if (drawField) { displayField.draw(outputFbo.getTexture(), _x, _y, _w, _h); }
				else { displayScalar.draw(outputFbo.getTexture(), _x, _y, _w, _h); }
			} else { outputFbo.getTexture().draw(_x, _y, _w, _h); }
			ofPopStyle();
		}
		
		void drawInput(int _x, int _y, int _w, int _h, ofBlendMode _blendMode = OF_BLENDMODE_ALPHA) {
			ofPushStyle();
			ofEnableBlendMode(_blendMode);
			if (internalFormat == GL_R32F || internalFormat == GL_RG32F) {
				if (drawField) { displayField.draw(inputFbo.getTexture(), _x, _y, _w, _h); }
				else { displayScalar.draw(inputFbo.getTexture(), _x, _y, _w, _h); }
			} else { inputFbo.getTexture().draw(_x, _y, _w, _h); }
			ofPopStyle();
		}
		
		void setFieldSize(int _w, int _h) {
			displayField.setup(_w, _h); 
		}
		
		bool setDrawField(bool _value) { drawField = _value; }
		void setDrawScale(float _scale) {
			if (internalFormat == GL_R32F || internalFormat == GL_RG32F) {
				displayScalar.setScale(_scale);
				displayField.setScale(_scale);
			}
		}
		
	protected:
		
		ofParameterGroup	parameters;
		int			width;
		int			height;
		int 		internalFormat;
		int			numChannels;
		bool		isFloat;
		bool		drawField;
		ftSwapFbo 	inputFbo;
		bool		bInputSet;
		
		ftSwapFbo	outputFbo;
		ftAddMultipliedShader	AddMultipliedShader;
		
		
		ftDisplayScalar		displayScalar;
		ftDisplayField		displayField;
	};
	
}



