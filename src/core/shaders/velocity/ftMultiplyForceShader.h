
#pragma once

#include "ofMain.h"
#include "ftShader.h"


namespace flowTools {
	
	class ftMultiplyForceShader : public ftShader {
	public:
		ftMultiplyForceShader() {
            bInitialized = 1;
            if (ofIsGLProgrammableRenderer()) { glThree(); } else { glTwo(); }
			
            if (bInitialized)
                ofLogVerbose("ftMultilpyForceShader initialized");
			else
				ofLogWarning("ftMultilpyForceShader failed to initialize");
		}
		
	protected:
		void glTwo() {
			fragmentShader = GLSL120(
								  uniform sampler2DRect tex0;
								  uniform float force;
								  
								  void main(){
									  vec2 st = gl_TexCoord[0].st;
									  vec4 color = texture2DRect(tex0, st) * force;
									  gl_FragColor = color ;
								  }
								  
								  );
			
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.linkProgram();
		}
		
		void glThree() {
			fragmentShader = GLSL150(
									 uniform sampler2DRect tex0;
								  uniform float force;
								  
								  in vec2 texCoordVarying;
								  out vec4 fragColor;
								  
								  void main(){
									  vec2 st = texCoordVarying;
									  vec4 color = texture(tex0, st) * force;
									  fragColor = color ;
								  }
								  );
			
			bInitialized *= shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.bindDefaults();
			bInitialized *= shader.linkProgram();
		}
		
	public:
		
		void update(ofFbo& _fbo, ofTexture& _srcTex, float _force){
			_fbo.begin();
			shader.begin();
			shader.setUniformTexture("tex0", _srcTex, 0);
			shader.setUniform1f("force", _force);
			renderFrame(_fbo.getWidth(), _fbo.getHeight());
			shader.end();
			_fbo.end();
		}
	};
}
