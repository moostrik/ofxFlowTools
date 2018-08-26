
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {
	
	class ftAddMultipliedShader : public ftShader {
	public:
		ftAddMultipliedShader() {
			bInitialized = true;
			if (ofIsGLProgrammableRenderer()) { glThree(); } else { glTwo(); }
			string shaderName = "ftAddMultipiedShader";
			if (bInitialized) { ofLogVerbose(shaderName + " initialized"); }
			else { ofLogWarning(shaderName + " failed to initialize"); }
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
			
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= linkProgram();
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
			
			bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= bindDefaults();
			bInitialized *= linkProgram();
		}
		
	public:
		void update(ofFbo& _fbo, ofTexture& _baseTex, ofTexture& _blendTex, float _baseMultiplier, float _blendMultiplier){
			_fbo.begin();
			begin();
			setUniformTexture("baseTex", _baseTex, 0);
			setUniformTexture("blendTex", _blendTex, 1);
			setUniform1f("baseWeight", _baseMultiplier);
			setUniform1f("blendWeight", _blendMultiplier);
			setUniform2f("baseScale", _baseTex.getWidth() / _fbo.getWidth(), _baseTex.getHeight() / _fbo.getHeight());
			setUniform2f("blendScale", _blendTex.getWidth() / _fbo.getWidth(), _blendTex.getHeight() / _fbo.getHeight());
			renderFrame(_fbo.getWidth(), _fbo.getHeight());
			end();
			_fbo.end();
		}
	};
}

