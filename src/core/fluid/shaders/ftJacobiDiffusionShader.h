
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {
	
	class ftJacobiDiffusionShader : public ftShader {
	public:
		ftJacobiDiffusionShader() {
			bInitialized = 1;
			if (ofIsGLProgrammableRenderer()) { glThree(); } else { glTwo(); }
			string shaderName = "ftJacobiDiffusionShader";
			if (bInitialized) { ofLogVerbose(shaderName + " initialized"); }
			else { ofLogWarning(shaderName + " failed to initialize"); }
//			load("tempShader/ftVertexShader.vert", "tempShader/" + shaderName + ".frag");
		}
		
	protected:
		void glTwo() {
			fragmentShader = GLSL120(
									 uniform sampler2DRect tex_source;
									 uniform sampler2DRect tex_obstacle;
									 uniform sampler2DRect tex_obstacleOffset;
									 
									 uniform float alpha;
									 uniform float beta;
									 uniform vec2  scale;
									 
									 void main(){
										 
										 vec2 st = gl_TexCoord[0].st;
										 
										 float oC = texture2DRect(tex_obstacle, st).x;
										 if (oC == 1.0) {
											 gl_FragColor = vec4(0.0);
											 return;
										 }
										 
										 vec4 xT = texture2DRect(tex_source, st + ivec2(0,1));
										 vec4 xB = texture2DRect(tex_source, st - ivec2(0,1));
										 vec4 xR = texture2DRect(tex_source, st + ivec2(1,0));
										 vec4 xL = texture2DRect(tex_source, st - ivec2(1,0));
										 vec4 xC = texture2DRect(tex_source, st);
										 
										 vec4 oN = texture2DRect(tex_obstacleOffset, st);
										 xT = mix(xT, xC, oN.x);
										 xB = mix(xB, xC, oN.y);
										 xR = mix(xR, xC, oN.z);
										 xL = mix(xL, xC, oN.w);
										 
										 gl_FragColor = (xL + xR + xB + xT + alpha * xC) * beta;
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
									 
									 uniform sampler2DRect tex_source;
									 uniform sampler2DRect tex_obstacle;
									 uniform sampler2DRect tex_obstacleOffset;
									 
									 uniform float alpha;
									 uniform float beta;
									 uniform vec2  scale;
									 
									 void main(){
										 vec2 st = texCoordVarying;
										 vec2 st2 = st * scale;
										 
										 float oC = texture(tex_obstacle, st2).x;
										 if (oC == 1.0) {
											 glFragColor = vec4(0.0);
											 return;
										 }
										 
										 vec4 xT = textureOffset(tex_source, st, + ivec2(0,1));
										 vec4 xB = textureOffset(tex_source, st, - ivec2(0,1));
										 vec4 xR = textureOffset(tex_source, st, + ivec2(1,0));
										 vec4 xL = textureOffset(tex_source, st, - ivec2(1,0));
										 vec4 xC = texture      (tex_source, st);
										 
										 vec4 oN = texture(tex_obstacleOffset, st2);
										 xT = mix(xT, xC, oN.x);
										 xB = mix(xB, xC, oN.y);
										 xR = mix(xR, xC, oN.z);
										 xL = mix(xL, xC, oN.w);
										 
										 glFragColor = (xL + xR + xB + xT + alpha * xC) * beta;
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= bindDefaults();
			bInitialized *= linkProgram();
		}
		
	public:
		void update(ofFbo& _fbo, ofTexture& _backTex, ofTexture& _obsCTex, ofTexture& _obsNTex, float _timeStep, float _gridScale){
			float alpha = (_gridScale * _gridScale) / (_timeStep);
			float beta = 1.0f / (4.0f + alpha);
			
			_fbo.begin();
			begin();
			setUniform1f		("alpha",			alpha);
			setUniform1f		("beta",			beta);
			setUniform2f		("scale",			_obsCTex.getWidth() / _fbo.getWidth(), _obsCTex.getHeight()/ _fbo.getHeight());
			setUniformTexture	("tex_source",		_backTex,	0);
			setUniformTexture	("tex_obstacle",	_obsCTex,	2);
			setUniformTexture	("tex_obstacleOffset",	_obsNTex,	3);
			renderFrame(_fbo.getWidth(), _fbo.getHeight());
			end();
			_fbo.end();
		}
	};
}


