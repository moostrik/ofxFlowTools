
#pragma once
#include "ofMain.h"
#include "ftShader.h"
#include "ftFbo.h"

namespace flowTools {
	
	class ftDecayShader : public ftShader {
	public:
		ftDecayShader(){
			bInitialized = 1;
			
			if (ofIsGLProgrammableRenderer())
				glThree();
			else
				glTwo();
			
			if (bInitialized)
				ofLogNotice("ftDecayShader initialized");
			else
				ofLogWarning("ftDecayShader failed to initialize");
		}
		
	protected:
		void glTwo() {
			fragmentShader = GLSL120(
									 uniform sampler2DRect tex0;
									 uniform sampler2DRect tex1;
									 uniform float decay;
									 
									 void main(){
										 vec4 color0 = texture2DRect(tex0,gl_TexCoord[0].st);
										 vec4 color1 = texture2DRect(tex1,gl_TexCoord[0].st);
										 color0.xyz *= vec3(1.0 - decay);
										 
										 gl_FragColor = vec4(color0.xyz + color1.xyz, 1.0);
									 }
									 );
			
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.linkProgram();
			
		}
		
		void glThree() {
			
			fragmentShader = GLSL150(
									 uniform sampler2DRect tex0;
									 uniform sampler2DRect tex1;
									 uniform float decay;
									 
									 in vec2 texCoordVarying;
									 out vec4 fragColor;
									 
									 void main(){
										 vec4 color0 = texture(tex0, texCoordVarying);
										 vec4 color1 = texture(tex1, texCoordVarying);
										 color0.xyz *= vec3(1.0 - decay);
										 
										 fragColor = vec4(color0.xyz + color1.xyz, 1.0);
									 }
									 );
			
			bInitialized *= shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.bindDefaults();
			bInitialized *= shader.linkProgram();
			
		}
		
	public:
		void update(ofFbo& _buffer, ofTexture& _backBufferTexture,  ofTexture& _texture, float _decay){
			_buffer.begin();
			shader.begin();
			shader.setUniformTexture("tex0", _backBufferTexture, 0);
			shader.setUniformTexture("tex1", _texture, 1);
			shader.setUniform1f("decay", max(_decay, 0.0f));
			renderFrame(_buffer.getWidth(), _buffer.getHeight());
			shader.end();
			_buffer.end();
		}
	};
}