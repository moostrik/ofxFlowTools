
#pragma once

#include "ofMain.h"
#include "ftShader.h"


namespace flowTools {
	
	class ftNormalizationShader : public ftShader {
	public:
		ftNormalizationShader() {
			bInitialized = 1;
			
			if (ofIsGLProgrammableRenderer())
				glThree();
			else
				glTwo();
			
			if (bInitialized)
				ofLogNotice("ftNormalizationShader initialized");
			else
				ofLogWarning("ftNormalizationShader failed to initialize");
		}
		
	protected:
		void glTwo() {
			fragmentShader = GLSL120(
								  uniform sampler2DRect Texture;
								  uniform float Normalization;
								  uniform vec2	Scale;
								  
								  void main(){
									  vec2 st = gl_TexCoord[0].st;
									  vec2 st2 = st * Scale;
									  
									  vec4 color = texture2DRect(Texture, st);
									  
									  float magnitude = length(color);
									  if(magnitude > 0.0) {
										  color = normalize(color) * (magnitude / Normalization);
									  }
									  
									  gl_FragColor = color ;
								  }
								  
								  );
			
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.linkProgram();
		}
		
		void glThree() {
			fragmentShader = GLSL150(
								  uniform sampler2DRect Texture;
								  uniform float Normalization;
								  uniform vec2	Scale;
								  
								  in vec2 texCoordVarying;
								  out vec4 fragColor;
								  
								  void main(){
									  vec2 st = texCoordVarying;
									  vec2 st2 = st * Scale;
									  
									  vec4 color = texture(Texture, st);
									  
									  float magnitude = length(color);
									  if(magnitude > 0.0) {
										  color = normalize(color) * (magnitude / Normalization);
									  }
									  
									  fragColor = color ;
								  }
								  );
			
			bInitialized *= shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.bindDefaults();
			bInitialized *= shader.linkProgram();
		}
		
	public:
		
		void update(ofFbo& _buffer, ofTexture& _texture, float _normalization){
			_buffer.begin();
			shader.begin();
			shader.setUniformTexture("Texture", _texture, 0);
			shader.setUniform1f("Normalization", _normalization);
			shader.setUniform2f("Scale", _texture.getWidth() / _buffer.getWidth(), _texture.getHeight()/ _buffer.getHeight());
			renderFrame(_buffer.getWidth(), _buffer.getHeight());
			shader.end();
			_buffer.end();
		}
	};
}