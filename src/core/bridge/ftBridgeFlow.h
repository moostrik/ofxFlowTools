
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
		
		void setVelocity(ofTexture &_inputTex) { set(velocityInputFbo, _inputTex);}
		
		void addVelocity(ofTexture &_inputTex, float _strength) { add(velocityInputFbo, _inputTex, _strength);}
		
		virtual ofTexture&	getVelocity()	{ return getOutput(); }
		
		virtual void update(float _deltaTime)  {
			ofPushStyle();
			ofEnableBlendMode(OF_BLENDMODE_DISABLED);
			
			velocityTrailFbo.swap();
			bridgeShader.update(velocityTrailFbo, velocityTrailFbo.getBackTexture(), velocityInputFbo.getTexture(), trailWeight.get());
			if (blurRadius.get() > 0) { blurShader.update(velocityTrailFbo, 1, blurRadius.get()); }
			ftUtil::zero(velocityInputFbo);
			ofPopStyle();
		}
		
		virtual void reset() { ftFlow::reset(); ftUtil::zero(velocityTrailFbo); }
		
		void	setTrailWeight(float value)		{ trailWeight.set(value); }
		void	setBlurRadius(float value)		{ blurRadius.set(value); }
		void	setSpeed(float value)			{ speed.set(value); }
		
		float	getTrailWeight()	{ return trailWeight.get(); }
		float	getBlurRadius()		{ return blurRadius.get(); }
		float	getSpeed()			{ return speed.get(); }
		
	protected:
		ofParameter<float>			trailWeight;
		ofParameter<float>			blurRadius;
		ofParameter<float>			speed;
		ftPingPongFbo				velocityInputFbo;
		ftPingPongFbo				velocityTrailFbo;
		ftBridgeShader				bridgeShader;
		ftGaussianBlurShader		blurShader;
		ftMultiplyForceShader		multiplyShader;
		
		void allocate(int _velocityWidth, int _velocityHeight, int _inputWidth, int _inputHeight, int _internalFormat){
			ftFlow::allocate(_inputWidth, _inputHeight, _internalFormat);
			velocityInputFbo.allocate(_velocityWidth, _velocityHeight, GL_RG32F);
			velocityTrailFbo.allocate(_velocityWidth, _velocityHeight, GL_RG32F);
			ftUtil::zero(velocityInputFbo);
			ftUtil::zero(velocityTrailFbo);
		}
		
	};
}
