
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {
	
	class ftJacobiPressureShader : public ftShader {
	public:
		ftJacobiPressureShader() {
			bInitialized = 1;
			if (ofIsGLProgrammableRenderer()) { glFour(); } else { glTwo(); }
			string shaderName = "ftJacobiPressureShader";
			if (bInitialized) { ofLogVerbose(shaderName + " initialized"); }
			else { ofLogWarning(shaderName + " failed to initialize"); }
//			load("tempShader/ftVertexShader.vert", "tempShader/" + shaderName + ".frag");
		}
		
	protected:
		void glTwo() {
			fragmentShader = GLSL120(
									 uniform sampler2DRect tex_source;
									 uniform sampler2DRect tex_divergence;
									 uniform sampler2DRect tex_obstacle;
									 uniform sampler2DRect tex_obstacleOffset;
									 
									 uniform float alpha;
									 uniform float beta;
									 
									 void main(){
										 vec2 st = gl_TexCoord[0].st;
										 
										 float oC = texture2DRect(tex_obstacle, st).x;
										 if (oC == 1.0) {
											 gl_FragColor = vec4(0.0);
											 return;
										 }
										 
										 // tex b
										 vec4 bC = texture2DRect(tex_divergence, st);
										 
										 // tex x
										 vec4 xT = texture2DRect(tex_source, st + ivec2(0,1));
										 vec4 xB = texture2DRect(tex_source, st - ivec2(0,1));
										 vec4 xR = texture2DRect(tex_source, st + ivec2(1,0));
										 vec4 xL = texture2DRect(tex_source, st - ivec2(1,0));
										 vec4 xC = texture2DRect(tex_source, st);
										 
										 // pure Neumann pressure boundary
										 // use center x (pressure) if neighbor is an obstacle
										 vec4 oN = texture2DRect(tex_obstacleOffset, st);
										 xT = mix(xT, xC, oN.x);
										 xB = mix(xB, xC, oN.y);
										 xR = mix(xR, xC, oN.z);
										 xL = mix(xL, xC, oN.w);
										 
										 gl_FragColor = (xL + xR + xB + xT + alpha * bC) * beta;
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
									 out vec4 glFragColor;
									 
									 uniform sampler2DRect tex_source;
									 uniform sampler2DRect tex_divergence;
									 uniform sampler2DRect tex_obstacle;
									 uniform sampler2DRect tex_obstacleOffset;
									 
									 uniform float alpha;
									 uniform float beta;
									 
									 void main(){
										 vec2 st = texCoordVarying;
										 
										 float oC = texture(tex_obstacle, st).x;
										 if (oC == 1.0) {
											 glFragColor = vec4(0.0);
											 return;
										 }
										 
										 vec4 bC = texture(tex_divergence, st);
										 
										 vec4 xT = textureOffset(tex_source, st, + ivec2(0,1));
										 vec4 xB = textureOffset(tex_source, st, - ivec2(0,1));
										 vec4 xR = textureOffset(tex_source, st, + ivec2(1,0));
										 vec4 xL = textureOffset(tex_source, st, - ivec2(1,0));
										 vec4 xC = texture      (tex_source, st);
										 
										 // pure Neumann pressure boundary
										 // use center x (pressure) if neighbor is an obstacle
										 vec4 oN = texture(tex_obstacleOffset, st);
										 xT = mix(xT, xC, oN.x);
										 xB = mix(xB, xC, oN.y);
										 xR = mix(xR, xC, oN.z);
										 xL = mix(xL, xC, oN.w);
										 
										 glFragColor = (xL + xR + xB + xT + alpha * bC) * beta;
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= bindDefaults();
			bInitialized *= linkProgram();
		}
		
	public:
		void update(ofFbo& _fbo, ofTexture& _backTex, ofTexture& _divTex, ofTexture& _obsCTex, ofTexture& _obsNTex, float _gridscale){
			float alpha = -(_gridscale * _gridscale);
			float beta = 0.25f;
			
			_fbo.begin();
			begin();
			setUniform1f		("alpha",			alpha);
			setUniform1f		("beta",			beta);
			setUniformTexture	("tex_source",		_backTex,	0);
			setUniformTexture	("tex_divergence",	_divTex,	1);
			setUniformTexture	("tex_obstacle",	_obsCTex,	2);
			setUniformTexture	("tex_obstacleOffset",	_obsNTex,	3);
			renderFrame(_fbo.getWidth(), _fbo.getHeight());
			end();
			_fbo.end();
		}
	};
}


