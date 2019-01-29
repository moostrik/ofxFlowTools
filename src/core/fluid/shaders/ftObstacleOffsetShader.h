
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {
	
	class ftObstacleOffsetShader : public ftShader {
	public:
		ftObstacleOffsetShader() {
			bInitialized = 1;
			if (ofIsGLProgrammableRenderer()) { glFour(); } else { glTwo(); }
			string shaderName = "ftObstacleOffsetShader";
			if (bInitialized) { ofLogVerbose(shaderName + " initialized"); }
			else { ofLogWarning(shaderName + " failed to initialize"); }
//			load("tempShader/ftVertexShader.vert", "tempShader/" + shaderName + ".frag");
		}
		
	protected:
		void glTwo() {
			fragmentShader = GLSL120(
									 uniform sampler2DRect tex_obstacle;
									 
									 void main(){
										 vec2 st = gl_TexCoord[0].st;
										 gl_FragColor = vec4(0.0);
										 gl_FragColor.x = texture2DRect(tex_obstacle, st + ivec2(0,1)).x;
										 gl_FragColor.y = texture2DRect(tex_obstacle, st - ivec2(0,1)).x;
										 gl_FragColor.z = texture2DRect(tex_obstacle, st + ivec2(1,0)).x;
										 gl_FragColor.w = texture2DRect(tex_obstacle, st - ivec2(1,0)).x;
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= linkProgram();
		}
		
		void glFour() {
			fragmentShader = GLSL410(
									 precision mediump float;
									 precision mediump int;
									 
									 in vec2 texCoordVarying;
									 out vec4 glFragColor;
									 
									 uniform sampler2DRect tex_obstacle;
									 
									 void main(){
										 vec2 st = texCoordVarying;
										 glFragColor = vec4(0.0);
										 glFragColor.x = textureOffset(tex_obstacle, st, + ivec2(0,1)).x;
										 glFragColor.y = textureOffset(tex_obstacle, st, - ivec2(0,1)).x;
										 glFragColor.z = textureOffset(tex_obstacle, st, + ivec2(1,0)).x;
										 glFragColor.w = textureOffset(tex_obstacle, st, - ivec2(1,0)).x;
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= bindDefaults();
			bInitialized *= linkProgram();
		}
		
	public:
		void update(ofFbo& _fbo, ofTexture& _obsTex){
			_fbo.begin();
			begin();
			setUniformTexture("Obstacle", _obsTex, 0);
			setUniform2f("Scale", _obsTex.getWidth() / _fbo.getWidth(), _obsTex.getHeight()/ _fbo.getHeight());
			renderFrame(_fbo.getWidth(), _fbo.getHeight());
			end();
			_fbo.end();
		}
	};
}



