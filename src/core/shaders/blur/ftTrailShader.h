
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {
	
	class ftTrailShader : public ftShader {
	public:
		ftTrailShader(){
			bInitialized = 1;
			if (ofIsGLProgrammableRenderer()) { glThree(); } else { glTwo(); }
			string shaderName = "ftTrailShader";
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
										 vec4 color0 = texture2DRect(tex0, gl_TexCoord[0].st);
										 vec4 color1 = texture2DRect(tex1, gl_TexCoord[0].st);
										 gl_FragColor = mix(color0, color1, weight);
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= linkProgram();
		}
		
		void glThree() {
			fragmentShader = GLSL410(
									 uniform sampler2DRect tex0;
									 uniform sampler2DRect tex1;
									 uniform float weight;
									 
									 in vec2 texCoordVarying;
									 out vec4 fragColor;
									 
									 void main(){
										 vec4 color0 = texture(tex0, texCoordVarying);
										 vec4 color1 = texture(tex1, texCoordVarying);
										 fragColor = mix(color0, color1, weight);
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


