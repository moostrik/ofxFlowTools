
#pragma once

#include "ofMain.h"
#include "ftSwapBuffer.h"
#include "ftOpticalFlowShader.h"
#include "ftTimeBlurShader.h"

namespace flowTools {
	
	class ftOpticalFlow {
	public:
		
		ftOpticalFlow();
		void		setup(int _width, int _height);
		void		update(float _deltaTime = 0);
		void		setSource(ofTexture& _tex);
		
		ofTexture&	getTextureReference() {return getOpticalFlow() ;};
		ofTexture&	getOpticalFlow() {return velocityBuffer.getTextureReference();};
		ofTexture&	getOpticalFlowDecay() {if(doTimeBlurDecay.get()) return decayBuffer.getTextureReference(); else return velocityBuffer.getTextureReference();
											};
//		int			getFlowVectorSize(){return width * height;};
//		ofVec2f*	getFlowVectors();
//		float		getAverageFlow();
		
		int			getWidth() {return width;};
		int			getHeight(){return height;};
		
		
		float		getStrength()	{return strength.get();}
		int			getOffset()		{return offset.get();}
		float		getLambda()		{return lambda.get();}
		float		getThreshold()	{return threshold.get();}
		bool		getInverseX()	{return doInverseX.get();}
		bool		getInverseY()	{return doInverseY.get();}
		bool		getTimeBlurActive()	{return doTimeBlurDecay.get();}
		float		getTimeBlurRadius() {return timeBlurRadius.get();}
		float		getTimeBlurDecay() {return timeBlurDecay.get();}
		
		void		setStrength(float value)	{strength.set(value);}
		void		setOffset(int value)		{offset.set(value);}
		void		setLambda(float value)		{lambda.set(value);}
		void		setThreshold(float value)	{threshold.set(value);}
		void		setInverseX(bool value)		{doInverseX.set(value);}
		void		setInverseY(bool value)		{doInverseY.set(value);}
		void		setTimeBlurActive(bool value) {doTimeBlurDecay.set(value);}
		void		setTimeBlurRadius(float value) {timeBlurRadius.set(value);}
		void		setTimeBlurDecay(float value) {timeBlurDecay.set(value);}
		
		ofParameterGroup	parameters;
	protected:
		ofParameter<float>	strength;
		ofParameter<int>	offset;
		ofParameter<float>	lambda;
		ofParameter<float>	threshold;
		ofParameter<bool>	doInverseX;
		ofParameter<bool>	doInverseY;
		ofParameter<bool>	doTimeBlurDecay;
		ofParameterGroup	timeBlurParameters;
		ofParameter<float>	timeBlurRadius;
		ofParameter<float>	timeBlurDecay;
		
		int		width;
		int		height;
		float	deltaTime;
		float   lastTime;
		float	timeStep;
		
		bool bSourceSet;
		
		ofTexture			velocityTexture;
		
		ftFbo				velocityBuffer;
		ftFbo				decayBuffer;
		ftSwapBuffer		sourceSwapBuffer;
		ftOpticalFlowShader opticalFlowShader;
		ftTimeBlurShader	timeBlurShader;
		
		
//		float*	flowFloats;
//		ofVec2f* flowVectors;
//		bool	flowVectorsDidUpdate;
		
	};
}
