
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
			parameters.add(trailWeight.set("trail"	, 0.3, 0.0, 0.99));
			parameters.add(blurRadius.set("blur"	, 3.0, 0.0, 10.0));
			parameters.add(speed.set("speed"		, 0.3, 0.0, 1.0 ));
		}
		
		void setVelocity(ofTexture &_inputTex) { set(velocityInputFbo, _inputTex);}
		
		void addVelocity(ofTexture &_inputTex, float _strength) { add(velocityInputFbo, _inputTex, _strength);}
		
		virtual ofTexture&	getVelocity()	{ return getOutput(); }
		
		virtual void update(float _deltaTime)  {
			ofPushStyle();
			ofEnableBlendMode(OF_BLENDMODE_DISABLED);
			
			velocityTrailFbo.swap();
			bridgeShader.update(velocityTrailFbo.get(), velocityTrailFbo.getBackTexture(), velocityInputFbo.getTexture(), trailWeight.get());
			if (blurRadius.get() > 0) { blurShader.update(velocityTrailFbo.get(), 1, blurRadius.get()); }
//			ftUtil::zero(velocityInputFbo);
			ofPopStyle();
		}
		
		virtual void reset() override { ftFlow::reset(); ftUtil::zero(velocityInputFbo); ftUtil::zero(velocityTrailFbo); }
		
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
		
		virtual void allocate(int _velocityWidth, int _velocityHeight, GLint _velocityInternalFormat, int _inOutputWidth, int _inOutputHeight, GLint _inOutputInternalFormat) override{
			ftFlow::allocate(_inOutputWidth, _inOutputHeight, _inOutputInternalFormat, _inOutputWidth, _inOutputHeight, _inOutputInternalFormat);
			velocityInputFbo.allocate(_velocityWidth, _velocityHeight, _velocityInternalFormat);
			velocityTrailFbo.allocate(_velocityWidth, _velocityHeight, _velocityInternalFormat);
			ftUtil::zero(velocityInputFbo);
			ftUtil::zero(velocityTrailFbo);
		}
		
	};
}
