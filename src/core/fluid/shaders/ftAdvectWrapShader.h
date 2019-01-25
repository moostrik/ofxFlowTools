
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {
	
	class ftAdvectWrapShader : public ftAdvectShader {
	public:
		ftAdvectWrapShader() {
			bInitialized = 1;
			if (ofIsGLProgrammableRenderer()) { glThree(); } else { glTwo(); }
			string shaderName = "ftAdvectWrapShader";
			if (bInitialized) { ofLogVerbose(shaderName + " initialized"); }
			else { ofLogWarning(shaderName + " failed to initialize"); }
//			load("tempShader/ftVertexShader.vert", "tempShader/" + shaderName + ".frag");
		}
		
	protected:
		void glTwo() override {
			fragmentShader = GLSL120(
									 uniform sampler2DRect Backbuffer;
									 uniform sampler2DRect Velocity;
									 
									 uniform float	TimeStep;
									 uniform float	Dissipation;
									 uniform vec2	Scale;
									 uniform vec2	Resolution;
									 
									 void main(){
										 vec2 st = gl_TexCoord[0].st;
										 vec2 st2 = st * Scale;
										 vec2 u = texture2DRect(Velocity, st2).rg / Scale;
										 vec2 origin = mod(st - TimeStep * u, Resolution);
										 gl_FragColor = Dissipation * texture2DRect(Backbuffer, origin);
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= linkProgram();
		}
		
		void glThree() override {
			fragmentShader = GLSL410(
									 uniform sampler2DRect Backbuffer;
									 uniform sampler2DRect Velocity;
									 
									 uniform float	TimeStep;
									 uniform float	Dissipation;
									 uniform vec2	Scale;
									 uniform vec2	Resolution;
									 
									 in vec2 texCoordVarying;
									 out vec4 fragColor;
									 
									 void main(){
										 vec2 st = texCoordVarying;
										 vec2 st2 = st * Scale;
										 vec2 u = texture(Velocity, st2).rg / Scale;
										 vec2 origin = mod(st - TimeStep * u, Resolution);
										 fragColor = Dissipation * texture(Backbuffer, origin);
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= bindDefaults();
			bInitialized *= linkProgram();
		}
		
	public:
		void update(ofFbo& _fbo, ofTexture& _backTex,  ofTexture& _velTex, float _timeStep, float _dissipation) override {
			_fbo.begin();
			begin();
			setUniform1f("TimeStep", _timeStep);
			setUniform1f("Dissipation", _dissipation);
			setUniform2f("Scale", _velTex.getWidth() / _fbo.getWidth(), _velTex.getHeight()/ _fbo.getHeight());
			setUniform2f("Resolution", _fbo.getWidth(), _fbo.getHeight());
			setUniformTexture("Backbuffer", _backTex, 0);
			setUniformTexture("Velocity", _velTex, 1);
			renderFrame(_fbo.getWidth(), _fbo.getHeight());
			end();
			_fbo.end();
		}
	};
}


