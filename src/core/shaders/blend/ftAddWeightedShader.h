
#pragma once

#include "ofMain.h"
#include "ftAddMultipliedShader.h"


namespace flowTools {
	
	class ftAddWeightedShader : public ftAddMultipliedShader {
	public:
		void update(ofFbo& _fbo, ofTexture& _baseTex, ofTexture& _blendTex, float _weight){
			_fbo.begin();
			shader.begin();
			shader.setUniformTexture("_baseTex", _baseTex, 0);
			shader.setUniformTexture("_blendTex", _blendTex, 1);
			shader.setUniform1f("baseWeight", 1.0 - _weight);
			shader.setUniform1f("blendWeight", _weight);
			shader.setUniform2f("scale", _addTex.getWidth() / _fbo.getWidth(), _addTex.getHeight()/ _fbo.getHeight());
			renderFrame(_fbo.getWidth(), _fbo.getHeight());
			shader.end();
			_fbo.end();
		}
	};
}
