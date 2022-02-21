
#pragma once

#include "ofMain.h"
#include "ftUtil.h"
#include "ftFlow.h"

namespace flowTools {

class ftPixelFlow : public ftFlow {
public:
  void setup(int _width, int _height, ftFlowForceType _type) ;

  void update();

  ofFloatPixels& getPixels() { return floatPixels; }

  void setRoi(float _x, float _y, float _width, float _height) { setRoi(ofRectangle(_x, _y, _width, _height)); }
  void setRoi(ofRectangle _rect);

  ofRectangle  getRoi()   { return roi; }
  ofPoint getRoiCentre()  { return ofPoint(roi.x + roi.width / 2, roi.y + roi.height / 2); }

  int getWidth()  { return floatPixels.getWidth(); }
  int getHeight() { return floatPixels.getHeight(); }

  void resize(int _inputWidth, int _inputHeight, int _outputWidth, int _outputHeight) override { resize(_inputWidth, _inputHeight); }
  void resize(int _width, int _height) override { allocate(_width, _height, inputInternalFormat, _width * roi.width, _height * roi.height, outputInternalFormat); }

protected:
  ftFlowForceType type;
  int             numChannels;
  ofFloatPixels   floatPixels;
  bool inputSet, pixelsSet;
  ofParameterGroup            roiParameters;
  ofRectangle                 roi;
  vector<ofParameter<float>>  pRoi;
  ofParameter<int>            pDuration;
  void pRoiListener(float& _value)  { setRoi(pRoi[0], pRoi[1], pRoi[2], pRoi[3]); }

  void allocate(int _inputWidth, int _inputHeight, GLint _inputInternalFormat, int _outputWidth, int _outputHeight, GLint _outputInternalFormat) override;

};
}
