
#pragma once

#include "ofMain.h"
#include "ftSvVisualizeScalarShader.h"
#include "ftSvVisualizeFieldShader.h"

namespace flowTools {
	
	class ftSvVisualizeField : public ftVisualizeField{
	public:
		
		void drawScalar(ofTexture _tex, int _x, int _y, int _width, int _height) override {
			ftUtil::zero(displayScalarFbo);
			displayScalarShader.update(displayScalarFbo, _tex, pScale.get());
			displayScalarFbo.draw(_x, _y, _width, _height);
		}
		
		void drawField(ofTexture _tex, int _x, int _y, int _width, int _height) override {
			ofPushMatrix();
			ofTranslate(_x, _y);
			ofScale(_width, _height);
			int numChannels = ftUtil::getNumChannelsFromInternalFormat(_tex.getTextureData().glInternalFormat);
			if (numChannels == 1) {
				float barHeight = _height / (float)fieldHeight;
				float barWidth = _width / (float)fieldWidth * 0.25;
				temperatureFieldShader.update(fieldVbo, _tex, pScale.get(), barHeight, barWidth);
			} else {
				float arrowLength = _width / (float)fieldWidth;
				velocityFieldShader.update(fieldVbo, _tex, (1.0 / _width) * arrowLength * pScale.get(), fieldColor);
			}
			ofPopMatrix();
		}
		
	protected:
		
		ftSvVisualizeScalarShader	displayScalarShader;
		ftSvVisualizeFieldShader 	velocityFieldShader;
		
	};
}

