
#pragma once

#include "ofMain.h"
#include "ftShader.h"


namespace flowTools {
	
	class ftDisplayScalarShader : public ftShader {
	public:
		ftDisplayScalarShader() {
			
			fragmentShader = STRINGIFY(uniform sampler2DRect FloatTexture;
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