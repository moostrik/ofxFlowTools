
#pragma once

#include "ofMain.h"
#include "ftShader.h"


namespace flowTools {
	
	class ftDisplayScalarShader : public ftShader {
	public:
		ftDisplayScalarShader() {
			
			if (isProgrammableRenderer)
				glThree();
			else
				glTwo();
		}
		
	protected:
		void glTwo() {
			fragmentShader = GLSL120(
								  uniform sampler2DRect FloatTexture;
									   uniform float Scale;
									   void main(){
										   vec4	velocity = texture2DRect(FloatTexture, gl_TexCoord[0].st);
										   velocity.xyz += vec3(0.5 / Scale);
										   velocity.xyz *= vec3(Scale);
										   velocity.w = 1.0;
										   gl_FragColor = velocity;
									   }
									   );
			
			shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			shader.linkProgram();
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
									  velocity.xyz += vec3(0.5 / Scale);
									  velocity.xyz *= vec3(Scale);
									  velocity.w = 1.0;
									  fragColor = velocity;
								  }
								  );
			
			shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			shader.bindDefaults();
			shader.linkProgram();
			
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