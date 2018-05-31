#pragma once

#include "ofMain.h"
#include "ftDensityBridgeShader.h"
#include "ftVelocityBridgeShader.h"
#include "ftContrastShader.h"
#include "ftEOGShader.h"
#include "ftHSLShader.h"
#include "ftGaussianBlurShader.h"
#include "ftLuminanceShader.h"
#include "ftUtil.h"
#include "ftMultiplyForceShader.h"


namespace flowTools {
	
	class ftDensityBridge {
	public:
		
		void	setup(int _flowWidth, int _flowHeight, int _densityWidth = 0, int _densityHeight = 0);
		void	setDensity(ofTexture& tex);
		void	setVelocity(ofTexture& tex);
		void	update(float _deltaTime);
		void	draw(int _x, int _y, int _width, int _height, ofBlendMode _blendmode = OF_BLENDMODE_ALPHA);
		
		ofTexture& getTexture() 				{ return densitySwapBuffer.getTexture(); };
		ofTexture& getLuminanceMask()			{ return luminanceMaskFbo.getTexture(); };
		
		float	getTrailWeight()				{ return trailWeight.get(); }
		float	getBlurRadius()					{ return blurRadius.get(); }
		float	getSaturation()					{ return saturation.get(); }
		float	getSpeed()						{ return speed.get(); }
		
		void	setTrailWeight(float value)		{ trailWeight.set(value); }
		void	setBlurRadius(float value)		{ blurRadius.set(value); }
		void	setSaturation(float value)		{ saturation.set(value); }
		void	setSpeed(float value)			{ speed.set(value); }
		
		ofParameterGroup&	getParameters() 	{ return parameters; }
	protected:
		ofParameterGroup		parameters;
		ofParameter<float>		blurRadius;
		ofParameter<float>		trailWeight;
		ofParameter<float>		saturation;
//		ofParameter<float>		hue;
		ofParameter<float>		speed;
		
		ofTexture*				velocityTexture;
		bool					bVelocityTextureSet;
		ftSwapBuffer			velocitySwapBuffer;
		ftVelocityBridgeShader	velocityBridgeShader;
		ofTexture*				densityTexture;
		bool					bDensityTextureSet;
		ftSwapBuffer			densitySwapBuffer;
		ftDensityBridgeShader 	densityBridgeShader;
		ofFbo					luminanceMaskFbo;
		ftMultiplyForceShader	multiplyShader;
		ofFbo					drawFbo;
		
		ftHSLShader				HSLShader;
		ftGaussianBlurShader 	blurShader;
		ftLuminanceShader		luminanceShader;
	};
}

