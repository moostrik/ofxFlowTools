
#pragma once

#include "ofMain.h"
#include "ofxFlowTools.h"

#include "ftBridgeFlow.h"

namespace flowTools {
	
	class ftVelocityBridgeFlow : public ftBridgeFlow {
	public:
		ftVelocityBridgeFlow() {
			parameters.setName("velocity bridge");
		}
		
		void setup(int _width, int _height){
			ftBridgeFlow::allocate(_width, _height, _width, _height, GL_RG32F);
		};
		
		void update(float _deltaTime)  {
			bInputSet = true; // input is not used as it is the same as velocityInput
			ftBridgeFlow::update();
			if (!bVelocityInputSet) { return; }
			
			ofPushStyle();
			ofEnableBlendMode(OF_BLENDMODE_DISABLED);
			resetOutput();
			multiplyShader.update(outputFbo, velocityTrailFbo.getTexture(), speed.get() * _deltaTime );
			ofPopStyle();
		}
		
		void reset() { ftBridgeFlow::reset(); }
		
		ofTexture&	getVelocity()	{ return getOutput(); }
		void 	setInput(ofTexture &_texture)			{ setVelocity(_texture); }
		
	protected:
		ftMultiplyForceShader		multiplyShader;
		
	};
}
