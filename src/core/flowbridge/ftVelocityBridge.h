
#pragma once

#include "ofMain.h"
#include "ofxFlowTools.h"

#include "ftVelocityBridgeShader.h"
#include "ftGaussianBlurShader.h"
#include "ftMultiplyForceShader.h"

namespace flowTools {
	
	class ftVelocityBridge {
	public:
		
		ftVelocityBridge();
		void	setup(int _width, int _height);
		void	reset();
		void	update(float _deltaTime);
		void	setSource(ofTexture& _tex);
		
		ofTexture&	getTexture()				{ return multiplyFbo.getTexture(); }
		
		int		getWidth() 						{ return width; }
		int		getHeight()						{ return height; }
		
		float	getTrailWeight()				{ return trailWeight.get(); }
		float	getBlurRadius()					{ return blurRadius.get(); }
		float	getSpeed()						{ return speed.get(); }
		
		void	setTrailWeight(float value)		{ trailWeight.set(value); }
		void	setBlurRadius(float value)		{ blurRadius.set(value); }
		void	setSpeed(float value)			{ speed.set(value); }
		
		ofParameterGroup&	getParameters() 	{ return parameters; }
	protected:
		ofParameterGroup	parameters;
		ofParameter<float>	trailWeight;
		ofParameter<float>	blurRadius;
		ofParameter<float>	speed;
		
		int			width;
		int			height;
		
		ofTexture*				velocityTexture;
		bool					bSourceSet;
		ftSwapFbo				swapBuffer;
		ftVelocityBridgeShader	trailShader;
		ftGaussianBlurShader	blurShader;
		ftMultiplyForceShader	multiplyShader;
		ofFbo					multiplyFbo;
		
	};
}
