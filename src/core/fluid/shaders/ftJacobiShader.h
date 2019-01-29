
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {
	
	class ftJacobiShader : public ftShader {
	public:
		ftJacobiShader() {
			bInitialized = 1;
			if (ofIsGLProgrammableRenderer()) { glThree(); } else { glTwo(); }
			string shaderName = "ftJacobiShader";
			if (bInitialized) { ofLogVerbose(shaderName + " initialized"); }
			else { ofLogWarning(shaderName + " failed to initialize"); }
//			load("tempShader/ftVertexShader.vert", "tempShader/" + shaderName + ".frag");
		}
		
	protected:
		void glTwo() {
			fragmentShader = GLSL120(
									 uniform sampler2DRect tex_x;
									 uniform sampler2DRect tex_b;
									 uniform sampler2DRect tex_obstacleC;
									 uniform sampler2DRect tex_obstacleN;
									 
									 uniform float alpha;
									 uniform float rBeta;
									 
									 void main(){
										 
										 vec2 posn = gl_TexCoord[0].st;
										 
										 float oC = texture2DRect(tex_obstacleC, posn).x;
										 if (oC == 1.0) {
											 gl_FragColor = vec4(0.0);
											 return;
										 }
										 
										 // tex b
										 vec4 bC = texture2DRect(tex_b, posn);
										 
										 // tex x
										 vec4 xT = texture2DRect(tex_x, posn + ivec2(0,1));
										 vec4 xB = texture2DRect(tex_x, posn - ivec2(0,1));
										 vec4 xR = texture2DRect(tex_x, posn + ivec2(1,0));
										 vec4 xL = texture2DRect(tex_x, posn - ivec2(1,0));
										 vec4 xC = texture2DRect(tex_x, posn);
										 
										 // pure Neumann pressure boundary
										 // use center x (pressure) if neighbor is an obstacle
										 vec4 oN = texture2DRect(tex_obstacleN, posn);
										 xT = mix(xT, xC, oN.x);
										 xB = mix(xB, xC, oN.y);
										 xR = mix(xR, xC, oN.z);
										 xL = mix(xL, xC, oN.w);
										 
										 gl_FragColor = (xL + xR + xB + xT + alpha * bC) * rBeta;
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
									 out vec4 glFragColor;
									 
									 uniform sampler2DRect tex_x;
									 uniform sampler2DRect tex_b;
									 uniform sampler2DRect tex_obstacleC;
									 uniform sampler2DRect tex_obstacleN;
									 
									 uniform float alpha;
									 uniform float rBeta;
									 
									 void main(){
										 
										 vec2 posn = texCoordVarying;
										 
										 float oC = texture(tex_obstacleC, posn).x;
										 if (oC == 1.0) {
											 glFragColor = vec4(0.0);
											 return;
										 }
										 
										 // tex b
										 vec4 bC = texture(tex_b, posn);
										 
										 // tex x
										 vec4 xT = textureOffset(tex_x, posn, + ivec2(0,1));
										 vec4 xB = textureOffset(tex_x, posn, - ivec2(0,1));
										 vec4 xR = textureOffset(tex_x, posn, + ivec2(1,0));
										 vec4 xL = textureOffset(tex_x, posn, - ivec2(1,0));
										 vec4 xC = texture      (tex_x, posn);
										 
										 // pure Neumann pressure boundary
										 // use center x (pressure) if neighbor is an obstacle
										 vec4 oN = texture(tex_obstacleN, posn);
										 xT = mix(xT, xC, oN.x);
										 xB = mix(xB, xC, oN.y);
										 xR = mix(xR, xC, oN.z);
										 xL = mix(xL, xC, oN.w);
										 
										 glFragColor = (xL + xR + xB + xT + alpha * bC) * rBeta;
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= bindDefaults();
			bInitialized *= linkProgram();
		}
		
	public:
		void update(ofFbo& _fbo, ofTexture& _backTex, ofTexture& _divTex, ofTexture& _obsCTex, ofTexture& _obsNTex, float _alpha, float _beta){
			_fbo.begin();
			begin();
			
			setUniform1f		("alpha",			_alpha);
			setUniform1f		("rBeta",			_beta);
			setUniformTexture	("tex_x",			_backTex,	0);
			setUniformTexture	("tex_b",			_divTex,	1);
			setUniformTexture	("tex_obstacleC",	_obsCTex,	2);
			setUniformTexture	("tex_obstacleN",	_obsNTex,	3);
			renderFrame(_fbo.getWidth(), _fbo.getHeight());
			end();
			_fbo.end();
		}
	};
}


