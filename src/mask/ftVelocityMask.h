#pragma once

#include "ofMain.h"
#include "ftFbo.h"
#include "ftVelocityMaskShader.h"
#include "ftContrastShader.h"
#include "ftEOGShader.h"
#include "ftHSLShader.h"
#include "ftGaussianBlurShader.h"
#include "ftLuminanceShader.h"

namespace flowTools {
	
	class ftVelocityMask {
	public:
		
		void	setup(int _width, int _height);
		void	setDensity(ofTexture& tex);
		void	setVelocity(ofTexture& tex);
		void	update();
		
		ofTexture& getTexture() {return getColorMask();};
		ofTexture& getColorMask() {return colorMaskSwapBuffer.getBackTexture();};
		ofTexture& getLuminanceMask() {return luminanceMaskFbo.getTexture();};
		void	draw(int _x, int _y) {draw(_x, _y, width, height);}
		void	draw(int _x, int _y, int _width, int _height) {colorMaskSwapBuffer.getBackTexture().draw(_x, _y, _width, _height);};
		
		int		getWidth() {return width;};
		int		getHeight(){return height;};
		
		int		getBlurPasses() {return blurPasses.get();}
		float	getBlurRadius() {return blurRadius.get();}
		void	setBlurPasses(int value) {blurPasses.set(value);}
		void	setBlurRadius(float value) {blurRadius.set(value);}
		
		ofParameterGroup	parameters;
		
	protected:
		ofParameter<float>	strength;
		ofParameter<float>	saturation;
		ofParameter<int>	blurPasses;
		ofParameter<float>	blurRadius;
		
		int					width;
		int					height;
		ofTexture*			densityTexture;
		bool				bDensityTextureSet;
		ofTexture*			velocityTexture;
		bool				bVelocityTextureSet;
		ftSwapBuffer		colorMaskSwapBuffer;
		ftFbo				luminanceMaskFbo;
		ftVelocityMaskShader VelocityMaskShader;
		
		ftEOGShader			EOGShader;
		ftHSLShader			HSLShader;
		ftContrastShader	contrastShader;
		ftGaussianBlurShader gaussianBlurShader;
		ftLuminanceShader	luminanceShader;
		
	};
}

