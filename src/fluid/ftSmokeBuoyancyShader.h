
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {

	class ftSmokeBuoyancyShader : public ftShader {
	public:
		ftSmokeBuoyancyShader() {
			bInitialized = 1;
			
			if (ofGetGLProgrammableRenderer())
				glThree();
			else
				glTwo();
			
			if (bInitialized)
				ofLogNotice("ftSmokeBuoyancyShader initialized");
			else
				ofLogWarning("ftSmokeBuoyancyShader failed to initialize");
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
			
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.linkProgram();
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
			
			
			bInitialized *= shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.bindDefaults();
			bInitialized *= shader.linkProgram();
		}
		
	public:
		
		void update(ofFbo& _buffer, ofTexture& _velocityTexture, ofTexture& _temperatureTexture, ofTexture _colorTexture, float _ambientTemperature, float _timeStep, float _smokeBuoyancy, float _smokeWeight, ofVec2f _gForce){
			
			_buffer.begin();
			shader.begin();
			shader.setUniform1f("AmbientTemperature", _ambientTemperature);
			shader.setUniform1f("TimeStep", _timeStep);
			shader.setUniform1f("Sigma", _smokeBuoyancy);
			shader.setUniform1f("Kappa", _smokeWeight);
			
			shader.setUniform2f("Gravity", (float)_gForce.x, (float)_gForce.y );
			
			shader.setUniformTexture("Velocity", _velocityTexture, 0);
			shader.setUniformTexture("Temperature", _temperatureTexture, 1);
			shader.setUniformTexture("Density", _colorTexture, 2);
			
			renderFrame(_buffer.getWidth(), _buffer.getHeight());
			
			shader.end();
			_buffer.end();
		}

		
		
	private:
	};
}