
#pragma once

#include "ofMain.h"
#include "ftShader.h"


namespace flowTools {
	
	class ftAddMultipliedShader : public ftShader {
	public:
		ftAddMultipliedShader() {
            bInitialized = 1;
            if (ofIsGLProgrammableRenderer()) { glThree(); } else { glTwo(); }
			
			if (bInitialized)
				ofLogVerbose("ftAddMultipiedShader initialized");
			else
				ofLogWarning("ftAddMultipiedShader failed to initialize");
		}
		
	protected:
		void glTwo() {
			fragmentShader = GLSL120(
									 uniform sampler2DRect baseTex;
									 uniform sampler2DRect blendTex;
									 uniform float	baseWeight;
									 uniform float	blendWeight;
									 uniform vec2	baseScale;
									 uniform vec2	blendScale;
									 
									 void main(){
										 vec2 baseSt = gl_TexCoord[0].st * baseScale;
										 vec2 blendSt = gl_TexCoord[0].st * blendScale;
										 vec4 base = texture2DRect(baseTex, baseSt) * baseWeight;
										 vec4 blend = texture2DRect(blendTex, blendSt) * blendWeight;
										 gl_FragColor = base + blend ;
									 }
									 
									 );
			
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.linkProgram();
		}
		
		void glThree() {
			fragmentShader = GLSL150(
									 uniform sampler2DRect baseTex;
									 uniform sampler2DRect blendTex;
									 uniform float	baseWeight;
									 uniform float	blendWeight;
									 uniform vec2	baseScale;
									 uniform vec2	blendScale;
									 
									 in vec2 texCoordVarying;
									 out vec4 fragColor;
									 
									 void main(){
										 vec2 baseSt = texCoordVarying * baseScale;
										 vec2 blendSt = texCoordVarying * blendScale;
										 vec4 base = texture(baseTex, baseSt) * baseWeight;
										 vec4 blend = texture(blendTex, blendSt) * blendWeight;
										 fragColor = base + blend;
									 }
									 );
			
			bInitialized *= shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.bindDefaults();
			bInitialized *= shader.linkProgram();
		}
		
	public:
//		void update(ftPingPongFbo& _fbo, ofTexture& _blendTex, float _baseMultiplier, float _blendMultiplier) {
//			update(_fbo, _fbo.getBackTexture(), _blendTex, _baseMultiplier, _blendMultiplier);
//		}
		
		
		void update(ofFbo& _fbo, ofTexture& _baseTex, ofTexture& _blendTex, float _baseMultiplier, float _blendMultiplier){
			_fbo.begin();
			shader.begin();
			shader.setUniformTexture("baseTex", _baseTex, 0);
			shader.setUniformTexture("blendTex", _blendTex, 1);
			shader.setUniform1f("baseWeight", _baseMultiplier);
			shader.setUniform1f("blendWeight", _blendMultiplier);
			shader.setUniform2f("baseScale", _baseTex.getWidth() / _fbo.getWidth(), _baseTex.getHeight() / _fbo.getHeight());
			shader.setUniform2f("blendScale", _blendTex.getWidth() / _fbo.getWidth(), _blendTex.getHeight() / _fbo.getHeight());
			renderFrame(_fbo.getWidth(), _fbo.getHeight());
			shader.end();
			_fbo.end();
		}
	};
}
