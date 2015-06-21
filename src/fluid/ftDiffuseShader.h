
#pragma once

#include "ofMain.h"
#include "ftShader.h"


namespace flowTools {
	
	class ftDiffuseShader : public ftShader {
	public:
		ftDiffuseShader() {
			bInitialized = 1;
			
			if (ofGetGLProgrammableRenderer())
				glThree();
			else
				glTwo();
			
			if (bInitialized)
				ofLogNotice("ftDiffuseShader initialized");
			else
				ofLogWarning("ftDiffuseShader failed to initialize");
		}
		
	protected:
		void glTwo() {
			fragmentShader = GLSL120(
								  uniform sampler2DRect Velocity;
								  uniform sampler2DRect Obstacle;
								  uniform float Viscosity;
								  uniform float C;
								  uniform int test;
								  
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
									  vec2 vC = texture2DRect(Velocity, st).xy;
									  
									  float oL; float oR; float oB; float oT;
									  fRoundTexNeighbors (Obstacle, st, oL, oR, oB, oT);
									  float inverseSolid = 1.0 - ceil(texture2DRect(Obstacle, st).x - 0.5);
									  
									  vL *= 1.0 - oL;
									  vR *= 1.0 - oR;
									  vB *= 1.0 - oB;
									  vT *= 1.0 - oT;
									  // ADD NEIGHBOR OBSTACLES;
									  
									  vec2 newVel = ((vC + Viscosity * (vL + vR + vB + vT)) / C) * inverseSolid;
									  
									  gl_FragColor = vec4(newVel, 0.0, 0.0);
								  }
								  );
			
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.linkProgram();

		}
		
		void glThree() {
			fragmentShader = GLSL150(
								  uniform sampler2DRect Velocity;
								  uniform sampler2DRect Obstacle;
								  uniform float Viscosity;
								  uniform float C;
								  uniform int test;
								  
								  in vec2 texCoordVarying;
								  out vec4 fragColor;
								  
								  void v2TexNeighbors(sampler2DRect tex, vec2 st,
													  out vec2 left, out vec2 right, out vec2 bottom, out vec2 top) {
									  left   = texture(tex, st - vec2(1, 0)).xy;
									  right  = texture(tex, st + vec2(1, 0)).xy;
									  bottom = texture(tex, st - vec2(0, 1)).xy;
									  top    = texture(tex, st + vec2(0, 1)).xy;
								  }
								  
								  void fRoundTexNeighbors(sampler2DRect tex, vec2 st,
														  out float left, out float right, out float bottom, out float top) {
									  left   = ceil(texture(tex, st - vec2(1, 0)).x - 0.5); // round not available
									  right  = ceil(texture(tex, st + vec2(1, 0)).x - 0.5);
									  bottom = ceil(texture(tex, st - vec2(0, 1)).x - 0.5);
									  top    = ceil(texture(tex, st + vec2(0, 1)).x - 0.5);
								  }
								  
								  void main(){
									  vec2 st = texCoordVarying;
									  
									  
									  vec2 vL; vec2 vR; vec2 vB; vec2 vT;
									  v2TexNeighbors (Velocity, st, vL, vR, vB, vT);
									  vec2 vC = texture(Velocity, st).xy;
									  
									  float oL; float oR; float oB; float oT;
									  fRoundTexNeighbors (Obstacle, st, oL, oR, oB, oT);
									  float inverseSolid = 1.0 - ceil(texture(Obstacle, st).x - 0.5);
									  
									  vL *= 1.0 - oL;
									  vR *= 1.0 - oR;
									  vB *= 1.0 - oB;
									  vT *= 1.0 - oT;
									  // ADD NEIGHBOR OBSTACLES;
									  
									  vec2 newVel = ((vC + Viscosity * (vL + vR + vB + vT)) / C) * inverseSolid;
									  
									  fragColor = vec4(newVel, 0.0, 0.0);
								  }
								  );
			
			
			bInitialized *= shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.bindDefaults();
			bInitialized *= shader.linkProgram();
		}
		
	public:
		
			
		
		void update(ofFbo& _buffer, ofTexture& _backBufferTexture, ofTexture& _obstaclesTexture, float _viscosity){
			_buffer.begin();
			shader.begin();
			shader.setUniform1f("Viscosity", _viscosity);
			shader.setUniform1f("C", 1.0 + 4. * _viscosity);
			shader.setUniformTexture("Velocity", _backBufferTexture, 0);
			shader.setUniformTexture("Obstacle", _obstaclesTexture, 1);
			renderFrame(_buffer.getWidth(), _buffer.getHeight());
			shader.end();
			_buffer.end();
		}
		
	private:
		float c;
	};
	
}