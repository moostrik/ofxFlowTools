
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {
	
	class ftVorticityCurlShader : public ftShader {
	public:
		ftVorticityCurlShader() {
            bInitialized = 1;
			if (ofIsGLProgrammableRenderer()) { glThree(); } else { glTwo(); }
			string shaderName = "ftVorticityCurlShader";
			if (bInitialized) { ofLogVerbose(shaderName + " initialized"); }
			else { ofLogWarning(shaderName + " failed to initialize"); }
//			load("tempShader/ftVertexShader.vert", "tempShader/" + shaderName + ".frag");
		}
		
	protected:
		void glTwo() {
			fragmentShader = GLSL120(
									 uniform sampler2DRect tex_velocity;
									 uniform sampler2DRect tex_obstacleC;
									 
									 uniform float halfrdx;
									 
									 void main(){
										 
										 vec2 posn = gl_TexCoord[0].st;
										 
										 if (texture2DRect(tex_obstacleC, posn).x == 1.0) {
											 gl_FragColor = vec4(0.0);
											 return;
										 }
										 
										 // velocity
										 vec2 vT = texture2DRect(tex_velocity, posn + ivec2(0,1)).xy;
										 vec2 vB = texture2DRect(tex_velocity, posn - ivec2(0,1)).xy;
										 vec2 vR = texture2DRect(tex_velocity, posn + ivec2(1,0)).xy;
										 vec2 vL = texture2DRect(tex_velocity, posn - ivec2(1,0)).xy;
										 vec2 vC = texture2DRect(tex_velocity, posn             ).xy;
//
										 vec2 curl = vec2(halfrdx) * ((vT.x - vB.x) - (vR.y - vL.y));
										 gl_FragColor = vec4(curl, 0.0, 0.0);
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
									 out float glFragColor;
									 
									 uniform sampler2DRect tex_velocity;
									 uniform sampler2DRect tex_obstacleC;
									 
									 uniform float halfrdx;
									 
									 void main(){
										 
										 vec2 posn = texCoordVarying;
										 
										 if (texture(tex_obstacleC, posn).x == 1.0) {
											 glFragColor = 0.0;
											 return;
										 }
										 
										 // velocity
										 vec2 vT = textureOffset(tex_velocity, posn, + ivec2(0,1)).xy;
										 vec2 vB = textureOffset(tex_velocity, posn, - ivec2(0,1)).xy;
										 vec2 vR = textureOffset(tex_velocity, posn, + ivec2(1,0)).xy;
										 vec2 vL = textureOffset(tex_velocity, posn, - ivec2(1,0)).xy;
										 vec2 vC = texture      (tex_velocity, posn              ).xy;
										 
										 glFragColor = halfrdx * ((vT.x - vB.x) - (vR.y - vL.y));
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= bindDefaults();
			bInitialized *= linkProgram();
		}
		
	public:
		void update(ofFbo& _fbo, ofTexture& _velTex, ofTexture& _obsCTex, float _gridScale){
			_fbo.begin();
			begin();
			setUniform1f		("halfrdx",			0.5f / _gridScale);
			setUniformTexture	("tex_velocity",	_velTex,	0);
			setUniformTexture	("tex_obstacleC",	_obsCTex,	1);
			renderFrame(_fbo.getWidth(),_fbo.getHeight());
			end();
			_fbo.end();
		}
	};
}


