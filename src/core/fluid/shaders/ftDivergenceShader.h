
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {
	
	class ftDivergenceShader : public ftShader {
	public:
		ftDivergenceShader() {
			bInitialized = true;
			if (ofIsGLProgrammableRenderer()) { glThree(); } else { glTwo(); }
			string shaderName = "ftDivergenceShader";
			if (bInitialized) { ofLogVerbose(shaderName + " initialized"); }
			else { ofLogWarning(shaderName + " failed to initialize"); }
//			load("tempShader/ftVertexShader.vert", "tempShader/" + shaderName + ".frag");
		}
		
	protected:
		void glTwo() {
			fragmentShader = GLSL120(
									 uniform sampler2DRect Velocity;
									 uniform sampler2DRect Obstacle;
									 uniform float HalfInverseCellSize;
									 
									 void v2TexNeighbors(sampler2DRect tex, vec2 st,
														 out vec2 left, out vec2 right, out vec2 bottom, out vec2 top) {
										 left   = texture2DRect(tex, st - vec2(1, 0)).xy;
										 right  = texture2DRect(tex, st + vec2(1, 0)).xy;
										 bottom = texture2DRect(tex, st - vec2(0, 1)).xy;
										 top    = texture2DRect(tex, st + vec2(0, 1)).xy;
									 }
									 
									 void fRoundTexNeighbors(sampler2DRect tex, vec2 st,
															 out float left, out float right, out float bottom, out float top) {
										 left   = ceil(texture2DRect(tex, st - vec2(1, 0)).x - 0.5); // round not available
										 right  = ceil(texture2DRect(tex, st + vec2(1, 0)).x - 0.5);
										 bottom = ceil(texture2DRect(tex, st - vec2(0, 1)).x - 0.5);
										 top    = ceil(texture2DRect(tex, st + vec2(0, 1)).x - 0.5);
									 }
									 
									 void main(){
										 vec2 st = gl_TexCoord[0].st;
										 
										 vec2 vL; vec2 vR; vec2 vB; vec2 vT;
										 v2TexNeighbors (Velocity, st, vL, vR, vB, vT);
										 float oL; float oR; float oB; float oT;
										 fRoundTexNeighbors (Obstacle, st, oL, oR, oB, oT);
										 
										 //   if (oL > 0.9) vL = vec2(0.0, 0.0);
										 //   if (oR > 0.9) vR = vec2(0.0, 0.0);
										 //   if (oB > 0.9) vB = vec2(0.0, 0.0);
										 //   if (oT > 0.9) vT = vec2(0.0, 0.0);
										 
										 vL *= 1.0 - oL;
										 vR *= 1.0 - oR;
										 vB *= 1.0 - oB;
										 vT *= 1.0 - oT;
										 
										 gl_FragColor.r = HalfInverseCellSize * (vR.x - vL.x + vT.y - vB.y);
										 
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= linkProgram();
		}
		
		void glThree() {
			fragmentShader = GLSL150(
									 uniform sampler2DRect Velocity;
									 
									 in vec2 texCoordVarying;
									 out vec4 fragColor;
									 
									 void main(){
										 vec2 st = texCoordVarying;
										 vec2 vL = texture(Velocity, st - vec2(1, 0)).xy;
										 vec2 vR = texture(Velocity, st + vec2(1, 0)).xy;
										 vec2 vB = texture(Velocity, st - vec2(0, 1)).xy;
										 vec2 vT = texture(Velocity, st + vec2(0, 1)).xy;
										 float divergence = 0.5 * (vR.x - vL.x + vT.y - vB.y);
										 fragColor = vec4(divergence, 0, 0, 0);
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= bindDefaults();
			bInitialized *= linkProgram();
		}
		
	public:
		void update(ofFbo& _fbo, ofTexture& _velTex){
			_fbo.begin();
			begin();
			setUniformTexture("Velocity", _velTex, 1);
			renderFrame(_fbo.getWidth(),_fbo.getHeight());
			end();
			_fbo.end();
		}
	};
}


