
#pragma once

#include "ofMain.h"
#include "ftShader.h"


namespace flowTools {
	
	class ftMoveParticleShader : public ftShader {
	public:
		ftMoveParticleShader() {
			bInitialized = 1;
			
			if (ofIsGLProgrammableRenderer())
				glThree();
			else
				glTwo();
			
			if (bInitialized)
				ofLogNotice("ftMoveParticleShader initialized");
			else
				ofLogWarning("ftMoveParticleShader failed to initialize");
		}
		
	protected:
		void glTwo() {
			
			fragmentShader = GLSL120(
								  uniform sampler2DRect Backbuffer;
								  uniform sampler2DRect ALMSTexture;
								  uniform sampler2DRect Velocity;
								  uniform sampler2DRect HomeTexture;
								  uniform float TimeStep;
								  uniform float InverseCellSize;
								  uniform vec2	Scale;
								  uniform vec2	Dimensions;
								  
								  void main(){
									  vec2 st = gl_TexCoord[0].st;
									  vec2 particlePos = texture2DRect(Backbuffer, st).xy;
									  
									  vec4 alms = texture2DRect(ALMSTexture, st);
									  float age = alms.x;
									  float life = alms.y;
									  float mass = alms.z;
									  
									  if (age > 0.0) {
										  vec2 st2 = particlePos * Scale;
										  vec2 u = texture2DRect(Velocity, st2).rg / Scale;
										  vec2 coord =  TimeStep * InverseCellSize * u;
										  
										  particlePos += coord * (1.0 / mass);
									  }
									  else {
										  particlePos = texture2DRect(HomeTexture, st).xy;
									  }
									  
									  
									  gl_FragColor = vec4(particlePos, 0.0, 1.0); ;
								  }
								  );
			
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.linkProgram();
			
		}
		
		void glThree() {
			fragmentShader = GLSL150(
								  uniform sampler2DRect Backbuffer;
								  uniform sampler2DRect ALMSTexture;
								  uniform sampler2DRect Velocity;
								  uniform sampler2DRect HomeTexture;
								  uniform float TimeStep;
								  uniform float InverseCellSize;
								  uniform vec2	Scale;
								  uniform vec2	Dimensions;
								  
								  in vec2 texCoordVarying;
								  out vec4 fragColor;
								  
								  void main(){
									  vec2 st = texCoordVarying;
									  vec2 particlePos = texture(Backbuffer, st).xy;
									  
									  vec4 alms = texture(ALMSTexture, st);
									  float age = alms.x;
									  float life = alms.y;
									  float mass = alms.z;
									  
									  if (age > 0.0) {
										  vec2 st2 = particlePos * Scale;
										  vec2 u = texture(Velocity, st2).rg / Scale;
										  vec2 coord =  TimeStep * InverseCellSize * u;
										  
										  particlePos += coord * (1.0 / mass);
									  }
									  else {
										  particlePos = texture(HomeTexture, st).xy;
									  }
									  
									  
									  fragColor = vec4(particlePos, 0.0, 1.0); ;
								  }
								  );
			
			
			
			
			bInitialized *= shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.bindDefaults();
			bInitialized *= shader.linkProgram();
		}
		
	public:
		
		void update(ofFbo& _buffer, ofTexture& _backBufferTexture, ofTexture& _ALMSTexture, ofTexture& _velocityTexture, ofTexture& _homeTexture, float _timeStep, float _cellSize){
			_buffer.begin();
			shader.begin();
			shader.setUniformTexture("Backbuffer", _backBufferTexture, 0);
			shader.setUniformTexture("ALMSTexture", _ALMSTexture, 1);
			shader.setUniformTexture("Velocity", _velocityTexture, 2);
			shader.setUniformTexture("HomeTexture", _homeTexture, 3);
			shader.setUniform1f("TimeStep", _timeStep);
			shader.setUniform1f("InverseCellSize", 1.0 / _cellSize);
			shader.setUniform2f("Scale", _velocityTexture.getWidth() / _buffer.getWidth(), _velocityTexture.getHeight()/ _buffer.getHeight());
			shader.setUniform2f("Dimensions", _buffer.getWidth(), _buffer.getHeight());
			renderFrame(_buffer.getWidth(), _buffer.getHeight());
			shader.end();
			_buffer.end();
		}
	};
}