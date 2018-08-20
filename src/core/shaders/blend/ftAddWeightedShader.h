
#pragma once

#include "ofMain.h"
#include "ftAddMultipliedShader.h"


namespace flowTools {
	
	class ftAddWeightedShader : public ftAddMultipliedShader {
	public:
		void update(ofFbo& _fbo, ofTexture& _baseTex, ofTexture& _blendTex, float _weight){
			_fbo.begin();
			begin();
			setUniformTexture("_baseTex", _baseTex, 0);
			setUniformTexture("_blendTex", _blendTex, 1);
			setUniform1f("baseWeight", 1.0 - _weight);
			setUniform1f("blendWeight", _weight);
			setUniform2f("scale", _addTex.getWidth() / _fbo.getWidth(), _addTex.getHeight()/ _fbo.getHeight());
			renderFrame(_fbo.getWidth(), _fbo.getHeight());
			end();
			_fbo.end();
		}
	};
}
