
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
									 uniform float HalfInverseCellSize;
									 
									 void v2TexNeighbors(sampler2DRect tex, vec2 st,
														 out vec2 left, out vec2 right, out vec2 bottom, out vec2 top) {
										 left   = texture2DRect(tex, st - vec2(1, 0)).xy;
										 right  = texture2DRect(tex, st + vec2(1, 0)).xy;
										 bottom = texture2DRect(tex, st - vec2(0, 1)).xy;
										 top    = texture2DRect(tex, st + vec2(0, 1)).xy;
									 }
									 
									 void fTexNeighbors(sampler2DRect tex, vec2 st,
														out float left, out float right, out float bottom, out float top) {
										 left   = texture2DRect(tex, st - vec2(1, 0)).x;
										 right  = texture2DRect(tex, st + vec2(1, 0)).x;
										 bottom = texture2DRect(tex, st - vec2(0, 1)).x;
										 top    = texture2DRect(tex, st + vec2(0, 1)).x;
									 }
									 
									 void main(){
										 
										 vec2 st = gl_TexCoord[0].st;
										 
										 float voL; float voR; float voB; float voT;
										 fTexNeighbors (Vorticity, st, voL, voR, voB, voT);
										 float voC = texture2DRect(Vorticity, st).x;
										 
										 vec2 force = HalfInverseCellSize * vec2(abs(voT) - abs(voB), abs(voR) - abs(voL));
										 
										 // safe normalize
										 const float EPSILON = 2.4414e-4; // 2^-12
										 
										 float magSqr = max(EPSILON, dot(force, force));
										 force = force * inversesqrt(magSqr);
										 
										 force *= ConfinementScale * voC * vec2(1., -1.);
										 force *= TimeStep;
										 
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


