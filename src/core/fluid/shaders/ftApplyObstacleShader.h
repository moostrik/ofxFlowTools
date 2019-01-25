
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {
	
	class ftApplyObstacleShader : public ftShader {
	public:
		ftApplyObstacleShader() {
            bInitialized = 1;
			if (ofIsGLProgrammableRenderer()) { glThree(); } else { glTwo(); }
			string shaderName = "ftApplyObstacleShader";
			if (bInitialized) { ofLogVerbose(shaderName + " initialized"); }
			else { ofLogWarning(shaderName + " failed to initialize"); }
//			load("tempShader/ftVertexShader.vert", "tempShader/" + shaderName + ".frag");
		}
		
	protected:
		void glTwo() {
			fragmentShader = GLSL120(
									 uniform sampler2DRect SrcTex;
									 uniform sampler2DRect ObstacleOffsetTex;
									 
									 uniform float	Weight;
									 uniform vec2	Scale;
									 
									 void main(){
										 vec2 st = gl_TexCoord[0].st;
										 vec2 st2 = st * Scale;
										 vec3 obs = texture2DRect(ObstacleOffsetTex, st2).xyz;
										 vec2 offset = obs.xy;
										 float obstacle = obs.z;
										 vec4 src = texture2DRect(SrcTex, st + offset);
										 if (length(offset) > 0) {
											 gl_FragColor = src * vec4(obstacle * Weight);
										 }
										 else {
											 gl_FragColor = src * vec4(obstacle);
										 }
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= linkProgram();
		}
		
		void glThree() {
			fragmentShader = GLSL410(
									 uniform sampler2DRect	SrcTex;
									 uniform sampler2DRect	ObstacleOffsetTex;
									 
									 uniform float	Weight;
									 uniform vec2	Scale;
									 
									 in vec2 texCoordVarying;
									 out vec4 fragColor;
									 
									 void main()
									 {
										 vec2 st = texCoordVarying;
										 vec2 st2 = st * Scale;
										 vec4 obs = texture(ObstacleOffsetTex, st2);
										 vec2 offset = obs.xy;// / Scale;
										 float obstacle = obs.z;
										 bool hasOffset = bool(obs.w);
										 float w = mix(1, Weight, hasOffset);
										 fragColor = texture(SrcTex, st + offset) * vec4(obstacle * w);
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= bindDefaults();
			bInitialized *= linkProgram();
		}
		
	public:
		void update(ofFbo& _fbo, ofTexture& _srcTex, ofTexture& _obstacleOffsetTex, float _weight){
			_fbo.begin();
			begin();
			setUniformTexture("ScrTex", _srcTex, 0);
			setUniformTexture("ObstacleOffsetTex", _obstacleOffsetTex, 1);
			setUniform1f("Weight", _weight);
			setUniform2f("Scale", _obstacleOffsetTex.getWidth() / _fbo.getWidth(), _obstacleOffsetTex.getHeight()/ _fbo.getHeight());
			renderFrame(_fbo.getWidth(), _fbo.getHeight());
			end();
			_fbo.end();
		}
	};
}





