
#pragma once

#include "ofMain.h"
#include "ofxFlowTools.h"

#include "ftFlow.h"
#include "ftBridgeShader.h"
#include "ftGaussianBlurShader.h"
#include "ftMultiplyForceShader.h"

namespace flowTools {
	
	class ftBridgeFlow : public ftFlow {
	public:
		ftBridgeFlow() {
			parameters.setName("bridge");
			parameters.add(trailWeight.set("trail", .25, 0, .99));
			parameters.add(blurRadius.set("blur", 2.5, 0, 10));
			parameters.add(speed.set("speed", 50, .1, 100));
		}
		
		
		void allocate(int _velocityWidth, int _velocityHeight, int _inputWidth, int _inputHeight, int _internalFormat){
			ftFlow::allocate(_inputWidth, _inputHeight, _internalFormat);
			velocityInputFbo.allocate(_velocityWidth, _velocityHeight, GL_RG32F);
			velocityTrailFbo.allocate(_velocityWidth, _velocityHeight, GL_RG32F);
			ftUtil::zero(velocityInputFbo);
			ftUtil::zero(velocityTrailFbo);
			
		}
		
		void setVelocity(ofTexture &_inputTex) {
			ofPushStyle();
			ofEnableBlendMode(OF_BLENDMODE_DISABLED);
			ftUtil::stretch(velocityInputFbo, _inputTex);
			bVelocityInputSet = true;
			ofPopStyle();
		}
		
		void addVelocity(ofTexture &_inputTex, float _strength) {
			ofPushStyle();
			ofEnableBlendMode(OF_BLENDMODE_DISABLED);
			inputFbo.swap();
			AddMultipliedShader.update(velocityInputFbo, velocityInputFbo.getBackTexture(), _inputTex, 1.0, _strength);
			bVelocityInputSet = true;
			ofPopStyle();
		}
		
		void update()  {
			if (!bInputSet) {
				ofLogWarning("ftBridgeFlow: input texture not set, can't update");
				return;
			}
			if (!bVelocityInputSet) {
				ofLogWarning("ftBridgeFlow: velocity input texture not set, can't update");
				return;
			}
			ofPushStyle();
			ofEnableBlendMode(OF_BLENDMODE_DISABLED);
			
			velocityTrailFbo.swap();
			bridgeShader.update(velocityTrailFbo, velocityTrailFbo.getBackTexture(), velocityInputFbo.getTexture(), trailWeight.get());
			if (blurRadius.get() > 0) { blurShader.update(velocityTrailFbo, 1, blurRadius.get()); }
			ftUtil::zero(velocityInputFbo);
			ofPopStyle();
		}
		
		void reset() { ftFlow::reset(); ftUtil::zero(velocityTrailFbo); }
		
		void	setTrailWeight(float value)		{ trailWeight.set(value); }
		void	setBlurRadius(float value)		{ blurRadius.set(value); }
		void	setSpeed(float value)			{ speed.set(value); }
		
		ofTexture&	getVelocity()	{ return getOutput(); }
		
		float	getTrailWeight()	{ return trailWeight.get(); }
		float	getBlurRadius()		{ return blurRadius.get(); }
		float	getSpeed()			{ return speed.get(); }
		
	protected:
		ofParameter<float>			trailWeight;
		ofParameter<float>			blurRadius;
		ofParameter<float>			speed;
		ftPingPongFbo				velocityInputFbo;
		bool						bVelocityInputSet;
		ftPingPongFbo				velocityTrailFbo;
		ftBridgeShader				bridgeShader;
		ftGaussianBlurShader		blurShader;
		ftMultiplyForceShader		multiplyShader;
		
	};
}
