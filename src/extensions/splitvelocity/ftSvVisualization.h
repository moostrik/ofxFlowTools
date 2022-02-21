
#pragma once

#include "ofMain.h"
#include "ftSvToScalarShader.h"

namespace flowTools {

class ftSvVisualization : public ftVisualization {
public:

  void drawScalar(ofTexture _tex, int _x, int _y, int _width, int _height) override {
    ftUtil::zero(scalarFbo);
    toScalarShader.update(scalarFbo, _tex, pScale.get());
    scalarFbo.draw(_x, _y, _width, _height);
  }

protected:
  ftSvToScalarShader	toScalarShader;
};
}

