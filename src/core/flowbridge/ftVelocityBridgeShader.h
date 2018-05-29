
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
									 uniform float strength;
									 
									 void main(){
										 vec2 u0 = texture2DRect(tex0,gl_TexCoord[0].st).xy * vec2(1.0 - weight);
										 vec2 u1 = texture2DRect(tex1,gl_TexCoord[0].st).xy * vec2(weight);
										 
										 vec2 u = u0 + u1;
										 float magnitude = length(u);
										 u = normalize(u) * vec2(min(magnitude, 1.0));
										 
										 u *= strength;
										 
										 gl_FragColor = vec4(u, 0.0, 1.0);
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
										 vec2 v0 = texture(tex0, texCoordVarying).xy; // * (1.0 - weight);
										 vec2 v1 = texture(tex1, texCoordVarying).xy;// * vec2(weight);
										 
										 vec2 v = v0 * strength * weight + v1 * (1.0 - weight);
//										 float magnitude = length(u);
//										 u = normalize(u) * vec2(min(magnitude, 1.0));
										 
//										 u *= strength;
										 
										 fragColor = vec4(v, 0.0, 1.0);
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
