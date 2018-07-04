
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
		
		virtual void setInput(ofTexture &_tex)							{ set(inputFbo, _tex); bInputSet = true; }
		virtual void addInput(ofTexture &_tex, float _strength = 1.0)	{ add(inputFbo, _tex, _strength); bInputSet = true; }
		virtual bool getInputSet()		{ return bInputSet; }
		
		virtual void setOutput(ofTexture &_tex)							{ set(outputFbo, _tex); bInputSet = true; }
		virtual void addOutput(ofTexture &_tex, float _strength = 1.0)	{ add(outputFbo, _tex, _strength); bInputSet = true; }
		
		virtual ofTexture& getInput()	{ return inputFbo.getTexture(); }
		virtual ofTexture& getOutput()	{ return outputFbo.getTexture(); }
	
		virtual void reset() 			{ resetInput(); resetOutput(); }
		virtual void resetInput()		{ ftUtil::zero(inputFbo); bInputSet = false; }
		virtual void resetOutput()		{ ftUtil::zero(outputFbo); }
		
		virtual void draw(int _x, int _y, int _w, int _h)	{ drawOutput(_x, _y, _w, _h); }
		virtual void drawInput(int _x, int _y, int _w, int _h);
		virtual void drawOutput(int _x, int _y, int _w, int _h);
		
		void setFieldSize(int _w, int _h)	{ visualizeField.setup(_w, _h); }
		void setFieldSize(int _size) 		{ setFieldSize(_size, (float)_size / (float)outputWidth * outputHeight); }
		void setVisualizationScale(float _scale);
		bool toggleVisualizationField(bool _value)		{ toggleVisualisationField = _value; }
		
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
		
		void allocate(int _width, int _height, GLint _internalFormat)  { allocate(_width, _height, _internalFormat, _width, _height, _internalFormat); }
		void allocate(int _inputWidth, int _inputHeight, GLint _inputInternalFormat, int _outputWidth, int _outputHeight, GLint _outputInternalFormat);
		
		void set(ftPingPongFbo &_fbo, ofTexture &_inputTex);
		void add(ftPingPongFbo &_dstFbo, ofTexture &_srcTex, float _strength = 1.0);
		ftAddMultipliedShader	AddMultipliedShader;
		

	};
	
}




