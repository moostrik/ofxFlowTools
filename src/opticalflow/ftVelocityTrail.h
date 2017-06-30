
#pragma once

#include "ofMain.h"
#include "ftSwapBuffer.h"
#include "ftOpticalFlowShaderExp.h"
#include "ftTimeBlurShader.h"
#include "ftDecayShader.h"

namespace flowTools {
	
	class ftVelocityTrail {
	public:
		
		ftVelocityTrail();
		void		setup(int _width, int _height);
		void		reset() { timeBlurShader.reset();}
		void		update(float _deltaTime = 0);
		void		setSource(ofTexture& _tex) { velocityTexture = &_tex; }
		
		ofTexture&	getTexture()		{ return trailBuffer.getTexture(); }
		
		int			getWidth() {return width;};
		int			getHeight(){return height;};
		
		
		float		getStrength()	{return strength.get();}
		bool		getTimeBlurActive()	{return doTimeBlurDecay.get();}
		float		getTimeBlurRadius() {return timeBlurRadius.get();}
		float		getTimeBlurDecay() {return timeBlurDecay.get();}
		
		void		setStrength(float value)	{strength.set(value);}
		void		setTimeBlurActive(bool value) {doTimeBlurDecay.set(value);}
		void		setTimeBlurRadius(float value) {timeBlurRadius.set(value);}
		void		setTimeBlurDecay(float value) {timeBlurDecay.set(value);}
		
		ofParameterGroup	parameters;
	protected:
		ofParameter<float>	strength;
		ofParameter<bool>	doTimeBlurDecay;
		ofParameterGroup	timeBlurParameters;
		ofParameter<float>	timeBlurRadius;
		ofParameter<float>	timeBlurDecay;
		
		int		width;
		int		height;
		float	deltaTime;
		float   lastTime;
		float	timeStep;
		
		ofTexture*			velocityTexture;
		
		ftFbo				trailBuffer;
		ftTimeBlurShader	timeBlurShader;
		ftDecayShader		decayShader;
		
	};
}
