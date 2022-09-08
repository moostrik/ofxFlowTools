
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

  void update() override {
    ftBridgeFlow::update();

    ofPushStyle();
    ofEnableBlendMode(OF_BLENDMODE_DISABLED);
    resetOutput();
    multiplyShader.update(outputFbo.get(), velocityTrailFbo.getTexture(), speed.get());
    ofPopStyle();
  }

  void setInput(ofTexture &_texture) override { setVelocity(_texture); }
  ofTexture& getInput() override { return velocityInputFbo.getTexture(); }

protected:
  ftMultiplyForceShader multiplyShader;
};
}
