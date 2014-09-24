
#pragma once

#include "ofMain.h"
#include "ftShader.h"


namespace flowTools {
	
	class ftClampLengthShader : public ftShader {
	public:
		ftClampLengthShader() {
			
			ofLogVerbose("init ftClampLengthShader");
			if (isProgrammableRenderer)
				glThree();
			else
				glTwo();
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
										  color.xyz = normalize(color.xyz) * (l - dinges);
									  }
									  gl_FragColor = color ;
								  }
								  );
			
			shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			shader.linkProgram();
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
										  color.xyz = normalize(color.xyz) * (l - dinges);
									  }
									  fragColor = color ;
								  }
								  );
			
			shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			shader.bindDefaults();
			shader.linkProgram();
		}
		
	public:
		
		void update(ofFbo& _buffer, ofTexture& _backBufferTexture, float _max, float _ClampForce){
			_buffer.begin();
			shader.begin();
			shader.setUniformTexture("Backbuffer", _backBufferTexture, 0);
			shader.setUniform1f("MaxLength", _max);
			shader.setUniform1f("ClampForce", _ClampForce);
			renderFrame(_buffer.getWidth(), _buffer.getHeight());
			shader.end();
			_buffer.end();
		}
	};
}