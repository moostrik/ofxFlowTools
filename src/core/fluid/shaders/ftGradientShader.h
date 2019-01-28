
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {
	
	class ftGradientShader : public ftShader {
	public:
		ftGradientShader() {
			bInitialized = 1;
			if (ofIsGLProgrammableRenderer()) { glThree(); } else { glTwo(); }
			string shaderName = "ftGradientShader";
			if (bInitialized) { ofLogVerbose(shaderName + " initialized"); }
			else { ofLogWarning(shaderName + " failed to initialize"); }
			load("tempShader/ftVertexShader.vert", "tempShader/" + shaderName + ".frag");
		}
		
	protected:
		void glTwo() {
			fragmentShader = GLSL120(
									 uniform sampler2DRect Velocity;
									 uniform sampler2DRect Pressure;
									 
									 void main(){
										 vec2 st = gl_TexCoord[0].st;
										 float pL = texture2DRect(Pressure, st - vec2(1, 0)).x;
										 float pR = texture2DRect(Pressure, st + vec2(1, 0)).x;
										 float pB = texture2DRect(Pressure, st - vec2(0, 1)).x;
										 float pT = texture2DRect(Pressure, st + vec2(0, 1)).x;
										 vec2 gradient = 0.5 * vec2(pR - pL, pT - pB);
										 vec2 vel = texture2DRect(Velocity, st).xy  - gradient;
										 gl_FragColor = vec4(vel, 0.0, 0.0);
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= linkProgram();
		}
		
		void glThree() {
			fragmentShader = GLSL410(
									 uniform sampler2DRect Velocity;
									 uniform sampler2DRect Pressure;
									 
									 in vec2 texCoordVarying;
									 out vec4 fragColor;
									 
									 void main(){
										 vec2 st = texCoordVarying;
										 float pL = texture(Pressure, st - vec2(1, 0)).x;
										 float pR = texture(Pressure, st + vec2(1, 0)).x;
										 float pB = texture(Pressure, st - vec2(0, 1)).x;
										 float pT = texture(Pressure, st + vec2(0, 1)).x;
										 vec2 gradient = 0.5 * vec2(pR - pL, pT - pB);
										 vec2 vel = texture(Velocity, st).xy  - gradient;
										 fragColor = vec4(vel, 0.0, 0.0);
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= bindDefaults();
			bInitialized *= linkProgram();
		}
		
	public:
		void update(ofFbo& _fbo, ofTexture& _backTex, ofTexture& _prsTex, ofTexture& _obsCTex, ofTexture& _obsNTex, float _gridScale){
			_fbo.begin();
			begin();
			setUniform1f		("halfrdx",			1.0f / _gridScale);
			setUniformTexture	("tex_velocity",	_backTex,	0);
			setUniformTexture	("tex_pressure",	_prsTex,	1);
			setUniformTexture	("tex_obstacleC",	_obsCTex,	2);
			setUniformTexture	("tex_obstacleN",	_obsNTex,	3);
			renderFrame(_fbo.getWidth(), _fbo.getHeight());
			end();
			_fbo.end();
		}
	};
}


