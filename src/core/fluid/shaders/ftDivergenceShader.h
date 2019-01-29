
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {
	
	class ftDivergenceShader : public ftShader {
	public:
		ftDivergenceShader() {
			bInitialized = 1;
			if (ofIsGLProgrammableRenderer()) { glFour(); } else { glTwo(); }
			string shaderName = "ftDivergenceShader";
			if (bInitialized) { ofLogVerbose(shaderName + " initialized"); }
			else { ofLogWarning(shaderName + " failed to initialize"); }
//			load("tempShader/ftVertexShader.vert", "tempShader/" + shaderName + ".frag");
		}
		
	protected:
		void glTwo() {
			fragmentShader = GLSL120(
									 uniform sampler2DRect tex_velocity;
									 uniform sampler2DRect tex_obstacle;
									 uniform sampler2DRect tex_obstacleOffset;
									 
									 uniform float halfrdx;
									 
									 void main(){
										 vec2 st = gl_TexCoord[0].st;
										 
										 float oC = texture2DRect(tex_obstacle, st).x;
										 if (oC == 1.0) {
											 gl_FragColor = vec4(0.0);
											 return;
										 }
										 
										 // velocity
										 vec2 vT = texture2DRect(tex_velocity, st + ivec2(0,1)).xy;
										 vec2 vB = texture2DRect(tex_velocity, st - ivec2(0,1)).xy;
										 vec2 vR = texture2DRect(tex_velocity, st + ivec2(1,0)).xy;
										 vec2 vL = texture2DRect(tex_velocity, st - ivec2(1,0)).xy;
										 vec2 vC = texture2DRect(tex_velocity, st).xy;
										 
										 // no-slip (zero) velocity boundary conditions
										 // use negative center velocity if neighbor is an obstacle
										 vec4 oN = texture2DRect(tex_obstacleOffset, st);
										 vT = mix(vT, -vC, oN.x);
										 vB = mix(vB, -vC, oN.y);
										 vR = mix(vR, -vC, oN.z);
										 vL = mix(vL, -vC, oN.w);
										 
										 vec2 divergence = vec2(halfrdx) * ((vR.x - vL.x) + (vT.y - vB.y));
										 gl_FragColor = vec4(divergence, 0.0, 0.0);
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= linkProgram();
		}
		
		void glFour() {
			fragmentShader = GLSL410(
									 precision mediump float;
									 precision mediump int;
									 
									 in vec2 texCoordVarying;
									 out float glFragColor;
									 
									 uniform sampler2DRect tex_velocity;
									 uniform sampler2DRect tex_obstacle;
									 uniform sampler2DRect tex_obstacleOffset;
									 
									 uniform float halfrdx;
									 
									 void main(){
										 vec2 st = texCoordVarying;
										 
										 float oC = texture(tex_obstacle, st).x;
										 if (oC == 1.0) {
											 glFragColor = 0.0;
											 return;
										 }
										 
										 // velocity
										 vec2 vT = textureOffset(tex_velocity, st, + ivec2(0,1)).xy;
										 vec2 vB = textureOffset(tex_velocity, st, - ivec2(0,1)).xy;
										 vec2 vR = textureOffset(tex_velocity, st, + ivec2(1,0)).xy;
										 vec2 vL = textureOffset(tex_velocity, st, - ivec2(1,0)).xy;
										 vec2 vC = texture      (tex_velocity, st).xy;
										 
										 // no-slip (zero) velocity boundary conditions
										 // use negative center velocity if neighbor is an obstacle
										 vec4 oN = texture(tex_obstacleOffset, st);
										 vT = mix(vT, -vC, oN.x);
										 vB = mix(vB, -vC, oN.y);
										 vR = mix(vR, -vC, oN.z);
										 vL = mix(vL, -vC, oN.w);
										 
										 glFragColor = halfrdx * ((vR.x - vL.x) + (vT.y - vB.y));
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= bindDefaults();
			bInitialized *= linkProgram();
		}
		
	public:
		void update(ofFbo& _fbo, ofTexture& _velTex, ofTexture& _obsCTex, ofTexture& _obsNTex, float _gridScale){
			_fbo.begin();
			begin();
			setUniform1f		("halfrdx",			0.5f / _gridScale);
			setUniformTexture	("tex_velocity",	_velTex,	0);
			setUniformTexture	("tex_obstacle",	_obsCTex,	1);
			setUniformTexture	("tex_obstacleOffset",	_obsNTex,	2);
			renderFrame(_fbo.getWidth(),_fbo.getHeight());
			end();
			_fbo.end();
		}
	};
}


