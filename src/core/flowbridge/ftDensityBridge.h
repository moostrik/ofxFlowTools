#pragma once

#include "ofMain.h"
#include "ftFbo.h"
#include "ftDensityBridgeShader.h"
#include "ftContrastShader.h"
#include "ftEOGShader.h"
#include "ftHSLShader.h"
#include "ftGaussianBlurShader.h"
#include "ftLuminanceShader.h"

namespace flowTools {
	
	class ftDensityBridge {
	public:
		
		void	setup(int _width, int _height);
		void	setDensity(ofTexture& tex);
		void	setVelocity(ofTexture& tex);
		void	update();
		
		ofTexture& getTexture() {return getColorMask();};
		ofTexture& getColorMask() {return colorMaskSwapBuffer.getTexture();};
		ofTexture& getLuminanceMask() {return luminanceMaskFbo.getTexture();};
		void	draw(int _x, int _y, int _width, int _height, ofBlendMode _blendmode = OF_BLENDMODE_ALPHA);
		
		int		getWidth() {return width;};
		int		getHeight(){return height;};
		
		float	getBlurRadius() {return blurRadius.get();}
		void	setBlurRadius(float value) {blurRadius.set(value);}
		
		ofParameterGroup	parameters;
		
	protected:
		ofParameter<float>	power;
		ofParameter<float>	cutOff;
		ofParameter<float>	saturation;
		ofParameter<float>	hue;
		ofParameter<float>	blurRadius;
		
		int					width;
		int					height;
		
		ofTexture*			densityTexture;
		bool				bDensityTextureSet;
		ofTexture*			velocityTexture;
		bool				bVelocityTextureSet;
		ftSwapBuffer		colorMaskSwapBuffer;
		ftFbo				luminanceMaskFbo;
		ftDensityBridgeShader densityBridgeShader;
		
		ftEOGShader			EOGShader;
		ftHSLShader			HSLShader;
		ftContrastShader	contrastShader;
		ftGaussianBlurShader gaussianBlurShader;
		ftLuminanceShader	luminanceShader;
		
	};
}

