
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {
	
	class ftColorizeLuminanceShader : public ftShader {
	public:
		ftColorizeLuminanceShader() {
            bInitialized = 1;
			if (ofIsGLProgrammableRenderer()) { glThree(); } else { glTwo(); }
			string shaderName = "ftColorizeLuminanceShader";
			if (bInitialized) { ofLogVerbose(shaderName + " initialized"); }
			else { ofLogWarning(shaderName + " failed to initialize"); }
		}
		
	protected:
		void glTwo() {
			fragmentShader = GLSL120(
									 uniform vec4	restColor;
									 
									 void main(){
										 gl_FragColor = restColor;
										 
									 }
									 
									 );
			
			setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			linkProgram();
			
		}
		
		void glThree() {
			
			fragmentShader = GLSL410(
									 uniform sampler2DRect SourceTexture;
									 uniform sampler2DRect RampTexture;
									 uniform vec2	TextureScale;
									 uniform float	RampSize;
									 
									 in vec2 texCoordVarying;
									 out vec4 fragColor;
									 
									 void main(){
										 vec2 st = texCoordVarying;
										 
										 vec2 stTexture = st * TextureScale;
										 vec4 srcColor = texture(SourceTexture, stTexture);
										 
										 vec2 stRamp = vec2(srcColor.x * RampSize, 0.5);
										 vec4 rampColor = texture(RampTexture, stRamp);
										 
										 fragColor = rampColor;
									 }
									 );
			
			setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bindDefaults();
			linkProgram();
		}
		
	public:
		void update(ofFbo& _fbo, ofTexture _srcTex, ofTexture _rampTexture){
			_fbo.begin();
			begin();
			setUniformTexture("SourceTexture", _srcTex, 0);
			setUniformTexture("RampTexture", _rampTexture, 1);
			setUniform2f("TextureScale", _srcTex.getWidth() / _fbo.getWidth(), _srcTex.getHeight()/ _fbo.getHeight());
			setUniform1f("RampSize", _rampTexture.getWidth());
			renderFrame(_fbo.getWidth(), _fbo.getHeight());
			end();
			_fbo.end();
		}
	};
}


