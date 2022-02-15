#pragma once

#include "ofMain.h"
#include "ofxFlowTools.h"

#include "ftBridgeFlow.h"
#include "ftDensityBridgeShader.h"
#include "ftHSVShader.h"
#include "ftRGB2LuminanceShader.h"


namespace flowTools {

class ftCombinedBridgeFlow : public ftBridgeFlow {
public:
  ftCombinedBridgeFlow() {
    parameters.setName("combined bridge");
    speed.setName("velocity speed");
    parameters.add(densitySpeed.set("density speed"           , 0.3, 0.0, 1.0));
    parameters.add(densitySaturation.set("density saturation" , 2.5, 0.0, 5.0));
    parameters.add(temperatureSpeed.set("temperature speed"   , 0.5, -1., 1.0));
  }

  void setup(int _simulationWidth, int _simulationHeight) { setup(_simulationWidth, _simulationHeight, _simulationWidth, _simulationHeight); }

  void setup(int _simulationWidth, int _simulationHeight, int _densityWidth, int _densityHeight){
    allocate(_simulationWidth, _simulationHeight, GL_RG32F, _densityWidth, _densityHeight, GL_RGBA32F);
  };

  void update(float _deltaTime) override {
    ftBridgeFlow::update(_deltaTime);

    ofPushStyle();
    ofEnableBlendMode(OF_BLENDMODE_DISABLED);
    resetOutput();

    float velocityTimeStep = _deltaTime * speed.get() * 200;
    multiplyShader.update(velocityFbo, velocityTrailFbo.getTexture(), velocityTimeStep);

    float densityTimeStep = _deltaTime * densitySpeed.get() * 10;
    densityBridgeShader.update(outputFbo.get(), inputFbo.getTexture(), velocityTrailFbo.getTexture(), densityTimeStep);
    outputFbo.swap();
    HSVShader.update(outputFbo.get(), outputFbo.getBackTexture(), 0, densitySaturation.get(), 1.0);

    float temperatureTimeStep = _deltaTime * temperatureSpeed.get() * 10;
    temperatureBridgeShader.update(temperatureFbo.get(), inputFbo.getTexture(), velocityTrailFbo.getTexture(), temperatureTimeStep);

    ofPopStyle();
  }

  void reset() override {
    ftBridgeFlow::reset();
    ftUtil::zero(densityVisibleFbo);
  }

  ofTexture&    getVisible() {
    ofPushStyle();
    ofEnableBlendMode(OF_BLENDMODE_DISABLED);
    multiplyShader.update(densityVisibleFbo, outputFbo.getTexture(), ofGetFrameRate());
    ofPopStyle();
    return densityVisibleFbo.getTexture();
  }

  void    drawInput(int _x, int _y, int _w, int _h) override { inputFbo.draw(_x, _y, _w, _h); }
  void    drawOutput(int _x, int _y, int _w, int _h) override { drawDensity(_x, _y, _w, _h); }

  void    setDensity(ofTexture& _inputTex)                        { setInput(_inputTex); }
  void    addDensity(ofTexture& _inputTex, float _strength = 1.0) { addInput(_inputTex, _strength); }

  void    setSaturation(float value)  { densitySaturation.set(value); }
  float    getSaturation()            { return densitySaturation.get(); }

  ofTexture& getVelocity() override   { return velocityFbo.getTexture(); }
  ofTexture& getDensity()             { return getOutput(); }
  ofTexture& getTemperature()         { return temperatureFbo.getTexture(); }

  void    drawVelocity(int _x, int _y, int _w, int _h)    { visualizationField.draw(velocityFbo.getTexture(), _x, _y, _w, _h); }
  void    drawDensity(int _x, int _y, int _w, int _h)     { getVisible().draw(_x, _y, _w, _h); }
  void    drawTemperature(int _x, int _y, int _w, int _h) { visualizationField.draw(temperatureFbo.getTexture(), _x, _y, _w, _h); }

protected:
  ofParameter<float>        densitySpeed;
  ofParameter<float>        densitySaturation;
  ofParameter<float>        temperatureSpeed;

  ftDensityBridgeShader     densityBridgeShader;
  ftTemperatureBridgeShader temperatureBridgeShader;
  ftHSVShader               HSVShader;

  ofFbo                     velocityFbo;
  ofFbo                     densityVisibleFbo;
  ftPingPongFbo             temperatureFbo;


  void allocate(int _velocityWidth, int _velocityHeight, GLint _velocityInternalFormat, int _inOutputWidth, int _inOutputHeight, GLint _inOutputInternalFormat) override {
    ftBridgeFlow::allocate(_velocityWidth, _velocityHeight, _velocityInternalFormat, _inOutputWidth, _inOutputHeight, _inOutputInternalFormat);
    velocityFbo.allocate(_velocityWidth, _velocityHeight, GL_RG32F);
    densityVisibleFbo.allocate(_inOutputWidth, _inOutputHeight, GL_RGBA);
    temperatureFbo.allocate(_velocityWidth, _velocityHeight, GL_R32F);
    ftUtil::zero(velocityFbo);
    ftUtil::zero(densityVisibleFbo);
    ftUtil::zero(temperatureFbo);

    visualizationField.setup(_velocityWidth, _velocityHeight);
  }
};
}

