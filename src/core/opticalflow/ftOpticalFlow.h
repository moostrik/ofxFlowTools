
#pragma once

#include "ofMain.h"
#include "ofxFlowTools.h"

#include "ftFlow.h"
#include "ftOpticalFlowShader.h"
#include "ftRGB2LuminanceShader.h"

namespace flowTools {

class ftOpticalFlow : public ftFlow {
public:
  void setup(int _width, int _height) {
    allocate(_width, _height, GL_R8, _width, _height, GL_RG32F);

    bFirstFrame = true;
    bInputSet = false;

    parameters.setName("optical flow");
    offset.set("offset", 3, 1, 10);
    threshold.set("threshold", 0.1, 0, 0.2);
    strength.set("force", 3, .1, 10);      // 3 is best for normalization
    boost.set("boost", 0.0, 0.0, .9);      //
    doInverseX.set("inverse x", true);       // flow velocity is inverse to fluid velocity
    doInverseY.set("inverse y", true);       // flow velocity is inverse to fluid velocity

    //    parameters.add(offset);
    parameters.add(threshold);
    //    parameters.add(strength);
    parameters.add(boost);
    //    parameters.add(doInverseX);
    //    parameters.add(doInverseY);
  };

  void update() {
    if (bInputSet) {
      bInputSet = false;

      ofPushStyle();
      ofEnableBlendMode(OF_BLENDMODE_DISABLED);

      opticalFlowFbo.swap();
      ftUtil::stretch(opticalFlowFbo.get(), inputFbo.getTexture());

      if (bFirstFrame) {
        bFirstFrame = false;
        opticalFlowFbo.swap();
        ftUtil::stretch(opticalFlowFbo.get(), opticalFlowFbo.getBackTexture());
      }

      opticalFlowShader.update(outputFbo.get(), opticalFlowFbo.getTexture(), opticalFlowFbo.getBackTexture(), offset.get(), threshold.get(),  glm::vec2(strength.get()),  1.0 - boost.get(), doInverseX.get(), doInverseY.get());

      ofPopStyle();
    }
  }

  void setInput(ofTexture& _tex) override {
    ofPushStyle();
    ofEnableBlendMode(OF_BLENDMODE_DISABLED);
    ftUtil::zero(inputFbo.get());
    if (_tex.getTextureData().glInternalFormat != GL_R8) {
      RGB2LumShader.update(inputFbo.get(), _tex); }
    else {
      ftUtil::stretch(inputFbo.get(), _tex);
    }
    bInputSet = true;

    ofPopStyle();
  }

  void addInput(ofTexture& _tex , float _strength = 1.0) override {
    inputFbo.swap();
    if (_tex.getTextureData().glInternalFormat != GL_R8) {
      RGB2LumShader.update(RGB2LumFbo, _tex);
      addMultipliedShader.update(inputFbo.get(), inputFbo.getBackTexture(), RGB2LumFbo.getTexture(), 1.0, _strength);
    } else {
      addMultipliedShader.update(inputFbo.get(), inputFbo.getBackTexture(), _tex, 1.0, _strength);
    }
    bInputSet = true;
    //  ofLogWarning("ftOpticalFlow: addInput") << " to the optical flow input can only be set";
  }

  void drawInput(int _x, int _y, int _w, int _h) override { inputFbo.draw(_x, _y, _w, _h); }

  void reset() override { ftFlow::reset(); bFirstFrame = true; bInputSet = false; }

  void  setStrength(float value)  {strength.set(value);}
  void  setBoost(float value)     {boost.set(value);}
  void  setOffset(int value)      {offset.set(value);}
  void  setThreshold(float value) {threshold.set(value);}
  void  setInverseX(bool value)   {doInverseX.set(value);}
  void  setInverseY(bool value)   {doInverseY.set(value);}

  ofTexture&  getVelocity()  { return getOutput(); }

  float getStrength()   {return strength.get();}
  float getBoost()      {return boost.get();}
  int   getOffset()     {return offset.get();}
  float getThreshold()  {return threshold.get();}
  bool  getInverseX()   {return doInverseX.get();}
  bool  getInverseY()   {return doInverseY.get();}

protected:
  ofParameter<float>  threshold;
  ofParameter<int>    offset;
  ofParameter<float>  strength;
  ofParameter<float>  boost;
  ofParameter<bool>   doInverseX;
  ofParameter<bool>   doInverseY;

  bool                  bInputSet;
  bool                  bFirstFrame;
  ftOpticalFlowShader   opticalFlowShader;
  ftRGB2LuminanceShader RGB2LumShader;

  ofFbo         RGB2LumFbo;
  ftPingPongFbo opticalFlowFbo;

  void allocate(int _inputWidth, int _inputHeight, GLint _inputInternalFormat, int _outputWidth, int _outputHeight, GLint _outputInternalFormat) override{
    ftFlow::allocate(_inputWidth, _inputHeight, GL_R8, _inputWidth, _inputHeight, GL_RG32F);
    opticalFlowFbo.allocate(_inputWidth, _inputHeight, GL_R8);
    ftUtil::zero(opticalFlowFbo);
    RGB2LumFbo.allocate(_inputWidth, _inputHeight, GL_R8);
    ftUtil::zero(RGB2LumFbo);
    bFirstFrame = true;
    bInputSet = false;
  }
};
}
