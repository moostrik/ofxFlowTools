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
		
		void setup(int _simulationWidth, int _simulationHeight)	{ setup(_simulationWidth, _simulationHeight, _simulationWidth, _simulationHeight); }
			
		void setup(int _simulationWidth, int _simulationHeight, int _densityWidth, int _densityHeight){
			allocate(_simulationWidth, _simulationHeight, GL_RG32F, _densityWidth, _densityHeight, GL_RGBA32F);
		};
		
		void update(float _deltaTime) override {
			ftBridgeFlow::update(_deltaTime);
			
			ofPushStyle();
			ofEnableBlendMode(OF_BLENDMODE_DISABLED);
			resetOutput();

			float timeStep = _deltaTime * speed.get() * 10;
			densityBridgeShader.update(outputFbo.get(), inputFbo.getTexture(), velocityTrailFbo.getTexture(), timeStep);

			outputFbo.swap();
			HSVShader.update(outputFbo.get(), outputFbo.getBackTexture(), 0, saturation.get(), 1.0);
			
			ofPopStyle();
		}
		
		void reset() override {
			ftBridgeFlow::reset();
			ftUtil::zero(visibleFbo);
		}
		
		ofTexture&	getVisible() {
			ftUtil::zero(visibleFbo);
			multiplyShader.update(visibleFbo, outputFbo.getTexture(), ofGetFrameRate());
			return visibleFbo.getTexture();
		}
		
		void	drawInput(int _x, int _y, int _w, int _h) override { inputFbo.draw(_x, _y, _w, _h); }
		void	drawOutput(int _x, int _y, int _w, int _h) override { getVisible().draw(_x, _y, _w, _h); }
		
		void	setDensity(ofTexture& _inputTex) 						{ setInput(_inputTex); }
		void	addDensity(ofTexture& _inputTex, float _strength = 1.0)	{ addInput(_inputTex, _strength); }
		
		void	setSaturation(float value)	{ saturation.set(value); }
		
		ofTexture& getDensity() 			{ return getOutput(); };
		
		float	getSaturation()				{ return saturation.get(); }
		
	protected:
		ofParameter<float>		saturation;
//		ofParameter<float>		hue;
		
		ftDensityBridgeShader 	densityBridgeShader;
		ofFbo					visibleFbo;
		
		ftHSVShader				HSVShader;
		
		void allocate(int _velocityWidth, int _velocityHeight, GLint _velocityInternalFormat, int _inOutputWidth, int _inOutputHeight, GLint _inOutputInternalFormat) override {
			ftBridgeFlow::allocate(_velocityWidth, _velocityHeight, _velocityInternalFormat, _inOutputWidth, _inOutputHeight, _inOutputInternalFormat);
			
			visibleFbo.allocate(_inOutputWidth, _inOutputHeight, GL_RGBA);
			ftUtil::zero(visibleFbo);
		}
	};
}

