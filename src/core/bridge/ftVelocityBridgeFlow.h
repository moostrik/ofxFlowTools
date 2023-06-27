
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
    ftBridgeFlow::allocate(_width, _height, GL_RG32F, _width, _height, GL_RG32F);
  };

  void update(float deltaTime) override {
    ftBridgeFlow::update(deltaTime);

    ofPushStyle();
    ofEnableBlendMode(OF_BLENDMODE_DISABLED);
    resetOutput();
    multiplyShader.update(outputFbo.get(), velocityTrailFbo.getTexture(), speed.get());
    ofPopStyle();
  }

  void resize(int _inputWidth, int _inputHeight, int _outputWidth, int _outputHeight) override {
    allocate(_inputWidth, _inputHeight, inputInternalFormat, _inputWidth, _inputHeight, outputInternalFormat);
  }

  void setInput(ofTexture &_texture) override { setVelocity(_texture); }
  ofTexture& getInput() override { return velocityInputFbo.getTexture(); }

protected:
  ftMultiplyForceShader multiplyShader;
};
}
