
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {
	
	class ftVorticityForceShader : public ftShader {
	public:
		ftVorticityForceShader() {
			bInitialized = 1;
			if (ofIsGLProgrammableRenderer()) { glThree(); } else { glTwo(); }
			string shaderName = "ftVorticityForceShader";
			if (bInitialized) { ofLogVerbose(shaderName + " initialized"); }
			else { ofLogWarning(shaderName + " failed to initialize"); }
//			load("tempShader/ftVertexShader.vert", "tempShader/" + shaderName + ".frag");
		}
		
	protected:
		void glTwo() {
			fragmentShader = GLSL120(
									 uniform sampler2DRect Vorticity;
									 
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
										 force *= inversesqrt(magSqr) * ConfinementScale * vorC * vec2(1., -1.);
										 gl_FragColor = vec4(force, 0.0, 0.0);
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= linkProgram();
		}
		
		void glThree() {
			fragmentShader = GLSL410(
									 precision mediump float;
									 precision mediump int;
									 
									 in vec2 texCoordVarying;
									 out vec2 glFragColor;
									 
									 uniform sampler2DRect tex_velocity;
									 uniform sampler2DRect tex_curl;
									 
									 uniform float halfrdx;
									 uniform float timestep;
									 uniform float vorticity;
									 
									 void main(){
										 vec2 posn = texCoordVarying;
										 
										 // velocity
										 vec2 vOld = texture(tex_velocity, posn).xy;
										 
										 // curl
										 float cT = abs(textureOffset(tex_curl    , posn, + ivec2(0,1)).x);
										 float cB = abs(textureOffset(tex_curl    , posn, - ivec2(0,1)).x);
										 float cR = abs(textureOffset(tex_curl    , posn, + ivec2(1,0)).x);
										 float cL = abs(textureOffset(tex_curl    , posn, - ivec2(1,0)).x);
										 float cC =     texture      (tex_curl    , posn              ).x;
										 
										 // normalize
										 vec2 dw = normalize(halfrdx * vec2(cT - cB, cR - cL) + 0.000001) * vec2(-1, 1);
										 
										 // vorticity confinement
										 vec2 fvc = dw * cC *  timestep * vorticity;
										 
										 // add to velocity
										 vec2 vNew = vOld + fvc;
										 
										 glFragColor = vNew;
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= bindDefaults();
			bInitialized *= linkProgram();
		}
		
	public:
		void update(ofFbo& _fbo, ofTexture& _velTex, ofTexture& _curlTex, float _timeStep, float _gridScale, float _vorticity){
			_fbo.begin();
			ofClear(0);
			begin();
			setUniform1f		("halfrdx",			0.5f / _gridScale);
			setUniform1f		("timestep",		_timeStep);
			setUniform1f		("vorticity",		_vorticity);
			setUniformTexture	("tex_velocity",	_velTex,	0);
			setUniformTexture	("tex_curl",		_curlTex,	1);
			renderFrame(_fbo.getWidth(), _fbo.getHeight());
			end();
			_fbo.end();
		};
	};
}


