
#pragma once

#include "ofMain.h"
#include "ftShader.h"


namespace flowTools {
	
	class ftAddForceShader : public ftShader {
	public:
		ftAddForceShader() {
			
			ofLogVerbose("init ftAddForceShader");
			if (isProgrammableRenderer)
				glThree();
			else
				glTwo();
		}
		
	protected:
		void glTwo() {
			fragmentShader = GLSL120(
								  uniform sampler2DRect Backbuffer;
								  uniform sampler2DRect AddTexture;
								  uniform float force;
								  uniform vec2	Scale;
								  
								  void main(){
									  vec2 st = gl_TexCoord[0].st;
									  vec2 st2 = st * Scale;
									  
									  vec4 color = texture2DRect(Backbuffer, st) + texture2DRect(AddTexture, st2) * force;
									  gl_FragColor = color ;
								  }
								  
								  );
			
			shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			shader.linkProgram();
		}
		
		void glThree() {
			fragmentShader = GLSL150(
								  uniform sampler2DRect Backbuffer;
								  uniform sampler2DRect AddTexture;
								  uniform float force;
								  uniform vec2	Scale;
								  
								  in vec2 texCoordVarying;
								  out vec4 fragColor;
								  
								  void main(){
									  vec2 st = texCoordVarying;
									  vec2 st2 = st * Scale;
									  
									  vec4 color = texture(Backbuffer, st) + texture(AddTexture, st2) * force;
									  fragColor = color ;
								  }
								  );
			
			shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			shader.bindDefaults();
			shader.linkProgram();
		}
		
	public:
		
		void update(ofFbo& _buffer, ofTexture& _backBufferTexture, ofTexture& _addTexture, float _force){
			_buffer.begin();
			shader.begin();
			shader.setUniformTexture("Backbuffer", _backBufferTexture, 0);
			shader.setUniformTexture("AddTexture", _addTexture, 1);
			shader.setUniform1f("force", _force);
			shader.setUniform2f("Scale", _addTexture.getWidth() / _buffer.getWidth(), _addTexture.getHeight()/ _buffer.getHeight());
			renderFrame(_buffer.getWidth(), _buffer.getHeight());
			shader.end();
			_buffer.end();
		}
	};
}