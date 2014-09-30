
#pragma once

#include "ofMain.h"
#include "ftShader.h"


namespace flowTools {
	
	class ftInitPositionShader : public ftShader {
	public:
		ftInitPositionShader() {
			
			ofLogVerbose("init ftInitPositionShader");
			if (isProgrammableRenderer)
				glThree();
			else
				glTwo();
		}
		
	protected:
		void glTwo() {
			
			fragmentShader = GLSL120(
								  uniform vec2 Dimensions;
								  void main(){
									  vec2 st = gl_TexCoord[0].st;
									  
									  gl_FragColor = vec4(st, 0.0, 1.0);
									  
								  }
								  );
			
			shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			shader.linkProgram();
		}
		
		void glThree() {
			
			fragmentShader = GLSL150(
								  uniform vec2 Dimensions;
								  
								  in vec2 texCoordVarying;
								  out vec4 fragColor;
								  
								  void main(){
									  vec2 st = texCoordVarying;
									  
									  fragColor = vec4(st, 0.0, 1.0);
									  
								  }
								  );
			
			shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			shader.bindDefaults();
			shader.linkProgram();
		}
		
	public:
		
		
		void update(ofFbo& _buffer){
			_buffer.begin();
			shader.begin();
			shader.setUniform2f("Dimensions", _buffer.getWidth(), _buffer.getHeight());
			renderFrame(_buffer.getWidth(), _buffer.getHeight());
			shader.end();
			_buffer.end();

		}
	};
}