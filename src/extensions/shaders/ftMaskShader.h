
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {
	
	class ftMaskShader : public ftShader {
	public:
		ftDecayShader(){
            bInitialized = 1;
			if (ofIsGLProgrammableRenderer()) { glFour(); } else { glTwo(); }
			string shaderName = "ftMaskShader";
			if (bInitialized) { ofLogVerbose(shaderName + " initialized"); }
			else { ofLogWarning(shaderName + " failed to initialize"); }
		}
		
	protected:
		void glTwo() {
			fragmentShader = GLSL120(
									 uniform sampler2DRect tex0;
									 uniform sampler2DRect tex1;
									 
									 void main(){
										 vec4 color = texture2DRect(tex0,gl_TexCoord[0].st);
										 float mask = texture2DRect(tex1,gl_TexCoord[0].st).x;
										 color0 *= mask;
										 gl_FragColor = color0;
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= linkProgram();
		}
		
		void glFour() {
			fragmentShader = GLSL410(
									 uniform sampler2DRect tex0;
									 uniform sampler2DRect tex1;
									 
									 in vec2 texCoordVarying;
									 out vec4 fragColor;
									 
									 void main(){
										 vec4 color = texture(tex0, texCoordVarying);
										 float mask = texture(tex1, texCoordVarying).x;
										 color0 *= mask;
										 fragColor = color0;
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= bindDefaults();
			bInitialized *= linkProgram();
		}
		
	public:
		void update(ofFbo& _fbo, ofTexture& _backTex,  ofTexture& _maskTex){
			_fbo.begin();
			begin();
			setUniformTexture("tex0", _backTex, 0);
			setUniformTexture("tex1", _maskTex, 1);
			renderFrame(_fbo.getWidth(), _fbo.getHeight());
			end();
			_fbo.end();
		}
	};
}

