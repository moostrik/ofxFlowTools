#pragma once

#include "ofMain.h"
#include "ofxFlowTools.h"

#include "ftBridgeFlow.h"
#include "ftTemperatureBridgeShader.h"


namespace flowTools {
	
	class ftTemperatureBridgeFlow : public ftBridgeFlow {
	public:
		ftTemperatureBridgeFlow() {
			parameters.setName("temperature bridge");
			speed.setMin(-1.0);
		}
		
		void setup(int _simulationWidth, int _simulationHeight)	{
			allocate(_simulationWidth, _simulationHeight, GL_RG32F, _simulationWidth, _simulationHeight, GL_R32F);
		};
		
		void update(float _deltaTime) override {
			ftBridgeFlow::update(_deltaTime);
			
			ofPushStyle();
			ofEnableBlendMode(OF_BLENDMODE_DISABLED);
			resetOutput();

			float timeStep = _deltaTime * speed.get() * 100;
			temperatureBridgeShader.update(outputFbo.get(), inputFbo.getTexture(), velocityTrailFbo.getTexture(), timeStep);

			ofPopStyle();
		}
		
		void	setDensity(ofTexture& _inputTex) 						{ setInput(_inputTex); }
		void	addDensity(ofTexture& _inputTex, float _strength = 1.0)	{ addInput(_inputTex, _strength); }
		
		ofTexture& getTemperature() 		{ return getOutput(); };
		
	protected:
		ftTemperatureBridgeShader 	temperatureBridgeShader;
	};
}

