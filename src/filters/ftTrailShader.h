
#pragma once
#include "ofMain.h"
#include "ftShader.h"
#include "ftFbo.h"

namespace flowTools {
	
	class ftTrailShader : public ftShader {
	public:
		ftTrailShader(){
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
									 uniform float sourceWeight;
									 
									 void main(){
										 vec4 color0 = texture2DRect(tex0,gl_TexCoord[0].st) * sourceWeight;
										 vec4 color1 = texture2DRect(tex1,gl_TexCoord[0].st) * (1.0 - sourceWeight);
										 
										 gl_FragColor = color0 + color1;
									 }
									 );
			
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.linkProgram();
			
		}
		
		void glThree() {
			
			fragmentShader = GLSL150(
									 uniform sampler2DRect tex0;
									 uniform sampler2DRect tex1;
									 uniform float weight;
									 uniform float strength;
									 
									 in vec2 texCoordVarying;
									 out vec4 fragColor;
									 
									 void main(){
										 vec4 color0 = texture(tex0, texCoordVarying) * weight * strength;
										 vec4 color1 = texture(tex1, texCoordVarying) * (1.0 - weight);
										 
										 fragColor = color0 + color1;
									 }
									 );
			
			bInitialized *= shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.bindDefaults();
			bInitialized *= shader.linkProgram();
			
		}
		
	public:
		void update(ofFbo& _buffer, ofTexture& _texture0,  ofTexture& _texture1, float _weight, float _strength = 1.0){
			_buffer.begin();
			shader.begin();
			shader.setUniformTexture("tex0", _texture0, 0);
			shader.setUniformTexture("tex1", _texture1, 1);
			shader.setUniform1f("weight", _weight);
			shader.setUniform1f("strength", _strength);
			renderFrame(_buffer.getWidth(), _buffer.getHeight());
			shader.end();
			_buffer.end();
		}
	};
}