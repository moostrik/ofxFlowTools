
#pragma once

#include "ofMain.h"
#include "ftShader.h"


namespace flowTools {
	
	class ftClampLengthShader : public ftShader {
	public:
		ftClampLengthShader() {
			
			fragmentShader = STRINGIFY(uniform sampler2DRect Backbuffer;
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