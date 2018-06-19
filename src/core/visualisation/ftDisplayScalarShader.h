
#pragma once

#include "ofMain.h"
#include "ftShader.h"


namespace flowTools {
	
	class ftDisplayScalarShader : public ftShader {
	public:
		ftDisplayScalarShader() {
			bInitialized = 1;
			if (ofIsGLProgrammableRenderer()) { glThree(); } else { glTwo(); }
			string shaderName = "ftDisplayScalarShader";
			if (bInitialized) { ofLogVerbose(shaderName + " initialized"); }
			else { ofLogWarning(shaderName + " failed to initialize"); }
		}
		
	protected:
		void glTwo() {
			fragmentShader = GLSL120(
									 uniform sampler2DRect FloatTexture;
									 uniform float Scale;
									 void main(){
										 vec4	velocity = texture2DRect(FloatTexture, gl_TexCoord[0].st);
										 velocity.xyz *= vec3(Scale);
										 velocity.w = pow(length(velocity.xyz), 0.33);
										 velocity.xyz += vec3(0.5);
										 gl_FragColor = velocity;
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= linkProgram();
		}
		
		void glThree() {
			
			fragmentShader = GLSL150(
									 uniform sampler2DRect FloatTexture;
									 uniform float Scale;
									 
									 in vec2 texCoordVarying;
									 out vec4 fragColor;
									 
									 void main(){
										 vec2 st = texCoordVarying;
										 vec4	velocity = texture(FloatTexture, st);
										 velocity.xyz *= vec3(Scale);
										 velocity.w = pow(length(velocity.xyz), 0.33);
										 velocity.xyz += vec3(0.5);
										 fragColor = velocity;
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= bindDefaults();
			bInitialized *= linkProgram();
			
		}
	
	public:
		void update(ofTexture& _floatTex, int _w, int _h, float _scale){
			begin();
			setUniformTexture("FloatTexture", _floatTex, 0);
			setUniform1f("Scale", _scale);
			renderFrame(_w, _h);
			end();
		}
			
		void update(ofFbo& _fbo, ofTexture& _floatTex, float _scale){
			begin();
			setUniformTexture("FloatTexture", _floatTex, 0);
			setUniform1f("Scale", _scale);
			_fbo.draw(0,0);
			end();
		}
	};
}
