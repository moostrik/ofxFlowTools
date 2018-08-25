
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {
	
	class ftApplyObstacleShader : public ftShader {
	public:
		ftApplyObstacleShader() {
			bInitialized = true;
			if (ofIsGLProgrammableRenderer()) { glThree(); } else { glTwo(); }
			string shaderName = "ftApplyObstacleShader";
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
									 uniform sampler2DRect	SrcTex;
									 uniform sampler2DRect	ObstacleTex;
									 
									 in vec2 texCoordVarying;
									 out vec4 fragColor;
									 
									 void main()
									 {
										 vec2 st = texCoordVarying;
										 vec4 src = texture(SrcTex, st);
										 float obstacle = texture(ObstacleTex, st).x;
										 fragColor = src * vec4((1.0 - obstacle));
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= bindDefaults();
			bInitialized *= linkProgram();
		}
		
	public:
		void update(ofFbo& _fbo, ofTexture& _srcTex, ofTexture& _obstacleTex, ofTexture& _obstacleOffsetTex, float _weight){
			_fbo.begin();
			begin();
			setUniformTexture("ScrTex", _srcTex, 0);
			setUniformTexture("ObstacleTex", _obstacleTex, 1);
			setUniformTexture("OffsetTex", _obstacleOffsetTex, 2);
			setUniform1f("Weight", _weight);
			setUniform2f("Scale", _obstacleTex.getWidth() / _fbo.getWidth(), _obstacleTex.getHeight()/ _fbo.getHeight());
			renderFrame(_fbo.getWidth(), _fbo.getHeight());
			end();
			_fbo.end();
		}
	};
}



