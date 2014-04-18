
#pragma once

#include "ofMain.h"
#include "ftShader.h"


namespace flowTools {
	
	class ftAdvectShader : public ftShader {
	public:
		ftAdvectShader() {
			
			fragmentShader = STRINGIFY(uniform sampler2DRect Backbuffer;
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

									/*	   float solid = texture2DRect(Obstacle, st).r;
										   
										   if (solid > 0.9) {
											   gl_FragColor = vec4(0.0,0.0,0.0,0.0);
											   return;
										   }
									*/	   
										   vec2 u = texture2DRect(Velocity, st2).rg / Scale;
										   vec2 coord =  st - TimeStep * InverseCellSize * u;
										   
										   gl_FragColor = Dissipation * texture2DRect(Backbuffer, coord) * inverseSolid;
										   
									   }
									   
									   );
			
			shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			shader.linkProgram();
		}
		
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