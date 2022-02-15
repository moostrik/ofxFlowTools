
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

  void update(float _deltaTime) override {
    ftBridgeFlow::update(_deltaTime);

    ofPushStyle();
    ofEnableBlendMode(OF_BLENDMODE_DISABLED);
    resetOutput();

    float timeStep = _deltaTime * speed.get() * 200;
    multiplyShader.update(outputFbo.get(), velocityTrailFbo.getTexture(), timeStep);

    ofPopStyle();
  }

  ofTexture& getVelocity() override { return getOutput(); }
  void setInput(ofTexture &_texture) override { setVelocity(_texture); }

protected:
  ftMultiplyForceShader multiplyShader;

  void allocate (int _inputWidth, int _inputHeight, GLint _inputInternalFormat, int _outputWidth, int _outputHeight, GLint _outputInternalFormat) override {
    ftBridgeFlow::allocate(_inputWidth, _inputHeight, _inputInternalFormat, _inputWidth, _inputHeight, _inputInternalFormat);
  }
};
}
