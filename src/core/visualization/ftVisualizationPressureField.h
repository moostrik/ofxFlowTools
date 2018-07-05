
#pragma once

#include "ofMain.h"
#include "ftPressureFieldShader.h"

namespace flowTools {
	
	class ftVisualizationPressureField : public ftVisualizationField{
	public:
		void drawField(ofTexture _tex, int _x, int _y, int _width, int _height) override {
			ofPushMatrix();
			ofTranslate(_x, _y);
			ofScale(_width, _height);
			float radius = _width / (float)fieldWidth * 0.01;
			pressureFieldShader.update(fieldVbo, _tex, pScale.get() * 0.01, radius);
			ofPopMatrix();
		}
		
	protected:
		ftPressureFieldShader pressureFieldShader;
	};
}

