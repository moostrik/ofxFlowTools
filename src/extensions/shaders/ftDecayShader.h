
#pragma once
#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {
	
	class ftDecayShader : public ftShader {
	public:
		ftDecayShader(){
            bInitialized = 1;
            if (ofIsGLProgrammableRenderer()) { glThree(); } else { glTwo(); }
			
            if (bInitialized)
                ofLogVerbose("ftDecayShader initialized");
			else
				ofLogWarning("ftDecayShader failed to initialize");
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
			
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.linkProgram();
			
		}
		
		void glThree() {
			
			fragmentShader = GLSL150(
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
			
			bInitialized *= shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.bindDefaults();
			bInitialized *= shader.linkProgram();
			
		}
		
	public:
		void update(ofFbo& _fbo, ofTexture& _backTex,  ofTexture& _srcTex, float _decay){
			_fbo.begin();
			shader.begin();
			shader.setUniformTexture("tex0", _backTex, 0);
			shader.setUniformTexture("tex1", _srcTex, 1);
			shader.setUniform1f("decay", max(_decay, 0.0f));
			renderFrame(_fbo.getWidth(), _fbo.getHeight());
			shader.end();
			_fbo.end();
		}
	};
}
