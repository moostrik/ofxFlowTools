
#pragma once

#include "ofMain.h"
#include "ftUtil.h"
#include "ftDisplayScalarShader.h"

namespace flowTools {
	
	class ftDisplayScalar {
	public:
		
		void setup(int _width, int _height){
			width = _width;
			height = _height;
			displayScalarFbo.allocate(width, height);
			ftUtil::zero(displayScalarFbo);
			parameters.setName("display scalar");
			parameters.add(isActive.set("active", true));
			parameters.add(scale.set("scale", 1, 0, 10));
		};
		
		void	setActive(bool _value)		{ isActive.set(_value); }
		void	setScale(float _value)		{ scale.set(_value); }
		
		int		getWidth()	{ return width; }
		int		getHeight()	{ return height; }
		bool	getActive()	{ return isActive.get(); }
		float	getScale()	{ return scale.get(); }
		
		ofTexture& getTexture() {return displayScalarFbo.getTexture();}
		
		void	draw(ofTexture _srcTex, int _x, int _y, int _width, int _height, ofBlendMode _blendmode = OF_BLENDMODE_ALPHA) {
			if (isActive.get()) {
				ftUtil::zero(displayScalarFbo);
				displayScalarShader.update(displayScalarFbo, _srcTex, scale.get());
				
				ofPushStyle();
				ofEnableBlendMode(_blendmode);
				displayScalarFbo.draw(_x, _y, _width, _height);
				ofPopStyle();
			}
		};
		
		ofParameterGroup parameters;
	protected:
		ofParameter<bool> 	isActive;
		ofParameter<float>	scale;
		int		width;
		int		height;
		ofFbo	displayScalarFbo;
		ftDisplayScalarShader displayScalarShader;
		
	};
}

