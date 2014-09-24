
#pragma once

#include "ofMain.h"
#include "ftShader.h"


namespace flowTools {
	
	class ftDrawParticleShader : public ftShader {
	public:
		ftDrawParticleShader() {
			
			ofLogVerbose("init ftDrawParticleShader");
			if (isProgrammableRenderer)
				glThree();
			else
				glTwo();
		}
		
	protected:
		void glTwo() {
			fragmentShader = GLSL(120,
								  uniform sampler2DRect positionTexture;
								  uniform sampler2DRect ALMSTexture;
								  void main(){
									  
									  vec2 st = gl_Vertex.xy;
									  
									  vec2 texPos = texture2DRect(positionTexture, st).xy;
									  gl_Position = gl_ModelViewProjectionMatrix * vec4(texPos, 0.0, 1.0);
									  vec4 alms = texture2DRect(ALMSTexture, st);
									  float age = alms.x;
									  float life = alms.y;
									  float size = alms.w;
									  gl_PointSize = size;
									  vec4 color = (age > 0.0)? vec4(vec3(1.0), min (0.5 - (age / life) * 0.5,age * 5.)): vec4(0.0);
									  gl_FrontColor = vec4(0.0,1.0,1.0,1.0);
									  
								  }
								  );
			
			shader.setupShaderFromSource(GL_VERTEX_SHADER, fragmentShader);
			shader.linkProgram();

			
		}
		
		void glThree() {
			vertexShader = GLSL(150,
								uniform mat4 modelViewProjectionMatrix;
								uniform mat4 textureMatrix;
								uniform sampler2DRect positionTexture;
								
								in vec4 position;
								in vec2	texcoord;
								in vec4	color;
								
								out vec2 texCoordVarying;
								out vec4 colorVarying;
								
								void main()
								{
									colorVarying = color;
									texCoordVarying = (textureMatrix*vec4(texcoord.x,texcoord.y,0,1)).xy;
									
									
									vec2 st = position.xy;
									vec2 texPos = texture(positionTexture, st).xy;
									gl_Position = modelViewProjectionMatrix * vec4(texPos, 0.0, 1.0);
								//	gl_PointSize = 5;
									
									
									
									
								//	gl_Position = modelViewProjectionMatrix * position;
								}
								);
			
			fragmentShader = GLSL(150,
								  out vec4 fragColor;
								  
								  void main()
								  {
									  fragColor = vec4(1.0,1.0,1.0,1.0);
								  }
								  );
			/*
			fragmentShader = GLSL(150,
								  uniform mat4 modelViewProjectionMatrix;
								  uniform sampler2DRect positionTexture;
								  uniform sampler2DRect ALMSTexture;
								  
								  in vec4 position;
								  in vec2 texCoordVarying;
								  out vec4 fragColor;
								  
								  void main(){
									  
									  vec2 st = position.xy;
									  
									  vec2 texPos = texture(positionTexture, st).xy;
									  gl_Position = modelViewProjectionMatrix * vec4(texPos, 0.0, 1.0);
									  vec4 alms = texture2DRect(ALMSTexture, st);
									  float age = alms.x;
									  float life = alms.y;
									  float size = alms.w;
									  gl_PointSize = size;
									  vec4 color = (age > 0.0)? vec4(vec3(1.0), min (0.5 - (age / life) * 0.5,age * 5.)): vec4(0.0);
									  fragColor = color;
									  
								  }
								  );
			*/
			
			
			
			shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			shader.bindDefaults();
			shader.linkProgram();
		}
		
	public:
		
		void update(GLuint& particles, int _numParticles, ofTexture& _positionTexture, ofTexture& _ALMSTexture){
			shader.begin();
			shader.setUniformTexture("positionTexture", _positionTexture, 0);
			shader.setUniformTexture("ALMSTexture", _ALMSTexture, 1);
			glEnable(GL_POINT_SMOOTH);
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, particles);
			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(2, GL_FLOAT, 0, 0);
			
			//		glBindBufferARB(GL_ARRAY_BUFFER_ARB, *myParticles.getColors());
			//		glEnableClientState(GL_COLOR_ARRAY);
			//		glColorPointer(4, GL_FLOAT, 0, 0);
			
			//		glBlendFunc(GL_DST_COLOR , GL_DST_COLOR);
			glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
			glDrawArrays(GL_POINTS, 0, _numParticles);
			glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
			
			glDisableClientState(GL_VERTEX_ARRAY);
			//		glDisableClientState(GL_COLOR_ARRAY);
			glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
			glDisable(GL_POINT_SMOOTH);
			shader.end();
			
		}
	};
}