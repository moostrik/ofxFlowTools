#pragma once

#include "ofMain.h"
#include "ofxFlowTools.h"

#include "ftVelocityBridgeShader.h"
#include "ftDensityBridgeShader.h"
#include "ftHSVShader.h"
#include "ftGaussianBlurShader.h"
#include "ftRGB2LuminanceShader.h"
#include "ftMultiplyForceShader.h"


namespace flowTools {
	
	class ftDensityBridge : public ftFlow {
	public:
		
		void setup(int _flowWidth, int _flowHeight, int _densityWidth = 0, int _densityHeight = 0){
			if (_densityWidth == 0 ) _densityWidth = _flowWidth;
			if (_densityHeight == 0 ) _densityHeight = _flowHeight;
			
			ftFlow::allocate(_densityWidth, _densityHeight, GL_RGBA32F);
			velocityBridge.setup(_flowWidth, _flowHeight);
			luminanceFbo.allocate(_densityWidth, _densityHeight, GL_R32F);
			ftUtil::zero(luminanceFbo);
			visibleFbo.allocate(_densityWidth, _densityHeight, GL_RGBA);
			ftUtil::zero(visibleFbo);
			
			parameters.setName("flow density");
			parameters.add(trailWeight.set("trail", .5, 0, .99));
			parameters.add(blurRadius.set("blur", 5, 0, 10)); // blur works funky above 5
			parameters.add(saturation.set("saturation", 1, 0, 3));
			//		parameters.add(hue.set("hue", 0, -.5, .5));
			parameters.add(speed.set("speed", 8, 0, 20));
		};
		
		void update(float _deltaTime) {
			ofPushStyle();
			ofEnableBlendMode(OF_BLENDMODE_DISABLED);
			resetOutput();
			
			if (!bInputSet || !velocityBridge.getInputSet()) {
				ofLogVerbose("ftDensityBridge: velocity or density texture not set, can't update");
			}
			else {
				velocityBridge.setTrailWeight(trailWeight.get());
				velocityBridge.setBlurRadius(blurRadius.get());
				velocityBridge.update(_deltaTime);
				
				outputFbo.swap();
				densityBridgeShader.update(outputFbo,
										   inputFbo.getTexture(),
										   velocityBridge.getOutput(),
										   speed.get() * _deltaTime);
				outputFbo.swap();
				HSVShader.update(outputFbo,
								 outputFbo.getBackTexture(),
								 0, // hue.get(),
								 saturation.get(),
								 1.0);
				
				RGB2LuminanceShader.update(luminanceFbo, outputFbo.getTexture());
//				resetInput();
			}
			
			ofPopStyle();
		}
		
		void reset() {
			ftFlow::reset();
			velocityBridge.reset();
			ftUtil::zero(luminanceFbo);
			ftUtil::zero(visibleFbo);
		}
		
		void	draw(int _x, int _y, int _width, int _height, ofBlendMode _blendmode = OF_BLENDMODE_ALPHA){
			ofPushStyle();
			ofEnableBlendMode(_blendmode);
			multiplyShader.update(visibleFbo, outputFbo.getTexture(), ofGetFrameRate());
			visibleFbo.getTexture().draw(_x, _y, _width, _height);
			ofPopStyle();
		}
		
		void	setDensity(ofTexture& _inputTex) 					{ setInput(_inputTex); }
		void	addDensity(ofTexture& _inputTex, float _strength) 	{ addInput(_inputTex, _strength); }
		void	setVelocity(ofTexture& _inputTex) 					{ velocityBridge.setInput(_inputTex); }
		void	addVelocity(ofTexture& _inputTex, float _strength) 	{ velocityBridge.addInput(_inputTex, _strength); }
		
		
		ofTexture& getDensity() 				{ return getOutput(); };
		ofTexture& getLuminance()				{ return luminanceFbo.getTexture(); };
		
		float	getTrailWeight()				{ return trailWeight.get(); }
		float	getBlurRadius()					{ return blurRadius.get(); }
		float	getSaturation()					{ return saturation.get(); }
		float	getSpeed()						{ return speed.get(); }
		
		void	setTrailWeight(float value)		{ trailWeight.set(value); }
		void	setBlurRadius(float value)		{ blurRadius.set(value); }
		void	setSaturation(float value)		{ saturation.set(value); }
		void	setSpeed(float value)			{ speed.set(value); }
		
	protected:
		ofParameter<float>		blurRadius;
		ofParameter<float>		trailWeight;
//		ofParameter<float>		hue;
		ofParameter<float>		saturation;
		ofParameter<float>		speed;
		
		ftVelocityBridge		velocityBridge;
		ftDensityBridgeShader 	densityBridgeShader;
		ofFbo					luminanceFbo;
		ftMultiplyForceShader	multiplyShader;
		ofFbo					visibleFbo;
		
		ftHSVShader				HSVShader;
		ftRGB2LuminanceShader	RGB2LuminanceShader;
	};
}

