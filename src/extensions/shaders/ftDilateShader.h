
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {
	
	class ftDilateShader : public ftShader {
	public:
		ftDilateShader(){
			bInitialized = 1;
			if (ofIsGLProgrammableRenderer()) { glThree(); } else { glTwo(); }
			string shaderName = "ftDilateShader";
			if (bInitialized) { ofLogVerbose(shaderName + " initialized"); }
			else { ofLogWarning(shaderName + " failed to initialize"); }
		}
		
	protected:
		void glTwo() {
			fragmentShader = GLSL120(
									 uniform sampler2DRect tex0;
									 
									 void main(){
										 vec2 st = gl_TexCoord[0].st;
										 gl_FragColor = max(max(max(max(max(max(max(max(texture2DRect(tex0, texCoordVarying),
																						texture2DRect(tex0, texCoordVarying + vec2(1,0))),
																					texture2DRect(tex0, texCoordVarying - vec2(1,0))),
																				texture2DRect(tex0, texCoordVarying + vec2(0,1))),
																			texture2DRect(tex0, texCoordVarying - vec2(0,1))),
																		texture2DRect(tex0, texCoordVarying + vec2(1,1))),
																	texture2DRect(tex0, texCoordVarying - vec2(1,1))),
																texture2DRect(tex0, texCoordVarying + vec2(1,-1))),
															texture2DRect(tex0, texCoordVarying - vec2(1,-1)));
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= linkProgram();
		}
		
		void glThree() {
			fragmentShader = GLSL410(
									 uniform sampler2DRect tex0;
									 
									 in vec2 texCoordVarying;
									 out vec4 fragColor;
									 
									 void main(){
										 vec2 st = texCoordVarying;
										 fragColor = max(max(max(max(max(max(max(max(texture(tex0, texCoordVarying),
																					 texture(tex0, texCoordVarying + vec2(1,0))),
																				 texture(tex0, texCoordVarying - vec2(1,0))),
																			 texture(tex0, texCoordVarying + vec2(0,1))),
																		 texture(tex0, texCoordVarying - vec2(0,1))),
																	 texture(tex0, texCoordVarying + vec2(1,1))),
																 texture(tex0, texCoordVarying - vec2(1,1))),
															 texture(tex0, texCoordVarying + vec2(1,-1))),
														 texture(tex0, texCoordVarying - vec2(1,-1)));
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= bindDefaults();
			bInitialized *= linkProgram();
		}
		
	public:
		void update(ofFbo& _fbo, ofTexture& _tex){
			_fbo.begin();
			begin();
			setUniformTexture("tex0", _tex, 0);
			renderFrame(_fbo.getWidth(), _fbo.getHeight());
			end();
			_fbo.end();
		}
	};
}

