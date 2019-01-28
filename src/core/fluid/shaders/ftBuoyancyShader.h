
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {
	
	class ftBuoyancyShader : public ftShader {
	public:
		ftBuoyancyShader() {
			bInitialized = 1;
			if (ofIsGLProgrammableRenderer()) { glThree(); } else { glTwo(); }
			string shaderName = "ftBuoyancyShader";
			if (bInitialized) { ofLogVerbose(shaderName + " initialized"); }
			else { ofLogWarning(shaderName + " failed to initialize"); }
			load("tempShader/ftVertexShader.vert", "tempShader/" + shaderName + ".frag");
		}
		
	protected:
		void glTwo() {
			fragmentShader = GLSL120(
									 uniform sampler2DRect Temperature;
									 uniform sampler2DRect Density;
									 
									 uniform float AmbientTemperature;
									 uniform float TimeStep;
									 uniform float Sigma;
									 uniform float Kappa;
									 
									 uniform vec2  Gravity;
									 
									 void main(){
										 vec2 st = gl_TexCoord[0].st;
										 float temp = texture2DRect(Temperature, st).r;
										 vec2 buoyancy = vec2(0.0);
										 if (temp > AmbientTemperature) {
											 float D = length(texture2DRect(Density, st).rgb);
											 buoyancy = vec2((TimeStep * (temp - AmbientTemperature) * Sigma - D * Kappa ) * Gravity);
										 }
										 gl_FragColor = vec4(buoyancy, 0.0, 0.0);
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= linkProgram();
		}
		
		void glThree() {
			fragmentShader = GLSL410(
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
										 float temp = texture(Temperature, st).r;
										 vec2 buoyancy = vec2(0.0);
										 if (temp > AmbientTemperature) {
											 float D = length(texture(Density, st).rgb);
											 buoyancy = vec2((TimeStep * (temp - AmbientTemperature) * Sigma - D * Kappa ) * Gravity);
										 }
										 fragColor = vec4(buoyancy, 0.0, 0.0);
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= bindDefaults();
			bInitialized *= linkProgram();
		}
		
	public:
		void update(ofFbo& _fbo, ofTexture& _velTex, ofTexture& _temTex, ofTexture _denTex, float _timeStep, float _ambientTemperature, float _smokeBuoyancy, float _smokeWeight){
			_fbo.begin();
			begin();
			setUniform1f		("temperature_ambient", _ambientTemperature);
			setUniform1f		("timestep",			_timeStep);
			setUniform1f		("fluid_buoyancy",		_smokeBuoyancy);
			setUniform1f		("fluid_weight",		_smokeWeight);
			setUniformTexture	("tex_velocity",		_velTex,	0);
			setUniformTexture	("tex_temperature",		_temTex,	1);
			setUniformTexture	("tex_density",			_denTex,	2);
			renderFrame(_fbo.getWidth(), _fbo.getHeight());
			end();
			_fbo.end();
		}
	};
}



