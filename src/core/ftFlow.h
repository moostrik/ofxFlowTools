
#pragma once

#include "ofMain.h"
#include "ftUtil.h"
#include "ftPingPongFbo.h"
#include "ftAddMultipliedShader.h"
#include "ftVisualizationField.h"

namespace flowTools {
	
	class ftFlow {
	public:		
		virtual void setInput(ofTexture &_tex)							{ set(inputFbo, _tex); }
		virtual void addInput(ofTexture &_tex, float _strength = 1.0)	{ add(inputFbo, _tex, _strength); }
		
		virtual ofTexture& getInput()	{ return inputFbo.getTexture(); }
		virtual ofTexture& getOutput()	{ return outputFbo.getTexture(); }
	
		virtual void reset() 			{ resetInput(); resetOutput(); }
		virtual void resetInput()		{ ftUtil::zero(inputFbo); }
		virtual void resetOutput()		{ ftUtil::zero(outputFbo); }
		
		void resize(int _width, int _height) { resize(_width, _height, _width, _height); }
		void resize(int _inputWidth, int _inputHeight, int _outputWidth, int _outputHeight);
		
		int		getInputWidth()			{ return inputWidth; }
		int		getInputHeight()		{ return inputHeight; }
		GLint	getInputInternalFormat(){ return inputInternalFormat; }
		int		getOutputWidth()		{ return outputWidth; }
		int		getOutputHeight()		{ return outputHeight; }
		GLint	getOutputInternalFormat(){return outputInternalFormat; }
		
		void draw(int _x, int _y, int _w, int _h)		{ drawOutput(_x, _y, _w, _h); }
		virtual void drawInput(int _x, int _y, int _w, int _h) 	{ visualizationField.draw(inputFbo.getTexture(), _x, _y, _w, _h); }
		virtual void drawOutput(int _x, int _y, int _w, int _h) { visualizationField.draw(outputFbo.getTexture(), _x, _y, _w, _h); }
		
		virtual void setVisualizationScale(float _value)		{ visualizationField.setScale(_value); }
		virtual void setVisualizationFieldSize(int _w, int _h)	{ visualizationField.setupField(_w, _h); }
		virtual void setVisualizationFieldSize(glm::vec2 _value){ setVisualizationFieldSize(_value.x, _value.y); }
		virtual void setVisualizationToggleScalar(bool _value)	{ visualizationField.setToggleScalar(_value); }
		
		ofParameterGroup&	getParameters() 	{ return parameters; }
		
	protected:
		
		ofParameterGroup	parameters;
		
		ftPingPongFbo		inputFbo;
		ftPingPongFbo		outputFbo;
		
		int					inputWidth, inputHeight, outputWidth, outputHeight;
		GLint				inputInternalFormat, outputInternalFormat;
		
		ftVisualizationField	visualizationField;
		
		virtual void allocate(int _width, int _height, GLint _internalFormat)  { allocate(_width, _height, _internalFormat, _width, _height, _internalFormat); }
		virtual void allocate(int _inputWidth, int _inputHeight, GLint _inputInternalFormat, int _outputWidth, int _outputHeight, GLint _outputInternalFormat);
		
		void set(ftPingPongFbo &_fbo, ofTexture &_inputTex);
		void add(ftPingPongFbo &_dstFbo, ofTexture &_srcTex, float _strength = 1.0);
		ftAddMultipliedShader	addMultipliedShader;
	};
	
}




