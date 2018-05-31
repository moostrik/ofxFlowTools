
#pragma once

#include "ofMain.h"
#include "ftSvFromVelocityShader.h"

namespace flowTools {
	
	class ftSvFromVelocity {
	public:
		
		void	setup(int _width, int _height){
			width = _width;
			height = _height;
			splitVelocityBuffer.allocate(width, height, GL_RGBA32F);
			ftUtil::zero(splitVelocityBuffer);
		};
		
		void	setSource(ofTexture& tex)	{ floatTexture = &tex; } // this is ambigue
		void	update(ofTexture& tex) { setSource(tex); update(); }
		void	update() {
			ofPushStyle();
			ofEnableBlendMode(OF_BLENDMODE_DISABLED);
			ftUtil::zero(splitVelocityBuffer);
			svFromVelocityShader.update(splitVelocityBuffer, *floatTexture, 1.0);
			ofPopStyle();
		}
		
		int		getWidth()	{ return width; }
		int		getHeight()	{ return height; }
		
		ofTexture& getTexture() {return splitVelocityBuffer.getTexture();}
		
	protected:
		int		width;
		int		height;
		ofFbo	splitVelocityBuffer;
		ofTexture* floatTexture;
		ftSvFromVelocityShader svFromVelocityShader;
		
	};
}

