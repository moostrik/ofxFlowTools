
#pragma once

#include "ofMain.h"
#include "ofxFlowTools.h"

#include "ftOpticalFlowShader.h"
#include "ftRGB2LuminanceShader.h"

namespace flowTools {
	
	class ftOpticalFlow {
	public:
		
		ftOpticalFlow();
		void		setup(int _width, int _height);
		void		reset();
		void		setInput(ofTexture& _tex);
		void		update();
		void		update(ofTexture& _tex) { setInput(_tex); update(); }
		
		ofTexture&	getVelocity()		{ return getTexture(); }
		ofTexture&	getTexture()		{ return velocityFbo.getTexture(); }
		
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
		bool	bFirstFrame;
		
		ftSwapFbo				inputSwapFbo;
		ofFbo					velocityFbo;
		ftOpticalFlowShader 	opticalFlowShader;
		ftRGB2LuminanceShader	RGB2LumShader;
		
	};
}
