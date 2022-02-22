
#pragma once

#include "ofMain.h"
#include "ftVisualizationField.h"

namespace flowTools {

class ftVisualizationTemperatureField : public ftVisualizationField{
public:
  void drawField(ofTexture _tex, int _x, int _y, int _width, int _height) override {
    ofPushMatrix();
    ofTranslate(_x, _y);
    ofScale(_width, _height);
    float barHeight = _height / (float)fieldHeight;
    float barWidth = _width / (float)fieldWidth * 0.25;
    temperatureFieldShader.update(fieldVbo, _tex, pScale.get() * 0.1, barHeight, barWidth);
    ofPopMatrix();
  }

};
}

