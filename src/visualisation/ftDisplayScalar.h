
#pragma once

#include "ofMain.h"
#include "ftDisplayScalarShader.h"

namespace flowTools {
	
	class ftDisplayScalar {
	public:
		
		void	allocate(int _width, int _height){
			width = _width;
			height = _height;
			visibleBuffer.allocate(width, height);
			parameters.setName("display scalar");
			parameters.add(scale.set("scale", .25, 0, 1));
		};
		
//		void	update() {visibleFloatShader.update(visibleBuffer, *floatTexture, scale.get());};
		
		void	setSource(ofTexture& tex)	{ floatTexture = &tex; }
		void	setScale(float _value)		{ scale.set(_value); }
		
		float	getScale()	{ return scale.get(); }
		int		getWidth()	{ return width; }
		int		getHeight()	{ return height; }
		
		ofTexture& getTextureReference() {return visibleBuffer.getTextureReference();};
		void	draw(int _x, int _y) {draw(_x, _y, width, height);}
		void	draw(int _x, int _y, int _width, int _height) {
			visibleFloatShader.update(visibleBuffer, *floatTexture, scale.get());
			visibleBuffer.draw(_x, _y, _width, _height);
		};
		
		ofParameterGroup parameters;
	protected:
		ofParameter<float>	scale;
		int		width;
		int		height;
		ofFbo	visibleBuffer;
		ofTexture* floatTexture;
		ftDisplayScalarShader visibleFloatShader;
		
	};
}

