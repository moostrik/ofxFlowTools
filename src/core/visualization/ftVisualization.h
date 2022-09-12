
#pragma once

#include "ofMain.h"
#include "ftUtil.h"
#include "ftToScalarShader.h"

namespace flowTools {

class ftVisualization {
public:
  ftVisualization() {
    parameters.setName("visualize");
    parameters.add(pScale.set("scale", 0.3, 0.1, 3.0));
  }

  virtual void setup(int _width, int _height){
    setupScalar(scalarWidth, scalarHeight);
  }

  virtual void setupScalar(int _width, int _height) {
    scalarWidth = _width;
    scalarHeight = _height;
    scalarFbo.allocate(scalarWidth, scalarHeight, GL_RGBA);
  }

  virtual void draw(ofTexture _tex, int _x, int _y, int _width, int _height) {
    drawScalar(_tex, _x, _y, _width, _height);
  }

  virtual void drawScalar(ofTexture _tex, int _x, int _y, int _width, int _height) {
    //      ftUtil::zero(scalarFbo);
    toScalarShader.update(scalarFbo, _tex, pScale.get());
    scalarFbo.draw(_x, _y, _width, _height);
  }

  virtual void reload() { toScalarShader.loadFromFile(); }

  void  setScale(float _value)  { pScale.set(_value); }
  float getScale()              { return pScale.get(); }

  int   getWidth()              { return getScalarWidth(); }
  int   getHeight()             { return getScalarHeight(); }
  int   getScalarWidth()        { return scalarWidth; }
  int   getScalarHeight()       { return scalarHeight; }

  ofParameterGroup&  getParameters() { return parameters; }

protected:
  ofParameterGroup   parameters;
  ofParameter<float>  pScale;

  int          scalarWidth;
  int          scalarHeight;

  ofFbo        scalarFbo;
  ftToScalarShader  toScalarShader;

};
}

