
#pragma once

#include "ofMain.h"
#include "ftUtil.h"
#include "ftVelocityField.h"
#include "ftTemperatureField.h"

namespace flowTools {
	
	class ftDisplayField {
	public:
		
		void setup(int _width, int _height){
			width = _width;
			height = _height;
			
			c1Field.setup( _width, _height);
			c2Field.setup( _width, _height);
			
			parameters.setName("display scalar");
			parameters.add(isActive.set("active", true));
			parameters.add(scale.set("scale", 1, 0, 10));
		};
		
		void	setActive(bool _value)		{ isActive.set(_value); }
		void	setScale(float _value)		{ c1Field.setScale( _value); c2Field.setScale( _value); }
		
		int		getWidth()	{ return width; }
		int		getHeight()	{ return height; }
		bool	getActive()	{ return isActive.get(); }
		float	getScale()	{ return scale.get(); }
		
		void	draw(ofTexture _srcTex, int _x, int _y, int _width, int _height, ofBlendMode _blendmode = OF_BLENDMODE_ALPHA) {
			if (isActive.get()) {
				ofPushStyle();
				int numChannels = ftUtil::getNumChannelsFromInternalFormat(_srcTex.getTextureData().glInternalFormat);
				ofEnableBlendMode(_blendmode);
				if (numChannels == 1) {
					c1Field.draw(_srcTex, _x, _y, _width, _height, _blendmode);
				} else {
					c2Field.draw(_srcTex, _x, _y, _width, _height, _blendmode);
				}
				ofPopStyle();
			}
		};
		
		ofParameterGroup parameters;
	protected:
		ofParameter<bool> 	isActive;
		ofParameter<float>	scale;
		int					width;
		int					height;
		ftVelocityField		c2Field;
		ftTemperatureField	c1Field;
		
	};
}

