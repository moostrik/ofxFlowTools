
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {
	
	class ftAddBooleanShader : public ftShader {
	public:
		ftAddBooleanShader(){
			bInitialized = true;
			if (ofIsGLProgrammableRenderer()) { glThree(); } else { glTwo(); }
			string shaderName = "ftAddBooleanShader";
			if (bInitialized) { ofLogVerbose(shaderName + " initialized"); }
			else { ofLogWarning(shaderName + " failed to initialize"); }
		}
		
	protected:
		void glTwo() {
			fragmentShader = GLSL120(
									 uniform sampler2DRect baseTex;
									 uniform sampler2DRect blendTex;
									 
									 uniform vec2	baseScale;
									 uniform vec2	blendScale;

									 void main(){
										 vec2 baseSt = gl_TexCoord[0].st * baseScale;
										 vec2 blendSt = gl_TexCoord[0].st * blendScale;
										 vec4 base = texture2DRect(baseTex, baseSt);
										 vec4 blend = clamp(floor(texture2DRect(blendTex, blendSt) + 0.1), 0.0, 1.0);
										 gl_FragColor = base + blend;
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= linkProgram();
		}
		
		void glThree() {
			fragmentShader = GLSL150(
									 uniform sampler2DRect baseTex;
									 uniform sampler2DRect blendTex;
									 
									 uniform vec2	baseScale;
									 uniform vec2	blendScale;
									 
									 in vec2 texCoordVarying;
									 out vec4 fragColor;
									 
									 void main(){
										 vec2 baseSt = texCoordVarying * baseScale;
										 vec2 blendSt = texCoordVarying * blendScale;
										 vec4 base = texture(baseTex, baseSt);
										 vec4 blend = clamp(round(texture(blendTex, blendSt)), 0.0, 1.0);
										 fragColor = base + blend;
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= bindDefaults();
			bInitialized *= linkProgram();
		}
		
	public:
		void update(ofFbo& _fbo, ofTexture& _baseTex, ofTexture& _blendTex){
			_fbo.begin();
			begin();
			setUniformTexture("baseTex", _baseTex, 0);
			setUniformTexture("blendTex", _blendTex, 1);
			setUniform2f("baseScale", _baseTex.getWidth() / _fbo.getWidth(), _baseTex.getHeight() / _fbo.getHeight());
			setUniform2f("blendScale", _blendTex.getWidth() / _fbo.getWidth(), _blendTex.getHeight() / _fbo.getHeight());
			renderFrame(_fbo.getWidth(), _fbo.getHeight());
			end();
			_fbo.end();
		}
	};
}


