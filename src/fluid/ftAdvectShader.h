
#pragma once

#include "ofMain.h"
#include "ftShader.h"


namespace flowTools {
	
	class ftAdvectShader : public ftShader {
	public:
		ftAdvectShader() {
			bInitialized = 1;
			
			if (ofGetGLProgrammableRenderer())
				glThree();
			else
				glTwo();
			
			if (bInitialized)
				ofLogNotice("ftAdvectShader initialized");
			else
				ofLogWarning("ftAdvectShader failed to initialize");
		}
		
	protected:
		void glTwo() {
			fragmentShader = GLSL120(
								  uniform sampler2DRect Backbuffer;
								  uniform sampler2DRect Obstacle;
								  uniform sampler2DRect Velocity;
								  
								  uniform float TimeStep;
								  uniform float Dissipation;
								  uniform float InverseCellSize;
								  uniform vec2	Scale;
								  
								  void main(){
									  vec2 st = gl_TexCoord[0].st;
									  vec2 st2 = st * Scale;
									  
									  float inverseSolid = 1.0 - ceil(texture2DRect(Obstacle, st2).x - 0.5);
									  
									  vec2 u = texture2DRect(Velocity, st2).rg / Scale;
									  vec2 coord =  st - TimeStep * InverseCellSize * u;
									  
									  gl_FragColor = Dissipation * texture2DRect(Backbuffer, coord) * inverseSolid;
									  
								  }
								  
								  );
			
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.linkProgram();

		}
		
		void glThree() {
			
			fragmentShader = GLSL150(
								  uniform sampler2DRect Backbuffer;
								  uniform sampler2DRect Obstacle;
								  uniform sampler2DRect Velocity;
								  
								  uniform float TimeStep;
								  uniform float Dissipation;
								  uniform float InverseCellSize;
								  uniform vec2	Scale;
								  
								  in vec2 texCoordVarying;
								  out vec4 fragColor;
								  
								  void main(){
									  vec2 st = texCoordVarying;
									  vec2 st2 = st * Scale;
									  
									  float inverseSolid = 1.0 - ceil(texture(Obstacle, st2).x - 0.5);
									  
									  vec2 u = texture(Velocity, st2).rg / Scale;
									  vec2 coord =  st - TimeStep * InverseCellSize * u;
									  
									  fragColor = Dissipation * texture(Backbuffer, coord) * inverseSolid;
									  
								  }
								  
								  );
			
			bInitialized *= shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.bindDefaults();
			bInitialized *= shader.linkProgram();
		}
		
	public:
		
		void update(ofFbo& _buffer, ofTexture& _backBufferTexture,  ofTexture& _velocityTexture, ofTexture& _obstaclesTexture, float _timeStep, float _dissipation, float _cellSize){
			
			
			_buffer.begin();
			shader.begin();
			shader.setUniform1f("TimeStep", _timeStep);
			shader.setUniform1f("Dissipation", _dissipation);
			shader.setUniform1f("InverseCellSize", 1.0 / _cellSize);
			shader.setUniform2f("Scale", _velocityTexture.getWidth() / _buffer.getWidth(), _velocityTexture.getHeight()/ _buffer.getHeight());
			shader.setUniformTexture("Backbuffer", _backBufferTexture, 0);
			shader.setUniformTexture("Velocity", _velocityTexture, 1);
			shader.setUniformTexture("Obstacle", _obstaclesTexture, 2);
			renderFrame(_buffer.getWidth(), _buffer.getHeight());
			shader.end();
			_buffer.end();
		}
	};
}