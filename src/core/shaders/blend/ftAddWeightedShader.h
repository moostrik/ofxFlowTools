
#pragma once

#include "ofMain.h"
#include "ftAddMultipliedShader.h"


namespace flowTools {
	
	class ftAddWeightedShader : public ftAddMultipliedShader {
	public:
		void update(ofFbo& _buffer, ofTexture& _baseTexture, ofTexture& _blendTexture, float _weight){
			_buffer.begin();
			shader.begin();
			shader.setUniformTexture("_baseTexture", _baseTexture, 0);
			shader.setUniformTexture("_blendTexture", _blendTexture, 1);
			shader.setUniform1f("baseWeight", 1.0 - _weight);
			shader.setUniform1f("blendWeight", _weight);
			shader.setUniform2f("scale", _addTexture.getWidth() / _buffer.getWidth(), _addTexture.getHeight()/ _buffer.getHeight());
			renderFrame(_buffer.getWidth(), _buffer.getHeight());
			shader.end();
			_buffer.end();
		}
	};
}
