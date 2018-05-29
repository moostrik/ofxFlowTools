
#pragma once

#include "ofMain.h"
#include "ftSwapBuffer.h"
#include "ftVelocityBridgeShader.h"
#include "ftGaussianBlurShader.h"

namespace flowTools {
	
	class ftVelocityBridge {
	public:
		
		ftVelocityBridge();
		void		setup(int _width, int _height);
		void		reset() { trailSwapBuffer.black(); }
		void		update();
		void		setSource(ofTexture& _tex) { velocityTexture = &_tex; }
		
		ofTexture&	getTexture()		{ return trailSwapBuffer.getTexture(); }
		
		int			getWidth() {return width;};
		int			getHeight(){return height;};
		
		
		float		getTrailWeight()	{ return trailWeight.get(); }
		float		getBlurRadius()		{ return blurRadius.get(); }
		float		getBlurPasses()		{ return blurPasses.get(); }
		
		void		setTrailWeight(float value)		{ trailWeight.set(value); }
		void		setBlurRadius(float value)		{ blurRadius.set(value); }
		void		setBlurPasses(float value)		{ blurPasses.set(value); }
		
		ofParameterGroup	parameters;
	protected:
		ofParameter<float>	trailWeight;
		ofParameter<float>	trailStrength;
		ofParameter<int>	blurPasses;
		ofParameter<float>	blurRadius;
		
		int		width;
		int		height;
		float	deltaTime;
		float   lastTime;
		float	timeStep;
		
		ofTexture*			velocityTexture;
		
		ftSwapBuffer		trailSwapBuffer;
		
		ftVelocityBridgeShader	trailShader;
		ftGaussianBlurShader	blurShader;
		
	};
}
