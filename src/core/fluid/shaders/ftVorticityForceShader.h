
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
									 uniform sampler2DRect tex_curl;
									 
									 uniform float halfrdx;
									 uniform float timestep;
									 uniform float vorticity;
									 
									 void main(){
										 vec2 posn = gl_TexCoord[0].st;
										 
										 // curl
										 float cT = abs(texture2DRect(tex_curl , posn + ivec2(0,1)).x);
										 float cB = abs(texture2DRect(tex_curl , posn - ivec2(0,1)).x);
										 float cR = abs(texture2DRect(tex_curl , posn + ivec2(1,0)).x);
										 float cL = abs(texture2DRect(tex_curl , posn - ivec2(1,0)).x);
										 float cC =     texture2DRect(tex_curl , posn              ).x;
										 
										 // normalize
										 vec2 dw = normalize(halfrdx * vec2(cT - cB, cR - cL) + 0.000001) * vec2(-1, 1);
										 
										 // vorticity confinement
										 vec2 fvc = dw * cC *  timestep * vorticity;
										 
										 gl_FragColor = vec4(fvc, 0.0, 0.0);
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
									 
									 uniform sampler2DRect tex_curl;
									 
									 uniform float halfrdx;
									 uniform float timestep;
									 uniform float vorticity;
									 
									 void main(){
										 vec2 posn = texCoordVarying;
										 
										 // curl
										 float cT = abs(textureOffset(tex_curl , posn, + ivec2(0,1)).x);
										 float cB = abs(textureOffset(tex_curl , posn, - ivec2(0,1)).x);
										 float cR = abs(textureOffset(tex_curl , posn, + ivec2(1,0)).x);
										 float cL = abs(textureOffset(tex_curl , posn, - ivec2(1,0)).x);
										 float cC =     texture      (tex_curl , posn              ).x;
										 
										 // normalize
										 vec2 dw = normalize(halfrdx * vec2(cT - cB, cR - cL) + 0.000001) * vec2(-1, 1);
										 
										 // vorticity confinement
										 vec2 fvc = dw * cC *  timestep * vorticity;
										 
										 glFragColor = fvc;
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= bindDefaults();
			bInitialized *= linkProgram();
		}
		
	public:
		void update(ofFbo& _fbo, ofTexture& _curlTex, float _timeStep, float _gridScale, float _vorticity){
			_fbo.begin();
			ofClear(0);
			begin();
			setUniform1f		("halfrdx",			0.5f / _gridScale);
			setUniform1f		("timestep",		_timeStep);
			setUniform1f		("vorticity",		_vorticity);
			setUniformTexture	("tex_curl",		_curlTex,	1);
			renderFrame(_fbo.getWidth(), _fbo.getHeight());
			end();
			_fbo.end();
		};
	};
}


