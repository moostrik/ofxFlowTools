#pragma once

#include "ofMain.h"
#include "ofxFlowTools.h"

#include "ftBridgeFlow.h"
#include "ftColorBridgeShader.h"
#include "ftHSVShader.h"
#include "ftRGB2LuminanceShader.h"


namespace flowTools {

class ftColorBridgeFlow : public ftBridgeFlow {
public:
  ftColorBridgeFlow() {
    parameters.setName("density bridge");
    parameters.add(saturation.set("saturation", 2.5, 0.0, 5.0));
    parameters.add(lowColor.set(  "low Color",  ofFloatColor(0,0,0,0), ofFloatColor(0,0,0,0), ofFloatColor(1,1,1,1)));
    parameters.add(midColor.set(  "mid Color",  ofFloatColor(0,0,0,0), ofFloatColor(0,0,0,0), ofFloatColor(1,1,1,1)));
    parameters.add(highColor.set( "high Color", ofFloatColor(1,1,1,1), ofFloatColor(0,0,0,0), ofFloatColor(1,1,1,1)));
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

    multiplyShader.update(outputFbo.get(), velocityTrailFbo.getTexture(), 1.0);

    colorBridgeShader.update(visibleFbo, outputFbo.getTexture(), lowColor.get(), midColor.get(), highColor.get(), 1);
    multiplyShader.update(densityFbo, visibleFbo.getTexture(), speed.get());

    if (saturation.get() != 1.0) {
      outputFbo.swap();
      HSVShader.update(outputFbo.get(), outputFbo.getBackTexture(), 0, saturation.get(), 1.0);
    }

    ofPopStyle();
  }

  void reset() override {
    ftBridgeFlow::reset();
    ftUtil::zero(visibleFbo);
  }

  ofTexture&  getVisible() {
    ofPushStyle();
    ofEnableBlendMode(OF_BLENDMODE_DISABLED);
    multiplyShader.update(visibleFbo, densityFbo.getTexture(), ofGetFrameRate());
    ofPopStyle();
    return visibleFbo.getTexture();
  }

  void  drawInput(int _x, int _y, int _w, int _h) override { inputFbo.draw(_x, _y, _w, _h); }
//  void  drawOutput(int _x, int _y, int _w, int _h) override { getVisible().draw(_x, _y, _w, _h); }
  void  drawDensity(int _x, int _y, int _w, int _h) { densityFbo.draw(_x, _y, _w, _h); }
  void  drawVisible(int _x, int _y, int _w, int _h) { visibleFbo.draw(_x, _y, _w, _h); }
  void  drawVelocity(int _x, int _y, int _w, int _h) { ftBridgeFlow::draw(_x, _y, _w, _h); }

  void  setLowColor(ofFloatColor color)   { lowColor.set(color); }
  void  setMidColor(ofFloatColor color)   { midColor.set(color); }
  void  setHighColor(ofFloatColor color)  { highColor.set(color); }
  ofFloatColor getLowColor()   { return lowColor; }
  ofFloatColor getMidColor()   { return midColor; }
  ofFloatColor getHighColor()  { return highColor; }

  void  setSaturation(float value) { saturation.set(value); }
  float getSaturation(){ return saturation.get(); }

  ofTexture& getDensity() { return densityFbo.getTexture(); };


protected:
  ofParameter<float> saturation;
  ofParameter<ofFloatColor> lowColor;
  ofParameter<ofFloatColor> midColor;
  ofParameter<ofFloatColor> highColor;

  ftColorBridgeShader colorBridgeShader;


  ofFbo densityFbo;
  ofFbo visibleFbo;

  ftHSVShader HSVShader;

  void allocate(int _velocityWidth, int _velocityHeight, GLint _velocityInternalFormat, int _inOutputWidth, int _inOutputHeight, GLint _inOutputInternalFormat) override {
    ftBridgeFlow::allocate(_velocityWidth, _velocityHeight, _velocityInternalFormat, _velocityWidth, _velocityHeight, _velocityInternalFormat);

    densityFbo.allocate(_inOutputWidth, _inOutputHeight, GL_RGBA);
    visibleFbo.allocate(_inOutputWidth, _inOutputHeight, GL_RGBA);
    ftUtil::zero(visibleFbo);
  }
};
}

