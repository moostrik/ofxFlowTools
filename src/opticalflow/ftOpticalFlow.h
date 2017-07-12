
#pragma once

#include "ofMain.h"
#include "ftSwapBuffer.h"
#include "ftOpticalFlowShader.h"

#include "ftAddForceShader.h"

namespace flowTools {
	
	class ftOpticalFlow {
	public:
		
		ftOpticalFlow();
		void		setup(int _width, int _height);
		void		reset() { sourceSwapBuffer.black(); bSourceSet = false; }
		void		setSource(ofTexture& _tex);
		void		update();
		void		update(ofTexture& _tex) { setSource(_tex); update(); }
		
		ofTexture&	getTexture()		{ return getOpticalFlow(); }
		ofTexture&	getOpticalFlow()	{ if(doCombinedFlow){ return velocityBufferCombined.getTexture(); } else { return velocityBuffer.getTexture();}; }
		
		int			getWidth() {return width;};
		int			getHeight(){return height;};
		
		
		float		getStrength()	{return strength.get();}
		int			getOffset()		{return offset.get();}
		float		getLambda()		{return lambda.get();}
		float		getThreshold()	{return threshold.get();}
		bool		getInverseX()	{return doInverseX.get();}
		bool		getInverseY()	{return doInverseY.get();}
		
		void		setStrength(float value)	{strength.set(value);}
		void		setOffset(int value)		{offset.set(value);}
		void		setLambda(float value)		{lambda.set(value);}
		void		setThreshold(float value)	{threshold.set(value);}
		void		setInverseX(bool value)		{doInverseX.set(value);}
		void		setInverseY(bool value)		{doInverseY.set(value);}
		
		ofParameterGroup	parameters;
	protected:
		ofParameter<float>	strength;
		ofParameter<int>	offset;
		ofParameter<float>	lambda;
		ofParameter<float>	threshold;
		ofParameter<bool>	doInverseX;
		ofParameter<bool>	doInverseY;
		ofParameter<bool>	doCombinedFlow;
		void doCombinedFlowListener(bool& _value) { if(_value) bSourceSet = false; }
		
		int		width;
		int		height;
		
		bool	bSourceSet;
		
		ftFbo				velocityBuffer;
		ftFbo				velocityBufferHalf;
		ftFbo				velocityBufferQuarter;
		ftFbo				velocityBufferEighth;
		ftFbo				velocityBufferCombined;
		ftFbo				decayBuffer;
		
		ftSwapBuffer		sourceSwapBuffer;
		ftSwapBuffer		sourceSwapBufferHalf;
		ftSwapBuffer		sourceSwapBufferQuarter;
		ftSwapBuffer		sourceSwapBufferEighth;
		
		ftOpticalFlowShader opticalFlowShader;
		
		ftAddForceShader	addShader;
		
	};
}
