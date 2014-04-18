
#pragma once

#include "ofMain.h"
#include "ftShader.h"


namespace flowTools {
	
	class ftToScalarShader : public ftShader {
	public:
		ftToScalarShader() {
			
			fragmentShader = STRINGIFY(uniform sampler2DRect ScalarTexture;
									   uniform float Scale;
									   void main(){
										   vec4	velocity = texture2DRect(normalTexture, gl_TexCoord[0].st);
										   velocity.xyz -= vec3(Scale * 0.5);
										   velocity.xyz *= vec3(Scale);
										   velocity.w = 0.0;
										   gl_FragColor = velocity;
									   }
									   );
			
			shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			shader.linkProgram();
		}
		
		void update(ofFbo& _buffer, ofTexture& _scalarTexture, float _scale){
			_buffer.begin();
			shader.begin();
			shader.setUniformTexture("ScalarTexture", _scalarTexture, 0);
			shader.setUniform1f("Scale", _scale);
			renderFrame(_buffer.getWidth(), _buffer.getHeight());
			shader.end();
			_buffer.end();
		}
	};
}