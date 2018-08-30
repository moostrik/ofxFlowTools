
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {
	
	class ftRGB2LuminanceShader : public ftShader {
	public:
		ftRGB2LuminanceShader(){
            bInitialized = 1;
			if (ofIsGLProgrammableRenderer()) { glThree(); } else { glTwo(); }
			string shaderName = "ftLuminanceShader";
			if (bInitialized) { ofLogVerbose(shaderName + " initialized"); }
			else { ofLogWarning(shaderName + " failed to initialize"); }
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
			
			bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= bindDefaults();
			bInitialized *= linkProgram();
		}
		
		void glThree() {
			fragmentShader = GLSL410(
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
			
			bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= bindDefaults();
			bInitialized *= linkProgram();
		}
		
	public:
		void update(ofFbo& _fbo, ofTexture& _srcTex){
			_fbo.begin();
			begin();
			setUniformTexture("tex0" , _srcTex, 0 );
			setUniform2f("scale0", _srcTex.getWidth() / _fbo.getWidth(), _srcTex.getHeight() / _fbo.getHeight());
			renderFrame(_fbo.getWidth(), _fbo.getHeight());
			end();
			_fbo.end();
		}
	};
}

