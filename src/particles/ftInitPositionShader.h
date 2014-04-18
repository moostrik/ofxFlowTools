
#pragma once

#include "ofMain.h"
#include "ftShader.h"


namespace flowTools {
	
	class ftInitPositionShader : public ftShader {
	public:
		ftInitPositionShader() {
			
			fragmentShader = STRINGIFY(uniform vec2 Dimensions;
									   void main(){
										   vec2 st = gl_TexCoord[0].st;
										   
										   gl_FragColor = vec4(st, 0.0, 1.0);
										   
									   }
									   
									   );
			
			shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			shader.linkProgram();
		}
		
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