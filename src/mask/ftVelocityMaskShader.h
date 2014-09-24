
#pragma once

#include "ofMain.h"
#include "ftShader.h"


namespace flowTools {
	
	class ftVelocityMaskShader : public ftShader {
	public:
		ftVelocityMaskShader() {
			ofLogVerbose("init ftVelocityMaskShader");
			if (isProgrammableRenderer)
				glThree();
			else
				glTwo();
			}
		
	protected:
		void glTwo() {
			
			fragmentShader = GLSL120(
								  uniform sampler2DRect	Density;
								  uniform sampler2DRect	Velocity;
								  uniform float	force;
								  uniform vec2	Scale;
								  
								  void main(){
									  vec2 st = gl_TexCoord[0].st;
									  vec2 st2 = st * Scale;
									  
									  vec4	color = texture2DRect(Density, st);
									  vec4	vel = texture2DRect(Velocity, st2);
									  float alpha = length(vel.xy) * force;
									  color.rgb *= vec3(alpha);
									  color.w = alpha;
									  
									  gl_FragColor = color;
								  }
								  
								  );
			
			shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			shader.linkProgram();
			
		}
		
		void glThree() {
			
			fragmentShader = GLSL150(
								  uniform sampler2DRect	Density;
								  uniform sampler2DRect	Velocity;
								  uniform float	force;
								  uniform vec2	Scale;
								  
								  in vec2 texCoordVarying;
								  out vec4 fragColor;
								  
								  void main(){
									  
									  vec2 st = texCoordVarying;
									  vec2 st2 = st * Scale;
									  
									  vec4	color = texture(Density, st);
									  vec4	vel = texture(Velocity, st2);
									  float alpha = length(vel.xy) * force;
									  color.rgb *= vec3(alpha);
									  color.w = alpha;
									  
									  fragColor = color;
								  }
								  
								  );
			
			shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			shader.bindDefaults();
			shader.linkProgram();
		}
		
	public:
		
		void update(ofFbo& _buffer, ofTexture& _densityTexture, ofTexture& _velocityTexture, float _force){
			_buffer.begin();
			shader.begin();
			shader.setUniformTexture("Density", _densityTexture, 0);
			shader.setUniformTexture("Velocity", _velocityTexture, 1);
			shader.setUniform1f("force", _force);
			shader.setUniform2f("Scale", _velocityTexture.getWidth() / _buffer.getWidth(), _velocityTexture.getHeight()/ _buffer.getHeight());
			renderFrame(_buffer.getWidth(), _buffer.getHeight());
			shader.end();
			_buffer.end();
		}
	};
}