
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {
	
	class ftJacobiEdgeShader : public ftShader {
	public:
		ftJacobiEdgeShader() {
			bInitialized = 1;
			if (ofIsGLProgrammableRenderer()) { glThree(); } else { glTwo(); }
			string shaderName = "ftJacobiEdgeShader";
			if (bInitialized) { ofLogVerbose(shaderName + " initialized"); }
			else { ofLogWarning(shaderName + " failed to initialize"); }
			load("tempShader/ftVertexShader.vert", "tempShader/" + shaderName + ".frag");
		}
		
	protected:
		void glTwo() {
			fragmentShader = GLSL120(
									 uniform sampler2DRect Pressure;
									 uniform sampler2DRect Divergence;
									 uniform sampler2DRect Edge;
									 
									 void main() {
										 vec2 st = gl_TexCoord[0].st;
										 vec2 offset = texture2DRect(Edge, st).xy;
										 st+= offset;
										 float D = texture2DRect(Divergence, st ).x;
										 float pL = texture2DRect(Pressure, st - vec2(1, 0)).x;
										 float pR = texture2DRect(Pressure, st + vec2(1, 0)).x;
										 float pB = texture2DRect(Pressure, st - vec2(0, 1)).x;
										 float pT = texture2DRect(Pressure, st + vec2(0, 1)).x;
										 float alpha = -1;
										 float beta = 0.25;
										 float pres = (alpha * D + pL + pR + pB + pT) * beta;
										 gl_FragColor = vec4(pres, 0.0, 0.0, 0.0);
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= linkProgram();
		}
		
		void glThree() {
			fragmentShader = GLSL410(
									 uniform sampler2DRect Pressure;
									 uniform sampler2DRect Divergence;
									 uniform sampler2DRect Edge;
									 
									 in vec2 texCoordVarying;
									 out vec4 fragColor;
									 
									 void main() {
										 vec2 st = texCoordVarying;
										 vec2 offset = texture(Edge, st).xy;
										 st+= offset;
										 float D = texture(Divergence, st ).x;
										 float pL = texture(Pressure, st - vec2(1, 0)).x;
										 float pR = texture(Pressure, st + vec2(1, 0)).x;
										 float pB = texture(Pressure, st - vec2(0, 1)).x;
										 float pT = texture(Pressure, st + vec2(0, 1)).x;
										 float alpha = -1;
										 float beta = 0.25;
										 float pres = (alpha * D + pL + pR + pB + pT) * beta;
										 fragColor = vec4(pres, 0.0, 0.0, 0.0);
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= bindDefaults();
			bInitialized *= linkProgram();
		}
		
	public:
		void update(ofFbo& _fbo, ofTexture& _backTex, ofTexture& _divTexture, ofTexture& _edgTexture, float _speed){
			_fbo.begin();
			begin();
//			setUniform1f("Alpha", -_cellSize * _cellSize);
			//			setUniform1f("InverseBeta", _inverseBeta);
			setUniformTexture("Pressure", _backTex, 0);
			setUniformTexture("Divergence", _divTexture, 1);
			setUniformTexture("Edge", _edgTexture, 2);
//			setUniform1f( "Alpha", -((1.0/_speed) * 1.0/_speed)));
			setUniform1f( "Speed", _speed);
			setUniform1f( "Beta", 0.25);
			renderFrame(_fbo.getWidth(), _fbo.getHeight());
			end();
			_fbo.end();
		}
	};
}

