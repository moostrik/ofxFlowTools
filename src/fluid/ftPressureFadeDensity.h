
#pragma once

#include "ofMain.h"
#include "ftShader.h"


namespace flowTools {
	
	class ftPressureFadeDensity : public ftShader {
	public:
		ftPressureFadeDensity() {
			
			fragmentShader = STRINGIFY(uniform sampler2DRect Backbuffer;
									   uniform sampler2DRect AddTexture;
									   uniform float force;
									   uniform vec2	Scale;
									   
									   void main(){
										   vec2 st = gl_TexCoord[0].st;
										   vec2 st2 = st * Scale;
										   
										   vec4 color = texture2DRect(Backbuffer, st);
										   float pressure = texture2DRect(AddTexture, st2).x * force;
										   color.xyz *= 1.0 - min(pressure, 0.9);
										   
										   gl_FragColor = color ;
									   }
									   
									   );
			
			shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			shader.linkProgram();
		}
		
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