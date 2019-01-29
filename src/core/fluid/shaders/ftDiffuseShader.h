
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {
	
	class ftDiffuseShader : public ftShader {
	public:
		ftDiffuseShader() {
            bInitialized = 1;
			if (ofIsGLProgrammableRenderer()) { glFour(); } else { glTwo(); }
			string shaderName = "ftDiffuseShader";
			if (bInitialized) { ofLogVerbose(shaderName + " initialized"); }
			else { ofLogWarning(shaderName + " failed to initialize"); }
//			load("tempShader/ftVertexShader.vert", "tempShader/" + shaderName + ".frag");
		}
		
	protected:
		void glTwo() {
			fragmentShader = GLSL120(
									 uniform sampler2DRect Velocity;
									 
									 uniform float Alpha;
									 uniform float Beta;
									 
									 void main(){
										 vec2 st = gl_TexCoord[0].st;
										 vec2 vC = texture2DRect(Velocity, st).xy;
										 vec2 vL = texture2DRect(Velocity, st - vec2(1, 0)).xy;
										 vec2 vR = texture2DRect(Velocity, st + vec2(1, 0)).xy;
										 vec2 vB = texture2DRect(Velocity, st - vec2(0, 1)).xy;
										 vec2 vT = texture2DRect(Velocity, st + vec2(0, 1)).xy;
										 vec2 diffusion = (vC + Alpha * (vL + vR + vB + vT)) / Beta;
										 gl_FragColor = vec4(diffusion, 0.0, 0.0);
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= linkProgram();
		}
		
		void glFour() {
			fragmentShader = GLSL410(
									 uniform sampler2DRect Velocity;
									 
									 uniform float Alpha;
									 uniform float Beta;
									 
									 in vec2 texCoordVarying;
									 out vec4 fragColor;
									 
									 void main(){
										 vec2 st = texCoordVarying;
										 vec2 vC = texture(Velocity, st).xy;
										 vec2 vL = texture(Velocity, st - vec2(1, 0)).xy;
										 vec2 vR = texture(Velocity, st + vec2(1, 0)).xy;
										 vec2 vB = texture(Velocity, st - vec2(0, 1)).xy;
										 vec2 vT = texture(Velocity, st + vec2(0, 1)).xy;
										 vec2 diffusion = (vC + Alpha * (vL + vR + vB + vT)) / Beta;
										 fragColor = vec4(diffusion, 0.0, 0.0);
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= bindDefaults();
			bInitialized *= linkProgram();
		}
		
	public:
		void update(ofFbo& _fbo, ofTexture& _backTex, float _viscosity){
			_fbo.begin();
			begin();
			float alpha = _viscosity;
			float beta = 1.0 + 4. * alpha;
			setUniform1f("Alpha", alpha);
			setUniform1f("Beta", beta);
			setUniformTexture("Velocity", _backTex, 0);
			renderFrame(_fbo.getWidth(), _fbo.getHeight());
			end();
			_fbo.end();
		}
	};
}


