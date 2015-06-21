
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {

	class ftJacobiShader : public ftShader {
	public:
		ftJacobiShader() {
			bInitialized = 1;
			
			if (ofGetGLProgrammableRenderer())
				glThree();
			else
				glTwo();
			
			if (bInitialized)
				ofLogNotice("ftJacobiShader initialized");
			else
				ofLogWarning("ftJacobiShader failed to initialize");
		}
		
	protected:
		void glTwo() {
			
			fragmentShader = GLSL120(
								  uniform sampler2DRect Pressure;
								  uniform sampler2DRect Divergence;
								  uniform sampler2DRect Obstacle;
								  uniform float Alpha;
								  //	   uniform float InverseBeta = 0.25;
								  
								  void fTexNeighbors(sampler2DRect tex, vec2 st,
													 out float left, out float right, out float bottom, out float top) {
									  left   = texture2DRect(tex, st - vec2(1, 0)).x;
									  right  = texture2DRect(tex, st + vec2(1, 0)).x;
									  bottom = texture2DRect(tex, st - vec2(0, 1)).x;
									  top    = texture2DRect(tex, st + vec2(0, 1)).x;
								  }
								  
								  void fRoundTexNeighbors(sampler2DRect tex, vec2 st,
														  out float left, out float right, out float bottom, out float top) {
									  left   = ceil(texture2DRect(tex, st - vec2(1, 0)).x - 0.5); // round not available
									  right  = ceil(texture2DRect(tex, st + vec2(1, 0)).x - 0.5);
									  bottom = ceil(texture2DRect(tex, st - vec2(0, 1)).x - 0.5);
									  top    = ceil(texture2DRect(tex, st + vec2(0, 1)).x - 0.5);
								  }
								  
								  void main() {
									  
									  vec2 st = gl_TexCoord[0].st;
									  
									  float pL; float pR; float pB; float pT;
									  fTexNeighbors (Pressure, st, pL, pR, pB, pT);
									  float pC = texture2DRect(Pressure, st).x;
									  
									  float oL; float oR; float oB; float oT;
									  fRoundTexNeighbors (Obstacle, st, oL, oR, oB, oT);
									  
									  float bC = texture2DRect(Divergence, st ).x;
									  
									  //   if (oL > 0.9) pL = pC;
									  //   if (oR > 0.9) pR = pC;
									  //   if (oB > 0.9) pB = pC;
									  //   if (oT > 0.9) pT = pC;
									  
									  pL = pL * (1.0 - oL) + pC * oL;
									  pR = pR * (1.0 - oR) + pC * oR;
									  pB = pB * (1.0 - oB) + pC * oB;
									  pT = pT * (1.0 - oT) + pC * oT;
									  
									  
									  gl_FragColor = vec4((pL + pR + pB + pT + Alpha * bC) * 0.25, 0.0,0.0,0.0);
								  }
								  );
			
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.linkProgram();
		}
		
		void glThree() {
						fragmentShader = GLSL150(
								  uniform sampler2DRect Pressure;
								  uniform sampler2DRect Divergence;
								  uniform sampler2DRect Obstacle;
								  uniform float Alpha;
								  //	   uniform float InverseBeta = 0.25;
								  in vec2 texCoordVarying;
								  out vec4 fragColor;
								  
								  void fTexNeighbors(sampler2DRect tex, vec2 st,
													 out float left, out float right, out float bottom, out float top) {
									  left   = texture(tex, st - vec2(1, 0)).x;
									  right  = texture(tex, st + vec2(1, 0)).x;
									  bottom = texture(tex, st - vec2(0, 1)).x;
									  top    = texture(tex, st + vec2(0, 1)).x;
								  }
								  
								  void fRoundTexNeighbors(sampler2DRect tex, vec2 st,
														  out float left, out float right, out float bottom, out float top) {
									  left   = ceil(texture(tex, st - vec2(1, 0)).x - 0.5); // round not available
									  right  = ceil(texture(tex, st + vec2(1, 0)).x - 0.5);
									  bottom = ceil(texture(tex, st - vec2(0, 1)).x - 0.5);
									  top    = ceil(texture(tex, st + vec2(0, 1)).x - 0.5);
								  }
								  
								  void main() {
									  
									  vec2 st = texCoordVarying;
									  
									  float pL; float pR; float pB; float pT;
									  fTexNeighbors (Pressure, st, pL, pR, pB, pT);
									  float pC = texture(Pressure, st).x;
									  
									  float oL; float oR; float oB; float oT;
									  fRoundTexNeighbors (Obstacle, st, oL, oR, oB, oT);
									  
									  float bC = texture(Divergence, st ).x;
									  
									  //   if (oL > 0.9) pL = pC;
									  //   if (oR > 0.9) pR = pC;
									  //   if (oB > 0.9) pB = pC;
									  //   if (oT > 0.9) pT = pC;
									  
									  pL = pL * (1.0 - oL) + pC * oL;
									  pR = pR * (1.0 - oR) + pC * oR;
									  pB = pB * (1.0 - oB) + pC * oB;
									  pT = pT * (1.0 - oT) + pC * oT;
									  
									  
									  fragColor = vec4((pL + pR + pB + pT + Alpha * bC) * 0.25, 0.0,0.0,0.0);
								  }
								  );
			
			bInitialized *= shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.bindDefaults();
			bInitialized *= shader.linkProgram();
		}
		
	public:
		
		void update(ofFbo& _buffer, ofTexture& _backBufferTexture, ofTexture& _divergenceTexture, ofTexture& _obstaclesTexture, float _cellSize){
			_buffer.begin();
			shader.begin();
			shader.setUniform1f("Alpha", -_cellSize * _cellSize);
//			shader.setUniform1f("InverseBeta", _inverseBeta);
			shader.setUniformTexture("Pressure", _backBufferTexture, 0);
			shader.setUniformTexture("Divergence", _divergenceTexture, 1);
			shader.setUniformTexture("Obstacle", _obstaclesTexture, 2);
			renderFrame(_buffer.getWidth(), _buffer.getHeight());
			shader.end();
			_buffer.end();
		}
	};
}