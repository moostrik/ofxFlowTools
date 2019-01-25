
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {
	
	class ftDivergenceShader : public ftShader {
	public:
		ftDivergenceShader() {
			bInitialized = 1;
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
									 
									 void main(){
										 vec2 st = gl_TexCoord[0].st;
										 vec2 vL = texture2DRect(Velocity, st - vec2(1, 0)).xy;
										 vec2 vR = texture2DRect(Velocity, st + vec2(1, 0)).xy;
										 vec2 vB = texture2DRect(Velocity, st - vec2(0, 1)).xy;
										 vec2 vT = texture2DRect(Velocity, st + vec2(0, 1)).xy;
										 float divergence = 0.5 * (vR.x - vL.x + vT.y - vB.y);
										 gl_FragColor = vec4(divergence, 0, 0, 0);
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= linkProgram();
		}
		
		void glThree() {
			fragmentShader = GLSL410(
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


