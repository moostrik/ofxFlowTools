
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {
	
	class ftBridgeShader : public ftShader {
	public:
		ftBridgeShader(){
			bInitialized = true;
			if (ofIsGLProgrammableRenderer()) { glThree(); } else { glTwo(); }
			string shaderName = "ftBridgeShader";
			if (bInitialized) { ofLogVerbose(shaderName + " initialized"); }
			else { ofLogWarning(shaderName + " failed to initialize"); }
		}
		
	protected:
		void glTwo() {
			fragmentShader = GLSL120(
									 uniform sampler2DRect tex0;
									 uniform sampler2DRect tex1;
									 uniform float weight;
									 
									 void main(){
										 vec2 baseVel = texture2DRect(tex0, gl_TexCoord[0].st).xy;
										 vec2 blendVel = texture2DRect(tex1, gl_TexCoord[0].st).xy;
										 vec2 vel = (baseVel * weight) + blendVel;
										 float magnitude = min(length(vel), 1);
										 vel = normalize(vel) * magnitude;
										 gl_FragColor = vec4(vel, 0.0, 1.0);
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= linkProgram();
		}
		
		void glThree() {
			fragmentShader = GLSL150(
									 uniform sampler2DRect tex0;
									 uniform sampler2DRect tex1;
									 uniform float weight;
									 
									 in vec2 texCoordVarying;
									 out vec4 fragColor;
									 
									 void main(){
										 vec2 baseVel = texture(tex0, texCoordVarying).xy;
										 vec2 blendVel = texture(tex1, texCoordVarying).xy;
										 vec2 vel = (baseVel * weight) + blendVel;
										 float magnitude = min(length(vel), 1);
										 vel = normalize(vel) * magnitude;
										 fragColor = vec4(vel, 0.0, 1.0);
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= bindDefaults();
			bInitialized *= linkProgram();
		}
		
	public:
		void update(ofFbo& _fbo, ofTexture& _baseTex,  ofTexture& _blendTex, float _weight){
			_fbo.begin();
			begin();
			setUniformTexture("tex0", _baseTex, 0);
			setUniformTexture("tex1", _blendTex, 1);
			setUniform1f("weight", _weight);
			renderFrame(_fbo.getWidth(), _fbo.getHeight());
			end();
			_fbo.end();
		}
	};
}


