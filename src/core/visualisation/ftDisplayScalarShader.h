
#pragma once

#include "ofMain.h"
#include "ftShader.h"


namespace flowTools {
	
	class ftDisplayScalarShader : public ftShader {
	public:
		ftDisplayScalarShader() {
			bInitialized = 1;
			
			if (ofIsGLProgrammableRenderer())
				glThree();
			else
				glTwo();
			
			if (bInitialized)
				ofLogNotice("ftDisplayScalarShader initialized");
			else
				ofLogWarning("ftDisplayScalarShader failed to initialize");
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
			
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.linkProgram();
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
			
			bInitialized *= shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.bindDefaults();
			bInitialized *= shader.linkProgram();
			
		}
	
	public:
		void update(ofFbo& _buffer, ofTexture& _floatTexture, float _scale){
			_buffer.begin();
			shader.begin();
			shader.setUniformTexture("FloatTexture", _floatTexture, 0);
			shader.setUniform1f("Scale", _scale);
			renderFrame(_buffer.getWidth(), _buffer.getHeight());
			shader.end();
			_buffer.end();
		}
	};
}