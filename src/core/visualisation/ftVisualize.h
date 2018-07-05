
#pragma once

#include "ofMain.h"
#include "ftUtil.h"
#include "ftDisplayScalarShader.h"

namespace flowTools {
	
	class ftVisualize {
	public:
		ftVisualize() {
			parameters.setName("visualize");
			parameters.add(pScale.set("scale", 0.3, 0.1, 3.0));
		}
		
		virtual void setup(int _width, int _height){
			scalarWidth = _width;
			scalarHeight = _height;
			displayScalarFbo.allocate(scalarWidth, scalarHeight, GL_RGBA);
		}
		
		virtual void draw(ofTexture _tex, int _x, int _y, int _width, int _height) {
			ftUtil::zero(displayScalarFbo);
			displayScalarShader.update(displayScalarFbo, _tex, pScale.get());
			displayScalarFbo.draw(_x, _y, _width, _height);
		}
		
		void 	setScale(float _value)		{ pScale.set(_value); }
		float 	getScale()					{ return pScale.get(); }
		
		int		getWidth()					{ return getScalarWidth(); }
		int		getHeight()					{ return getScalarHeight(); }
		int		getScalarWidth()			{ return scalarWidth; }
		int		getScalarHeight()			{ return scalarHeight; }
		
		ofParameterGroup&	getParameters() 	{ return parameters; }
		
	protected:
		ofParameterGroup 		parameters;
		ofParameter<float>		pScale;
		
		int						scalarWidth;
		int						scalarHeight;
		
		ofFbo					displayScalarFbo;
		ftDisplayScalarShader 	displayScalarShader;
		
	};
}

