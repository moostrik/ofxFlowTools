
#pragma once

#include "ofMain.h"
#include "ftSvToScalarShader.h"
#include "ftSvFieldShader.h"

namespace flowTools {
	
	class ftSvVisualizationField : public ftVisualizationField{
	public:
		
		void drawScalar(ofTexture _tex, int _x, int _y, int _width, int _height) override {
			ftUtil::zero(scalarFbo);
			toScalarShader.update(scalarFbo, _tex, pScale.get());
			scalarFbo.draw(_x, _y, _width, _height);
		}
		
		void drawField(ofTexture _tex, int _x, int _y, int _width, int _height) override {
			ofPushMatrix();
			ofTranslate(_x, _y);
			ofScale(_width, _height);
			float arrowLength = _width / (float)fieldWidth;
			fieldShader.update(fieldVbo, _tex, (1.0 / _width) * arrowLength * pScale.get(), fieldColor);
			ofPopMatrix();
		}
		
	protected:
		
		ftSvToScalarShader	toScalarShader;
		ftSvFieldShader 	fieldShader;
		
	};
}

