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
    speed.set(0.5);
  }

  void setup(int _simulationWidth, int _simulationHeight) {
    allocate(_simulationWidth, _simulationHeight, GL_RG32F, _simulationWidth, _simulationHeight, GL_R32F);
  };

  void update(float deltaTime) override {
    ftBridgeFlow::update(deltaTime);

    ofPushStyle();
    ofEnableBlendMode(OF_BLENDMODE_DISABLED);
    resetOutput();

    temperatureBridgeShader.update(outputFbo.get(), inputFbo.getTexture(), velocityTrailFbo.getTexture(), speed.get());

    ofPopStyle();
  }

  void resize(int _inputWidth, int _inputHeight, int _outputWidth, int _outputHeight) override {
    allocate(_inputWidth, _inputHeight, inputInternalFormat, _inputWidth, _inputHeight, outputInternalFormat);
  }

  void setDensity(ofTexture& _inputTex)                        { setInput(_inputTex); }
  void addDensity(ofTexture& _inputTex, float _strength = 1.0) { addInput(_inputTex, _strength); }

  ofTexture& getTemperature() { return getOutput(); };

protected:
  ftTemperatureBridgeShader temperatureBridgeShader;
  void allocate (int _inputWidth, int _inputHeight, GLint _inputInternalFormat, int _outputWidth, int _outputHeight, GLint _outputInternalFormat) override {
    ftBridgeFlow::allocate(_inputWidth, _inputHeight, _inputInternalFormat, _inputWidth, _inputHeight, _inputInternalFormat);
  }
};
}

