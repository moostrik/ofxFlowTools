
#pragma once
#include "ofMain.h"
#include "ftShader.h"
#include "ftFbo.h"

namespace flowTools {
	
	class ftVelocityBridgeShader : public ftShader {
	public:
		ftVelocityBridgeShader(){
			bInitialized = 1;
			
			if (ofIsGLProgrammableRenderer())
				glThree();
			else
				glTwo();
			
			if (bInitialized)
				ofLogNotice("ftVelocityBridgeShader initialized");
			else
				ofLogWarning("ftVelocityBridgeShader failed to initialize");
		}
		
	protected:
		void glTwo() {
			fragmentShader = GLSL120(
									 uniform sampler2DRect tex0;
									 uniform sampler2DRect tex1;
									 uniform float weight;
									 
									 void main(){
										 vec2 vel0 = texture2DRect(tex0, gl_TexCoord[0].st).xy;
										 vec2 vel1 = texture2DRect(tex1, gl_TexCoord[0].st).yx;
										 vec2 vel = vel0 + vel1 * weight;
										 float magnitude = min(length(vel), 1);
										 vel = normalize(vel) * magnitude;
										 gl_FragColor = vec4(vel, 0.0, 1.0);
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
									 
									 in vec2 texCoordVarying;
									 out vec4 fragColor;
									 
									 void main(){
										 vec2 vel0 = texture(tex0, texCoordVarying).xy;
										 vec2 vel1 = texture(tex1, texCoordVarying).xy;
										 vec2 vel = vel0 + vel1 * weight;
										 float magnitude = min(length(vel), 1);
										 vel = normalize(vel) * magnitude;
										 fragColor = vec4(vel, 0.0, 1.0);
									 }
									 );
			
			bInitialized *= shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.bindDefaults();
			bInitialized *= shader.linkProgram();
			
		}
		
	public:
		void update(ofFbo& _buffer, ofTexture& _texture0,  ofTexture& _texture1, float _weight){
			_buffer.begin();
			shader.begin();
			shader.setUniformTexture("tex0", _texture0, 0);
			shader.setUniformTexture("tex1", _texture1, 1);
			shader.setUniform1f("weight", _weight);
			renderFrame(_buffer.getWidth(), _buffer.getHeight());
			shader.end();
			_buffer.end();
		}
	};
}
