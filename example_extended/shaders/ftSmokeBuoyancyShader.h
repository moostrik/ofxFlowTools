
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {
	
	class ftSmokeBuoyancyShader : public ftShader {
	public:
		ftSmokeBuoyancyShader() {
			bInitialized = true;
			if (ofIsGLProgrammableRenderer()) { glThree(); } else { glTwo(); }
			string shaderName = "ftSmokeBuoyancyShader";
			if (bInitialized) { ofLogVerbose(shaderName + " initialized"); }
			else { ofLogWarning(shaderName + " failed to initialize"); }
		}
		
	protected:
		void glTwo() {
			fragmentShader = GLSL120(
									 uniform sampler2DRect Velocity;
									 uniform sampler2DRect Temperature;
									 uniform sampler2DRect Density;
									 
									 uniform float AmbientTemperature;
									 uniform float TimeStep;
									 uniform float Sigma;
									 uniform float Kappa;
									 
									 uniform vec2  Gravity;
									 
									 void main(){
										 vec2 st = gl_TexCoord[0].st;
										 
										 float T = texture2DRect(Temperature, st).r;
										 vec2 V = texture2DRect(Velocity, st).rg;
										 
										 //   gl_FragColor = vec4(0);
										 //   if (T > AmbientTemperature) {
										 float D = length(texture2DRect(Density, st).rgb);
										 gl_FragColor = vec4((TimeStep * (T - AmbientTemperature) * Sigma - D * Kappa ) * Gravity, 0.0, 0.0);
										 //   }
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= linkProgram();
		}
		
		void glThree() {
			fragmentShader = GLSL150(
									 uniform sampler2DRect Velocity;
									 uniform sampler2DRect Temperature;
									 uniform sampler2DRect Density;
									 
									 uniform float AmbientTemperature;
									 uniform float TimeStep;
									 uniform float Sigma;
									 uniform float Kappa;
									 
									 uniform vec2  Gravity;
									 
									 in vec2 texCoordVarying;
									 out vec4 fragColor;
									 
									 void main(){
										 vec2 st = texCoordVarying;
										 
										 float T = texture(Temperature, st).r;
										 vec2 V = texture(Velocity, st).rg;
										 
										 //   gl_FragColor = vec4(0);
										 //   if (T > AmbientTemperature) {
										 float D = length(texture(Density, st).rgb);
										 fragColor = vec4((TimeStep * (T - AmbientTemperature) * Sigma - D * Kappa ) * Gravity, 0.0, 0.0);
										 //   }
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= bindDefaults();
			bInitialized *= linkProgram();
		}
		
	public:
		void update(ofFbo& _fbo, ofTexture& _velTex, ofTexture& _temTex, ofTexture _colorTexture, float _ambientTemperature, float _timeStep, float _smokeBuoyancy, float _smokeWeight, glm::vec2 _gForce){
			_fbo.begin();
			begin();
			setUniform1f("AmbientTemperature", _ambientTemperature);
			setUniform1f("TimeStep", _timeStep);
			setUniform1f("Sigma", _smokeBuoyancy);
			setUniform1f("Kappa", _smokeWeight);
			setUniform2f("Gravity", _gForce);
			setUniformTexture("Velocity", _velTex, 0);
			setUniformTexture("Temperature", _temTex, 1);
			setUniformTexture("Density", _colorTexture, 2);
			renderFrame(_fbo.getWidth(), _fbo.getHeight());
			end();
			_fbo.end();
		}
	};
}


