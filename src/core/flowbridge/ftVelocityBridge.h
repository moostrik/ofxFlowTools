
#pragma once

#include "ofMain.h"
#include "ofxFlowTools.h"

#include "ftFlow.h"
#include "ftVelocityBridgeShader.h"
#include "ftGaussianBlurShader.h"
#include "ftMultiplyForceShader.h"

namespace flowTools {
	
	class ftVelocityBridge : public ftFlow {
	public:
		void setup(int _width, int _height){
			ftFlow::allocate(_width, _height, GL_RG32F);
			
			velocitySwapFbo.allocate(width, height, internalFormat);
			ftUtil::zero(velocitySwapFbo);
			
			parameters.setName("flow velocity");
			parameters.add(trailWeight.set("trail", .25, 0, .99));
			parameters.add(blurRadius.set("blur", 2.5, 0, 10));
			parameters.add(speed.set("speed", 30, .1, 100));
		};
		
		void update(float _deltaTime)  {
			if (bInputSet) {
				ofPushStyle();
				ofEnableBlendMode(OF_BLENDMODE_DISABLED);
				
				velocitySwapFbo.swap();
				bridgeShader.update(velocitySwapFbo, velocitySwapFbo.getBackTexture(), inputFbo.getTexture(), trailWeight.get());
				if (blurRadius.get() > 0) { blurShader.update(velocitySwapFbo, 1, blurRadius.get()); }
				multiplyShader.update(outputFbo, velocitySwapFbo.getTexture(), speed.get() * _deltaTime );
				
				ofPopStyle();
				
				ftFlow::resetInput();
			}
		}
		
		void reset() { ftFlow::reset(); ftUtil::zero(velocitySwapFbo); }
		
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
		ftPingPongFbo				velocitySwapFbo;
		ftVelocityBridgeShader		bridgeShader;
		ftGaussianBlurShader		blurShader;
		ftMultiplyForceShader		multiplyShader;
		
	};
}
