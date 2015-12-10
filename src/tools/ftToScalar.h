
#pragma once

#include "ofMain.h"
#include "ftToScalarShader.h"

namespace flowTools {
	
	class ftToScalar {
	public:
		
		void	allocate(int _width, int _height){
			width = _width;
			height = _height;
			ScalarBuffer.allocate(width, height, GL_RGB32F);
			parameters.setName("to scalar");
			parameters.add(scale.set("scale", 1, 0.5, 10));
		};
		
		void	setSource(ofTexture& tex) {normalTexture = &tex;};
		void	update() {visibleFloatShader.update(visibleBuffer, *normalTexture, scale.get());};
		
		int		getWidth() {return width;};
		int		getHeight(){return height;};
		
		ofTexture& getTexture() {return visibleBuffer.getTexture();};
		void	draw(int _x, int _y) {draw(_x, _y, width, height);}
		void	draw(int _x, int _y, int _width, int _height) {visibleBuffer.draw(_x, _y, _width, _height);};
		
		ofParameterGroup parameters;
	protected:
		ofParameter<float>	scale;
		int		width;
		int		height;
		ofFbo	visibleBuffer;
		ofTexture* normalTexture;
		ftToScalarShader toScalarShader;
		
	};
}

