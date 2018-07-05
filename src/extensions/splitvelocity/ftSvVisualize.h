
#pragma once

#include "ofMain.h"
#include "ftSvVisualizeScalarShader.h"

namespace flowTools {
	
	class ftSvVisualize : public ftVisualize {
	public:
		
		void drawScalar(ofTexture _tex, int _x, int _y, int _width, int _height) override {
			ftUtil::zero(displayScalarFbo);
			displayScalarShader.update(displayScalarFbo, _tex, pScale.get());
			displayScalarFbo.draw(_x, _y, _width, _height);
		}
		
	protected:
		ftSvVisualizeScalarShader	displayScalarShader;
	};
}

