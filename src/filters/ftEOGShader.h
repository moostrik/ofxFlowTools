
#pragma once
#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {

	class ftEOGShader : public ftShader {
	public:
		ftEOGShader(){
			
			fragmentShader = STRINGIFY(uniform sampler2DRect tex0;
									   uniform float exposure;
									   uniform float offset;
									   uniform float gamma;
									   
									   void main(){
										   vec4 color = texture2DRect(tex0,gl_TexCoord[0].st);
										   
										   vec3 eog = pow((color.xyz * vec3(exposure)) + vec3(offset), vec3(1.0/gamma));
										   
										   
										   gl_FragColor = vec4(eog, color.w);
									   }
									   );
			
			shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			shader.linkProgram();
		}
		
		
		void update(ofFbo& _drawBuffer, ofTexture& _srcTexture, float _exposure, float _offset, float _gamma){
			_drawBuffer.begin();
			shader.begin();
			shader.setUniformTexture( "tex0" , _srcTexture, 0 );
			shader.setUniform1f("exposure", _exposure );
			shader.setUniform1f("offset", _offset);
			shader.setUniform1f("gamma", _gamma);
			renderFrame(_drawBuffer.getWidth(), _drawBuffer.getHeight());
			shader.end();
			_drawBuffer.end();
		}
		
	};
}