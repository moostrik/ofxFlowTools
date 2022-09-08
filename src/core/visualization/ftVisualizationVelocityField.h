
#pragma once

#include "ofMain.h"
#include "ftVisualizationField.h"

namespace flowTools {

class ftVisualizationVelocityField : public ftVisualizationField{
public:
  void drawField(ofTexture _tex, int _x, int _y, int _width, int _height) override {
    ofPushMatrix();
    ofTranslate(_x, _y);
    ofScale(_width, _height);
    float arrowLength = _width / (float)fieldWidth;
    float scale = (1.0 / _width) * arrowLength  * pScale.get();
    velocityFieldShader.update(fieldVbo, _tex, scale, fieldColor);
    ofPopMatrix();
  }

};
}

