
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {
	
	class ftColorizeVelocityShader  : public ftShader {
	public:
		ftColorizeVelocityShader() {
            bInitialized = 1;
			if (ofIsGLProgrammableRenderer()) { glFour(); } else { glTwo(); }
			string shaderName = "ftColorizeVelocityShader";
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
		
		void glFour() {
			fragmentShader = GLSL410(
									 uniform sampler2DRect FloatTexture;
									 uniform sampler2DRect GradientTexture;
									 
									 uniform float Force;
									 uniform vec2 Scale;
									 uniform vec2 GradientDimensions;
									 
									 in vec2 texCoordVarying;
									 out vec4 fragColor;
									 
									 void main(){
										 vec2 st = texCoordVarying;
										 vec2 st2 = st * Scale;
										 
										 vec4	velocity = texture(FloatTexture, st2);
										 velocity.xyz *= vec3(Force);
										 //										 velocity.w = pow(length(velocity.xyz), 0.33);
										 //										 velocity.xyz += vec3(0.5);
										 
										 vec2 hGD = GradientDimensions / vec2(2.0, 2.0);
										 vec2 Gst = hGD + velocity.xy * hGD;
										 
										 fragColor = texture(GradientTexture, Gst);
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= bindDefaults();
			bInitialized *= linkProgram();
		}
		
	public:
		void update(ofFbo& _fbo, ofTexture& _floatTex, ofTexture& _gradientTexture, float _force){
			_fbo.begin();
			begin();
			setUniformTexture("FloatTexture", _floatTex, 0);
			setUniformTexture("GradientTexture", _gradientTexture, 1);
			setUniform2f("Scale", _floatTex.getWidth() / _fbo.getWidth(), _floatTex.getHeight()/ _fbo.getHeight());
			setUniform1f("Force", _force);
			setUniform2f("GradientDimensions", _gradientTexture.getWidth(), _gradientTexture.getHeight());
			renderFrame(_fbo.getWidth(), _fbo.getHeight());
			end();
			_fbo.end();
		}
	};
}

