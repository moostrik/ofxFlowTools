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
    parameters.add(saturation.set("saturation", 2.5, 0.0, 5.0));
  }

  void setup(int _simulationWidth, int _simulationHeight)  { setup(_simulationWidth, _simulationHeight, _simulationWidth, _simulationHeight); }

  void setup(int _simulationWidth, int _simulationHeight, int _densityWidth, int _densityHeight){
    allocate(_simulationWidth, _simulationHeight, GL_RG32F, _densityWidth, _densityHeight, GL_RGBA32F);
  };

  void update(float deltaTime) override {
    ftBridgeFlow::update(deltaTime);

    ofPushStyle();
    ofEnableBlendMode(OF_BLENDMODE_DISABLED);
    resetOutput();

    densityBridgeShader.update(outputFbo.get(), inputFbo.getTexture(), velocityTrailFbo.getTexture(), 1.0);

    outputFbo.swap();
    HSVShader.update(outputFbo.get(), outputFbo.getBackTexture(), 0, saturation.get(), 1.0);

    ofPopStyle();
  }

  void reset() override {
    ftBridgeFlow::reset();
    ftUtil::zero(visibleFbo);
  }

  ofTexture&  getVisible() {
    ofPushStyle();
    ofEnableBlendMode(OF_BLENDMODE_DISABLED);
    multiplyShader.update(visibleFbo, outputFbo.getTexture(), ofGetFrameRate());
    ofPopStyle();
    return visibleFbo.getTexture();
  }

  void  drawInput(int _x, int _y, int _w, int _h) override { inputFbo.draw(_x, _y, _w, _h); }
  void  drawOutput(int _x, int _y, int _w, int _h) override { getVisible().draw(_x, _y, _w, _h); }

  void  setDensity(ofTexture& _inputTex)                        { setInput(_inputTex); }
  void  addDensity(ofTexture& _inputTex, float _strength = 1.0) { addInput(_inputTex, _strength); }

  void  setSaturation(float value) { saturation.set(value); }

  ofTexture& getDensity() { return getOutput(); };

  float  getSaturation(){ return saturation.get(); }

protected:
  ofParameter<float> saturation;

  ftDensityBridgeShader densityBridgeShader;
  ofFbo visibleFbo;

  ftHSVShader HSVShader;

  void allocate(int _velocityWidth, int _velocityHeight, GLint _velocityInternalFormat, int _inOutputWidth, int _inOutputHeight, GLint _inOutputInternalFormat) override {
    ftBridgeFlow::allocate(_velocityWidth, _velocityHeight, _velocityInternalFormat, _inOutputWidth, _inOutputHeight, _inOutputInternalFormat);

    visibleFbo.allocate(_inOutputWidth, _inOutputHeight, GL_RGBA);
    ftUtil::zero(visibleFbo);
  }
};
}

