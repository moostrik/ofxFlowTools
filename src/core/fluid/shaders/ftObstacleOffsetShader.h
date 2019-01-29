
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {
	
	class ftObstacleOffsetShader : public ftShader {
	public:
		ftObstacleOffsetShader() {
			bInitialized = 1;
			if (ofIsGLProgrammableRenderer()) { glThree(); } else { glTwo(); }
			string shaderName = "ftObstacleOffsetShader";
			if (bInitialized) { ofLogVerbose(shaderName + " initialized"); }
			else { ofLogWarning(shaderName + " failed to initialize"); }
//			load("tempShader/ftVertexShader.vert", "tempShader/" + shaderName + ".frag");
		}
		
	protected:
		void glTwo() {
			fragmentShader = GLSL120(
									 uniform sampler2DRect Obstacle;
									 
									 void main(){
										 vec2 st = gl_TexCoord[0].st;
										 
										 float off = 1;
										 vec2 off_x = vec2(off, 0.0);
										 vec2 off_y = vec2(0.0, off);
										 
										// calculate the gradient
										 float gradx; float grady; float gradmag;
										 gradx = texture2DRect(Obstacle, st - off_x).x - texture2DRect(Obstacle, st + off_x).x;
										 grady = texture2DRect(Obstacle, st - off_y).x - texture2DRect(Obstacle, st + off_y).x;
										 gradmag = sqrt((gradx*gradx) + (grady*grady) + 0.0001);
										 
										 vec2 edgeOffset;
										 float invObs = floor(1.0 - texture2DRect(Obstacle, st).x + 0.5);
										 edgeOffset.x = invObs * floor(gradx/gradmag + 0.5);
										 edgeOffset.y = invObs * floor(grady/gradmag + 0.5);
										 float hasOffset = max(abs(edgeOffset.x), abs(edgeOffset.y));
										 float posNegOrZero = mix(invObs, -1.0, hasOffset);
										 
										 gl_FragColor = vec4(edgeOffset, posNegOrZero, 0);
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
									 out vec4 glFragColor;
									 
									 uniform sampler2DRect tex_obstacleC;
									 
									 void main(){
										 vec2 posn = texCoordVarying;
										 glFragColor = vec4(0);
										 glFragColor.x = textureOffset(tex_obstacleC, posn, + ivec2(0,1)).x;
										 glFragColor.y = textureOffset(tex_obstacleC, posn, - ivec2(0,1)).x;
										 glFragColor.z = textureOffset(tex_obstacleC, posn, + ivec2(1,0)).x;
										 glFragColor.w = textureOffset(tex_obstacleC, posn, - ivec2(1,0)).x;
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



