
#pragma once
#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {
	
	class ftRGB2LuminanceShader : public ftShader {
	public:
		ftRGB2LuminanceShader(){
            bInitialized = 1;
            if (ofIsGLProgrammableRenderer()) { glThree(); } else { glTwo(); }
			
			if (bInitialized)
				ofLogVerbose("ftLuminanceShader initialized");
			else
				ofLogWarning("ftLuminanceShader failed to initialize");
		}
		
	protected:
		void glTwo() {
			fragmentShader = GLSL120(
									 uniform sampler2DRect tex0;
									 uniform vec2 scale0;
									 
									 void main(){
										 vec4 color = texture2DRect(tex0, gl_TexCoord[0].st * scale0);
										 float p = 0.3 * color.r + 0.59 * color.g + 0.11 * color.b;
										 color.xyz = vec3(p,p,p);
										 gl_FragColor =  color;
									 }
									 );
			
			bInitialized *= shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.bindDefaults();
			bInitialized *= shader.linkProgram();
		}
		
		void glThree() {
			fragmentShader = GLSL150(
									 uniform sampler2DRect tex0;
									 uniform vec2 scale0;
									 
									 in vec2 texCoordVarying;
									 out vec4 fragColor;
									 
									 void main(){
										 vec4 color = texture(tex0, texCoordVarying * scale0);
										 float p = 0.3 * color.r + 0.59 * color.g + 0.11 * color.b;
										 color.xyz = vec3(p,p,p);
										 fragColor =  color;
									 }
									 );
			
			bInitialized *= shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.bindDefaults();
			bInitialized *= shader.linkProgram();
		}
		
	public:
		
		void update(ofFbo& _fbo, ofTexture& _srcTex){
			_fbo.begin();
			shader.begin();
			shader.setUniformTexture("tex0" , _srcTex, 0 );
			shader.setUniform2f("scale0", _srcTex.getWidth() / _fbo.getWidth(), _srcTex.getHeight() / _fbo.getHeight());
			renderFrame(_fbo.getWidth(), _fbo.getHeight());
			shader.end();
			_fbo.end();
		}
	};
}
