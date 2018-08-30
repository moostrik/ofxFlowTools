
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {
	
	class ftClampLengthShader : public ftShader {
	public:
		ftClampLengthShader() {
			bInitialized = true;
			if (ofIsGLProgrammableRenderer()) { glThree(); } else { glTwo(); }
			string shaderName = "ftClampLengthShader";
			if (bInitialized) { ofLogVerbose(shaderName + " initialized"); }
			else { ofLogWarning(shaderName + " failed to initialize"); }
		}
		
	protected:
		void glTwo() {
			fragmentShader = GLSL120(
									 uniform sampler2DRect Backbuffer;
									 uniform float MaxLength;
									 uniform float ClampForce;
									 
									 void main(){
										 vec2 st = gl_TexCoord[0].st;
										 
										 vec4 color = texture2DRect(Backbuffer, st);
										 
										 float l = length(color.xyz);
										 if (l > MaxLength) {
											 float dinges = (l - MaxLength) * ClampForce;
											 color.xyz += TINY;
											 color.xyz = normalize(color.xyz) * (l - dinges);
										 }
										 gl_FragColor = color ;
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= linkProgram();
		}
		
		void glThree() {
			fragmentShader = GLSL150(
									 uniform sampler2DRect Backbuffer;
									 uniform float MaxLength;
									 uniform float ClampForce;
									 
									 in vec2 texCoordVarying;
									 out vec4 fragColor;
									 
									 void main(){
										 vec2 st = texCoordVarying;
										 
										 vec4 color = texture(Backbuffer, st);
										 
										 float l = length(color.xyz);
										 if (l > MaxLength) {
											 float dinges = (l - MaxLength) * ClampForce;
											 color.xyz += TINY;
											 color.xyz = normalize(color.xyz) * (l - dinges);
										 }
										 fragColor = color ;
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= bindDefaults();
			bInitialized *= linkProgram();
		}
		
	public:
		void update(ofFbo& _fbo, ofTexture& _backTex, float _max, float _ClampForce){
			_fbo.begin();
			begin();
			setUniformTexture("Backbuffer", _backTex, 0);
			setUniform1f("MaxLength", _max);
			setUniform1f("ClampForce", _ClampForce);
			renderFrame(_fbo.getWidth(), _fbo.getHeight());
			end();
			_fbo.end();
		}
	};
}


