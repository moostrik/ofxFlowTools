
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {
	
	class ftApplyObstacleDensityShader : public ftShader {
	public:
		ftApplyObstacleDensityShader() {
			bInitialized = 1;
			if (ofIsGLProgrammableRenderer()) { glThree(); } else { glTwo(); }
			string shaderName = "ftApplyObstacleDensityShader";
			if (bInitialized) { ofLogVerbose(shaderName + " initialized"); }
			else { ofLogWarning(shaderName + " failed to initialize"); }
//			load("tempShader/ftVertexShader.vert", "tempShader/" + shaderName + ".frag");
		}
		
	protected:
		void glTwo() {
			fragmentShader = GLSL120(
									 uniform sampler2DRect Density;
									 uniform sampler2DRect Obstacle;
									 
									 uniform vec2	Scale;
									 
									 void main() {
										 vec2 st = gl_TexCoord[0].st;
										 vec2 st2 = st * Scale;
										 vec4 den = texture2DRect(Density, st);
										 float invObs = 1.0 - texture2DRect(Obstacle, st2).x;
										 gl_FragColor = den * vec4(invObs);
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= linkProgram();
		}
		
		void glThree() {
			fragmentShader = GLSL410(
									 uniform sampler2DRect	Density;
									 uniform sampler2DRect	Obstacle;
									 
									 uniform vec2	Scale;
									 
									 in vec2 texCoordVarying;
									 out vec4 fragColor;
									 
									 void main() {
										 vec2 st = texCoordVarying;
										 vec2 st2 = st * Scale;
										 vec4 den = texture(Density, st);
										 float invObs = 1.0 - texture(Obstacle, st2).x;
										 fragColor = den * vec4(invObs);
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= bindDefaults();
			bInitialized *= linkProgram();
		}
		
	public:
		void update(ofFbo& _fbo, ofTexture& _denTex, ofTexture& _obsTex){
			_fbo.begin();
			begin();
			setUniformTexture("Density", _denTex, 0);
			setUniformTexture("Obstacle", _obsTex, 1);
			setUniform2f("Scale", _obsTex.getWidth() / _fbo.getWidth(), _obsTex.getHeight()/ _fbo.getHeight());
			renderFrame(_fbo.getWidth(), _fbo.getHeight());
			end();
			_fbo.end();
		}
	};
}





