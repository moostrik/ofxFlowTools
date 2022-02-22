
#pragma once

#include "ofMain.h"
#include "ftVelocityDotsShader.h"
#include "ftVisualizationField.h"

namespace flowTools {

class ftVisualizationVelocityDots : public ftVisualizationField{
public:
  ftVisualizationVelocityDots() {
    parameters.setName("velocity dots");
    parameters.add(distanceScale.set("distance scale", .1, 0, 1));
    parameters.add(minDotSize.set("min dot size", 2, 0, 20));
    parameters.add(maxDotSize.set("max dot size", 10, 0, 20));
  }


  void setupField(int _width, int _height) override {
    fieldWidth = _width;
    fieldHeight = _height;

    fieldMesh.clear();
    fieldMesh.setMode(OF_PRIMITIVE_POINTS);
    float xStep = 1. / fieldWidth;
    float yStep = 1. / fieldHeight;
    for (int x=0; x<fieldWidth; x++) {
      for (int y=0; y<fieldHeight; y++) {
        fieldMesh.addVertex(glm::vec3((x + 0.5) * xStep, (y + 0.5) * yStep, 0));
        fieldMesh.addTexCoord(glm::vec2((x + 0.5) * xStep, (y + 0.5) * yStep));
        fieldMesh.addColor(ofFloatColor(1,1,1,1));
      }
    }

    fieldVbo.clear();
    fieldVbo.setMesh(fieldMesh, GL_DYNAMIC_DRAW, true, true, false);
  }

  void drawField(ofTexture _tex, int _x, int _y, int _width, int _height) override {
    ofPushMatrix();
    ofTranslate(_x, _y);
    ofScale(_width, _height);
    float arrowLength = _width / (float)fieldWidth;

    velocityDotsShader.update(fieldVbo, _tex, distanceScale.get(), pScale.get(), minDotSize.get(), maxDotSize.get());
    ofPopMatrix();
  }

protected:
  ofParameter<float>  distanceScale;
  ofParameter<float>  minDotSize;
  ofParameter<float>  maxDotSize;    // scale to normalize velocity
  ftVelocityDotsShader velocityDotsShader;
};
}

