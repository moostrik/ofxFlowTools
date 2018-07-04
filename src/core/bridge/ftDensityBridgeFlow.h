#pragma once

#include "ofMain.h"
#include "ofxFlowTools.h"

#include "ftBridgeFlow.h"
#include "ftDensityBridgeShader.h"
#include "ftHSVShader.h"
#include "ftRGB2LuminanceShader.h"


namespace flowTools {
	
	class ftDensityBridgeFlow : public ftBridgeFlow {
	public:
		ftDensityBridgeFlow() {
			parameters.setName("density bridge");
			parameters.add(saturation.set("saturation", 1, 0, 3));
			//		parameters.add(hue.set("hue", 0, -.5, .5));
		}
		
		void setup(int _flowWidth, int _flowHeight)	{ setup(_flowWidth, _flowHeight, _flowWidth, _flowHeight); }
			
		void setup(int _flowWidth, int _flowHeight, int _densityWidth, int _densityHeight){
			ftBridgeFlow::allocate(_flowWidth, _flowHeight, _densityWidth, _densityHeight, GL_RGBA32F);
			visibleFbo.allocate(_densityWidth, _densityHeight, GL_RGBA);
			ftUtil::zero(visibleFbo);
			
			luminanceFbo.allocate(_densityWidth, _densityHeight, GL_R32F);  // should go to temperatureBridge
			ftUtil::zero(luminanceFbo);
		};
		
		void update(float _deltaTime) {
			ftBridgeFlow::update();
			if (!bInputSet || !bVelocityInputSet) { return; }
			
			ofPushStyle();
			ofEnableBlendMode(OF_BLENDMODE_DISABLED);
			resetOutput();
			densityBridgeShader.update(outputFbo, inputFbo.getTexture(), velocityTrailFbo.getTexture(), _deltaTime * speed.get() * .1);
			outputFbo.swap();
			HSVShader.update(outputFbo, outputFbo.getBackTexture(), 0, saturation.get(), 1.0);
			resetInput();
			
			RGB2LuminanceShader.update(luminanceFbo, outputFbo.getTexture());
			ofPopStyle();
		}
		
		void reset() {
			ftBridgeFlow::reset();
			
			ftUtil::zero(luminanceFbo);
			ftUtil::zero(visibleFbo);
		}
		
		ofTexture&	getVisible() {
			ftUtil::zero(visibleFbo);
			multiplyShader.update(visibleFbo, outputFbo.getTexture(), ofGetFrameRate());
			return visibleFbo.getTexture();
		}
		
		void draw(int _x, int _y, int _w, int _h, ofBlendMode _blendMode = OF_BLENDMODE_ALPHA) {
			ofPushStyle();
			ofEnableBlendMode(_blendMode);
			getVisible().draw(_x, _y, _w, _h);
			ofPopStyle();
		}
		
		void	setDensity(ofTexture& _inputTex) 						{ setInput(_inputTex); }
		void	addDensity(ofTexture& _inputTex, float _strength = 1.0)	{ addInput(_inputTex, _strength); }
		
		void	setSaturation(float value)	{ saturation.set(value); }
		
		ofTexture& getDensity() 			{ return getOutput(); };
		ofTexture& getLuminance()			{ return luminanceFbo.getTexture(); };
		
		float	getSaturation()				{ return saturation.get(); }
		
	protected:
		ofParameter<float>		saturation;
//		ofParameter<float>		hue;
		
		ftDensityBridgeShader 	densityBridgeShader;
		ftMultiplyForceShader	multiplyShader;
		ofFbo					visibleFbo;
		
		ftHSVShader				HSVShader;
		ftRGB2LuminanceShader	RGB2LuminanceShader;
		
		ofFbo					luminanceFbo;  // should go to temperatureBridge
	};
}

