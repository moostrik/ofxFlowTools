
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {
	
	class ftDecayShader : public ftShader {
	public:
		ftDecayShader(){
            bInitialized = 1;
			if (ofIsGLProgrammableRenderer()) { glThree(); } else { glTwo(); }
			string shaderName = "ftDecayShader";
			if (bInitialized) { ofLogVerbose(shaderName + " initialized"); }
			else { ofLogWarning(shaderName + " failed to initialize"); }
		}
		
	protected:
		void glTwo() {
			fragmentShader = GLSL120(
									 uniform sampler2DRect tex0;
									 uniform sampler2DRect tex1;
									 uniform float decay;
									 
									 void main(){
										 vec4 color0 = texture2DRect(tex0,gl_TexCoord[0].st);
										 vec4 color1 = texture2DRect(tex1,gl_TexCoord[0].st);
										 color0 *= vec4(1.0 - decay);
										 
										 gl_FragColor = color0 + color1;
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= linkProgram();
		}
		
		void glThree() {
			fragmentShader = GLSL410(
									 uniform sampler2DRect tex0;
									 uniform sampler2DRect tex1;
									 uniform float decay;
									 
									 in vec2 texCoordVarying;
									 out vec4 fragColor;
									 
									 void main(){
										 vec4 color0 = texture(tex0, texCoordVarying);
										 vec4 color1 = texture(tex1, texCoordVarying);
										 color0 *= vec4(1.0 - decay);
										 
										 fragColor = color0 + color1;
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= bindDefaults();
			bInitialized *= linkProgram();
		}
		
	public:
		void update(ofFbo& _fbo, ofTexture& _backTex,  ofTexture& _srcTex, float _decay){
			_fbo.begin();
			begin();
			setUniformTexture("tex0", _backTex, 0);
			setUniformTexture("tex1", _srcTex, 1);
			setUniform1f("decay", max(_decay, 0.0f));
			renderFrame(_fbo.getWidth(), _fbo.getHeight());
			end();
			_fbo.end();
		}
	};
}

