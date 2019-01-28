
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
//			load("tempShader/ftVertexShader.vert", "tempShader/" + shaderName + ".frag");
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
									 precision mediump float;
									 precision mediump int;
									 
									 in vec2 texCoordVarying;
									 out vec2 glFragColor;
									 
									 uniform sampler2DRect tex_velocity;
									 uniform sampler2DRect tex_pressure;
									 uniform sampler2DRect tex_obstacleC;
									 uniform sampler2DRect tex_obstacleN;
									 
									 uniform float halfrdx;
									 
									 void main(){
										 
										 vec2 posn = texCoordVarying;
										 
										 float oC = texture(tex_obstacleC, posn).x;
										 if (oC == 1.0) {
											 glFragColor = vec2(0);
											 return;
										 }
										 
										 // pressure
										 float pT = textureOffset(tex_pressure, posn, + ivec2(0,1)).x;
										 float pB = textureOffset(tex_pressure, posn, - ivec2(0,1)).x;
										 float pR = textureOffset(tex_pressure, posn, + ivec2(1,0)).x;
										 float pL = textureOffset(tex_pressure, posn, - ivec2(1,0)).x;
										 float pC = texture      (tex_pressure, posn).x;
										 
										 // pure Neumann pressure boundary
										 // use center pressure if neighbor is an obstacle
										 vec4 oN = texture(tex_obstacleN, posn);
										 pT = mix(pT, pC, oN.x);  // if (oT > 0.0) xT = xC;
										 pB = mix(pB, pC, oN.y);  // if (oB > 0.0) xB = xC;
										 pR = mix(pR, pC, oN.z);  // if (oR > 0.0) xR = xC;
										 pL = mix(pL, pC, oN.w);  // if (oL > 0.0) xL = xC;
										 
										 // gradient subtract
										 vec2 grad = halfrdx * vec2(pR - pL, pT - pB);
										 vec2 vOld = texture(tex_velocity, posn).xy;
										 
										 glFragColor = vOld - grad;
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


