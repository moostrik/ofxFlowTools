
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {
	
	class ftVorticityConfinementShader : public ftShader {
	public:
		ftVorticityConfinementShader() {
			bInitialized = true;
			if (ofIsGLProgrammableRenderer()) { glThree(); } else { glTwo(); }
			string shaderName = "ftVorticityConfinementShader";
			if (bInitialized) { ofLogVerbose(shaderName + " initialized"); }
			else { ofLogWarning(shaderName + " failed to initialize"); }
//			load("tempShader/ftVertexShader.vert", "tempShader/" + shaderName + ".frag");
		}
		
	protected:
		void glTwo() {
			fragmentShader = GLSL120(
									 uniform sampler2DRect Vorticity;
									 
									 uniform float TimeStep;
									 uniform float ConfinementScale;
									 
									 void main(){
										 vec2 st = gl_TexCoord[0].st;
										 float vorL = texture2DRect(Vorticity, st - vec2(1, 0)).x;
										 float vorR = texture2DRect(Vorticity, st + vec2(1, 0)).x;
										 float vorB = texture2DRect(Vorticity, st - vec2(0, 1)).x;
										 float vorT = texture2DRect(Vorticity, st + vec2(0, 1)).x;
										 float vorC = texture2DRect(Vorticity, st).x;
										 vec2 force = 0.5 * vec2(abs(vorT) - abs(vorB), abs(vorR) - abs(vorL));
										 const float EPSILON = 2.4414e-4; // 2^-12
										 float magSqr = max(EPSILON, dot(force, force));
										 force *= inversesqrt(magSqr) * ConfinementScale * vorC * vec2(1., -1.) * TimeStep;
										 gl_FragColor = vec4(force, 0.0, 0.0);
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= linkProgram();
		}
		
		void glThree() {
			fragmentShader = GLSL150(
									 uniform sampler2DRect Vorticity;
									 
									 uniform float TimeStep;
									 uniform float ConfinementScale;
									 
									 in vec2 texCoordVarying;
									 out vec4 fragColor;
									 
									 void main(){
										 vec2 st = texCoordVarying;
										 float vorL = texture(Vorticity, st - vec2(1, 0)).x;
										 float vorR = texture(Vorticity, st + vec2(1, 0)).x;
										 float vorB = texture(Vorticity, st - vec2(0, 1)).x;
										 float vorT = texture(Vorticity, st + vec2(0, 1)).x;
										 float vorC = texture(Vorticity, st).x;
										 vec2 force = 0.5 * vec2(abs(vorT) - abs(vorB), abs(vorR) - abs(vorL));
										 const float EPSILON = 2.4414e-4; // 2^-12
										 float magSqr = max(EPSILON, dot(force, force));
										 force *= inversesqrt(magSqr) * ConfinementScale * vorC * vec2(1., -1.) * TimeStep;
										 fragColor = vec4(force, 0.0, 0.0);
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= bindDefaults();
			bInitialized *= linkProgram();
		}
		
	public:
		void update(ofFbo& _fbo, ofTexture& _vorticityTexture, float _timeStep, float _scale){
			_fbo.begin();
			ofClear(0);
			begin();
			setUniformTexture( "Vorticity" , _vorticityTexture, 0 );
			setUniform1f("TimeStep", _timeStep);
			setUniform1f("ConfinementScale", _scale);
			renderFrame(_fbo.getWidth(), _fbo.getHeight());
			end();
			_fbo.end();
		};
	};
}


