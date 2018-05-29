
#pragma once

#include "ofMain.h"
#include "ftSwapBuffer.h"
#include "ftVelocityBridgeShader.h"
#include "ftGaussianBlurShader.h"
#include "ftMultiplyForceShader.h"

namespace flowTools {
	
	class ftVelocityBridge {
	public:
		
		ftVelocityBridge();
		void		setup(int _width, int _height);
		void		reset() { swapBuffer.black(); }
		void		update();
		void		setSource(ofTexture& _tex) { velocityTexture = &_tex; }
		
		ofTexture&	getTexture()		{ return multiplyFbo.getTexture(); }
		
		int			getWidth() {return width;};
		int			getHeight(){return height;};
		
		float		getTrailWeight()	{ return trailWeight.get(); }
		float		getBlurRadius()		{ return blurRadius.get(); }
		float		getStrength()		{ return strength.get(); }
		
		void		setTrailWeight(float value)		{ trailWeight.set(value); }
		void		setBlurRadius(float value)		{ blurRadius.set(value); }
		void		setStrength(float value)		{ strength.set(value); }
		
		ofParameterGroup	parameters;
	protected:
		ofParameter<float>	trailWeight;
		ofParameter<float>	blurRadius;
		ofParameter<float>	strength;
		
		int			width;
		int			height;
		
		ofTexture*				velocityTexture;
		ftSwapBuffer			swapBuffer;
		ftVelocityBridgeShader	trailShader;
		ftGaussianBlurShader	blurShader;
		ftMultiplyForceShader	multiplyShader;
		ofFbo					multiplyFbo;
		
	};
}
