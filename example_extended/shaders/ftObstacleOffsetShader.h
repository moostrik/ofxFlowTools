
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {
	
	class ftObstacleOffsetShader : public ftShader {
	public:
		ftObstacleOffsetShader() {
			bInitialized = true;
			if (ofIsGLProgrammableRenderer()) { glThree(); } else { glTwo(); }
			string shaderName = "ftObstacleOffsetShader";
			if (bInitialized) { ofLogVerbose(shaderName + " initialized"); }
			else { ofLogWarning(shaderName + " failed to initialize"); }
			load("tempShader/ftVertexShader.vert", "tempShader/" + shaderName + ".frag");
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
			
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= linkProgram();
		}
		
		void glThree() {
			fragmentShader = GLSL150(
									 uniform sampler2DRect Backbuffer;
									 uniform float Scale;
									 
									 in vec2 texCoordVarying;
									 out vec4 fragColor;
									 
									 void main(){
										 vec2 st = texCoordVarying;
										 fragColor = texture(Backbuffer, st) * vec4(Scale);
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= bindDefaults();
			bInitialized *= linkProgram();
		}
		
	public:
		void update(ofFbo& _fbo, ofTexture& _tex){
			_fbo.begin();
			begin();
			setUniformTexture("tex0", _tex, 0);
			renderFrame(_fbo.getWidth(), _fbo.getHeight());
			end();
			_fbo.end();
		}
	};
}


