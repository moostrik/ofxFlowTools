
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
			vertexShader = GLSL120(
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
									  gl_FrontColor = color;
									  
								  }
								  );
			
			
			shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			shader.linkProgram();

			
		}
		
		void glThree() {
			vertexShader = GLSL150(
								uniform mat4 modelViewProjectionMatrix;
								uniform mat4 textureMatrix;
								uniform sampler2DRect positionTexture;
								uniform sampler2DRect ALMSTexture;
								
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
									
									vec4 alms = texture(ALMSTexture, st);
									float age = alms.x;
									float life = alms.y;
									float size = alms.w;
									gl_PointSize = size;
									colorVarying = (age > 0.0)? vec4(vec3(1.0), min (0.5 - (age / life) * 0.5,age * 5.)): vec4(0.0);
								}
								);
			
			
			
			// thanx to: http://mmmovania.blogspot.nl/2010/12/circular-point-sprites-in-opengl-33.html
			
			fragmentShader = GLSL150(
								  in vec4 colorVarying;
								  out vec4 fragColor;
								  
								  void main()
								  {
									  vec2 p = gl_PointCoord * 2.0 - vec2(1.0);
									  float d = dot(p,p);
									  float r = sqrt(d);
									  
									  if(d > r)
										  discard;
									  else
										  fragColor = colorVarying * (1.0, 1.0, 1.0, 1.0 - pow(r, 2.5));
								  }
								  );
			
			shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			shader.bindDefaults();
			shader.linkProgram();
		}
		
	public:
		
		void update(ofVboMesh &particleVbo, int _numParticles, ofTexture& _positionTexture, ofTexture& _ALMSTexture){
			shader.begin();
			shader.setUniformTexture("positionTexture", _positionTexture, 0);
			shader.setUniformTexture("ALMSTexture", _ALMSTexture, 1);
			
			bool dinges = true;
			glEnable(GL_POINT_SMOOTH);
			glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
			
			particleVbo.draw();
			
			glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
			glDisable(GL_POINT_SMOOTH);
			shader.end();
			
		}
	};
}