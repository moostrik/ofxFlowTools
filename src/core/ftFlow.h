
#pragma once

#include "ofMain.h"
#include "ftUtil.h"
#include "ftPingPongFbo.h"
#include "ftAddMultipliedShader.h"
#include "ftDisplayScalar.h"
#include "ftDisplayField.h"

namespace flowTools {
	
	class ftFlow {
	public:
		
		void allocate(int _width, int _height, GLint _internalFormat)  { allocate(_width, _height, _internalFormat, _width, _height, _internalFormat); }
		void allocate(int _inputWidth, int _inputHeight, GLint _inputInternalFormat, int _outputWidth, int _outputHeight, GLint _outputInternalFormat);
		
		void setInput(ofTexture &_inputTex);
		void addInput(ofTexture &_inputTex, float _strength = 1.0);
		bool getInputSet()		{ return bInputSet; }
		
		void setOutput(ofTexture &_inputTex);
		void addOutput(ofTexture &_inputTex, float _strength = 1.0);
		
		ofTexture& getOutput()	{ return outputFbo.getTexture(); }
		ofTexture& getInput()	{ return inputFbo.getTexture(); }
	
		void reset() 			{ resetInput(); resetOutput(); }
		void resetInput()		{ ftUtil::zero(inputFbo); bInputSet = false; }
		void resetOutput()		{ ftUtil::zero(outputFbo); }
		
		void draw(int _x, int _y, int _w, int _h)	{ drawOutput(_x, _y, _w, _h); }
		void drawInput(int _x, int _y, int _w, int _h);
		void drawOutput(int _x, int _y, int _w, int _h);
		
		void setFieldSize(int _w, int _h)	{ visualizeField.setup(_w, _h); }
		void setFieldSize(int _size) 		{ setFieldSize(_size, (float)_size / (float)outputWidth * outputHeight); }
		bool setDrawField(bool _value)		{ toggleVisualisationField = _value; }
		void setDrawScale(float _scale);
		
		ofParameterGroup&	getParameters() 	{ return parameters; }
		
	protected:
		
		ofParameterGroup	parameters;
		
		ftPingPongFbo		inputFbo;
		bool				bInputSet;
		ftPingPongFbo		outputFbo;
		
		int					inputWidth, inputHeight, outputWidth, outputHeight;
		GLint				inputInternalFormat, outputInternalFormat;
		
		ftDisplayScalar		visualizeScalar;
		ftDisplayField		visualizeField;
		bool				toggleVisualisationField;
		
		ftAddMultipliedShader	AddMultipliedShader;

	};
	
}




