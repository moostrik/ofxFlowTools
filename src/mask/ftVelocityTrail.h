
#pragma once

#include "ofMain.h"
#include "ftSwapBuffer.h"
#include "ftTrailShader.h"
#include "ftGaussianBlurShader.h"

namespace flowTools {
	
	class ftVelocityTrail {
	public:
		
		ftVelocityTrail();
		void		setup(int _width, int _height);
		void		reset() { trailSwapBuffer.black(); }
		void		update(float _deltaTime = 0);
		void		setSource(ofTexture& _tex) { velocityTexture = &_tex; }
		
		ofTexture&	getTexture()		{ return trailSwapBuffer.getTexture(); }
		
		int			getWidth() {return width;};
		int			getHeight(){return height;};
		
		
		float		getStrength()		{ return strength.get(); }
		float		getTrailWeight()	{ return trailWeight.get(); }
		float		getBlurRadius()		{ return blurRadius.get(); }
		float		getBlurPasses()		{ return blurPasses.get(); }
		
		void		setStrength(float value)		{ strength.set(value); }
		void		setTrailWeight(float value)		{ trailWeight.set(value); }
		void		setBlurRadius(float value)		{ blurRadius.set(value); }
		void		setBlurPasses(float value)		{ blurPasses.set(value); }
		
		ofParameterGroup	parameters;
	protected:
		ofParameter<float>	strength;
		ofParameter<float>	trailWeight;
		ofParameter<int>	blurPasses;
		ofParameter<float>	blurRadius;
		
		int		width;
		int		height;
		float	deltaTime;
		float   lastTime;
		float	timeStep;
		
		ofTexture*			velocityTexture;
		
		ftSwapBuffer		trailSwapBuffer;
		
		ftTrailShader		trailShader;
		ftGaussianBlurShader	blurShader;
		
	};
}
