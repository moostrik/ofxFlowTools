
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
									 uniform sampler2DRect Pressure;
									 uniform sampler2DRect Divergence;
									 
									 void main() {
										 vec2 st = gl_TexCoord[0].st;
										 float pC = texture2DRect(Divergence, st ).x;
										 float pL = texture2DRect(Pressure, st - vec2(1, 0)).x;
										 float pR = texture2DRect(Pressure, st + vec2(1, 0)).x;
										 float pB = texture2DRect(Pressure, st - vec2(0, 1)).x;
										 float pT = texture2DRect(Pressure, st + vec2(0, 1)).x;
										 float alpha = -1;
										 float beta = 0.25;
										 float pres = (alpha * pC + pL + pR + pB + pT) * beta;
										 gl_FragColor = vec4(pres, 0.0, 0.0, 0.0);
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
											 glFragColor = vec4(0);
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
										 xT = mix(xT, xC, oN.x);  // if (oT > 0.0) xT = xC;
										 xB = mix(xB, xC, oN.y);  // if (oB > 0.0) xB = xC;
										 xR = mix(xR, xC, oN.z);  // if (oR > 0.0) xR = xC;
										 xL = mix(xL, xC, oN.w);  // if (oL > 0.0) xL = xC;
										 
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


