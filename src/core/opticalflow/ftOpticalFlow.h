
#pragma once

#include "ofMain.h"
#include "ofxFlowTools.h"

#include "ftFlow.h"
#include "ftOpticalFlowShader.h"
#include "ftRGB2LuminanceShader.h"

namespace flowTools {
	
	class ftOpticalFlow : public ftFlow {
	public:
		void setup(int _width, int _height) {
			ftFlow::allocate(_width, _height, GL_R8, _width, _height, GL_RG32F);
			opticalFlowFbo.allocate(_width, _height, GL_R8);
			ftUtil::zero(opticalFlowFbo);
			RGB2LumFbo.allocate(_width, _height, GL_R8);
			ftUtil::zero(RGB2LumFbo);
			
			bFirstFrame = true;
			
			parameters.setName("optical flow");
			offset.set("offset", 3, 1, 10);
			threshold.set("threshold", 0.02, 0, 0.2);
			strength.set("force", 3, .1, 10);			// 3 is best for normalization
			power.set("power", 1.0, .01, 1);			//
			doInverseX.set("inverse x", true); 			// flow velocity is inverse to fluid velocity
			doInverseY.set("inverse y", true); 			// flow velocity is inverse to fluid velocity
			
			//		parameters.add(offset);
			parameters.add(threshold);
			//		parameters.add(strength);
			//		parameters.add(power);
			//		parameters.add(doInverseX);
			//		parameters.add(doInverseY);
		};
		
		void update() {
			if (bInputSet) {
				bInputSet = false;
				
				ofPushStyle();
				ofEnableBlendMode(OF_BLENDMODE_DISABLED);
				
				opticalFlowFbo.swap();
				ftUtil::stretch(opticalFlowFbo, inputFbo.getTexture());
				
				if (bFirstFrame) {
					bFirstFrame = false;
					opticalFlowFbo.swap();
					ftUtil::stretch(opticalFlowFbo, opticalFlowFbo.getBackTexture());
				}
				
				opticalFlowShader.update(outputFbo, opticalFlowFbo.getTexture(), opticalFlowFbo.getBackTexture(), offset.get(), threshold.get(),  glm::vec2(strength.get()),  power.get(), doInverseX.get(), doInverseY.get());
				
				ofPopStyle();
			}
		}
		
		void setInput(ofTexture& _tex) override {
			ofPushStyle();
			ofEnableBlendMode(OF_BLENDMODE_DISABLED);
			ftUtil::zero(inputFbo);
			if (_tex.getTextureData().glInternalFormat != GL_R8) {
				RGB2LumShader.update(inputFbo, _tex); }
			else {
				ftUtil::stretch(inputFbo, _tex);
			}
			bInputSet = true;
			
			ofPopStyle();
		}
		
		void addInput(ofTexture& _tex , float _strength = 1.0) override {
			inputFbo.swap();
			if (_tex.getTextureData().glInternalFormat != GL_R8) {
				RGB2LumShader.update(RGB2LumFbo, _tex);
				AddMultipliedShader.update(inputFbo, inputFbo.getBackTexture(), RGB2LumFbo.getTexture(), 1.0, _strength);
			} else {
				AddMultipliedShader.update(inputFbo, inputFbo.getBackTexture(), _tex, 1.0, _strength);
			}
			bInputSet = true;
			//	ofLogWarning("ftOpticalFlow: addInput") << " to the optical flow input can only be set";
		}
		
		void reset() override { ftFlow::reset(); bFirstFrame = true; }
		
		void		setStrength(float value)	{strength.set(value);}
		void		setOffset(int value)		{offset.set(value);}
		void		setThreshold(float value)	{threshold.set(value);}
		void		setInverseX(bool value)		{doInverseX.set(value);}
		void		setInverseY(bool value)		{doInverseY.set(value);}
	
		ofTexture&	getVelocity()	{ return getOutput(); }
		
		float		getStrength()	{return strength.get();}
		int			getOffset()		{return offset.get();}
		float		getThreshold()	{return threshold.get();}
		bool		getInverseX()	{return doInverseX.get();}
		bool		getInverseY()	{return doInverseY.get();}
		
	protected:
		ofParameter<float>			threshold;
		ofParameter<int>			offset;
		ofParameter<float>			strength;
		ofParameter<float>			power;
		ofParameter<bool>			doInverseX;
		ofParameter<bool>			doInverseY;
		
		bool						bInputSet;
		bool						bFirstFrame;
		ftOpticalFlowShader 		opticalFlowShader;
		ftRGB2LuminanceShader		RGB2LumShader;
		
		ofFbo						RGB2LumFbo;
		ftPingPongFbo				opticalFlowFbo;
	};
}
