
#pragma once

#include "ofMain.h"
#include "ftUtil.h"
#include "ftSwapBuffer.h"
#include "ftOpticalFlowShader.h"

#include "ftLuminanceShader.h"

namespace flowTools {
	
	class ftOpticalFlow {
	public:
		
		ftOpticalFlow();
		void		setup(int _width, int _height);
		void		reset() { ftUtil::zero(sourceSwapBuffer); bSourceSet = false; }
		void		setSource(ofTexture& _tex);
		void		update();
		void		update(ofTexture& _tex) { setSource(_tex); update(); }
		
		ofTexture&	getTexture()		{ return getOpticalFlow(); }
		ofTexture&	getOpticalFlow()	{ return velocityBuffer.getTexture(); }
		
		int			getWidth() {return width;};
		int			getHeight(){return height;};
		
		
		float		getStrength()	{return strength.get();}
		int			getOffset()		{return offset.get();}
		float		getThreshold()	{return threshold.get();}
		bool		getInverseX()	{return doInverseX.get();}
		bool		getInverseY()	{return doInverseY.get();}
		
		void		setStrength(float value)	{strength.set(value);}
		void		setOffset(int value)		{offset.set(value);}
		void		setThreshold(float value)	{threshold.set(value);}
		void		setInverseX(bool value)		{doInverseX.set(value);}
		void		setInverseY(bool value)		{doInverseY.set(value);}
		
		ofParameterGroup&	getParameters() 	{ return parameters; }
		
	protected:
		ofParameterGroup	parameters;
		ofParameter<float>	threshold;
		ofParameter<int>	offset;
		ofParameter<float>	strength;
		ofParameter<float>	power;
		ofParameter<bool>	doInverseX;
		ofParameter<bool>	doInverseY;
		
		int		width;
		int		height;
		
		bool	bSourceSet;
		
		ftSwapBuffer		sourceSwapBuffer;
		ofFbo				velocityBuffer;
		ftOpticalFlowShader opticalFlowShader;
		ftLuminanceShader	luminanceShader;
		ofFbo				luminanceBuffer;
		
	};
}
