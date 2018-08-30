
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {
	
	class ftNormalizationShader : public ftShader {
	public:
		ftNormalizationShader() {
			bInitialized = true;
			if (ofIsGLProgrammableRenderer()) { glThree(); } else { glTwo(); }
			string shaderName = "ftNormalizationShader";
			if (bInitialized) { ofLogVerbose(shaderName + " initialized"); }
			else { ofLogWarning(shaderName + " failed to initialize"); }
		}
		
	protected:
		void glTwo() {
			fragmentShader = GLSL120(
									 uniform sampler2DRect Texture;
									 uniform float Min;
									 uniform float Range;
									 uniform vec2	Scale;
									 
									 void main(){
										 vec2 st = gl_TexCoord[0].st;
										 vec2 st2 = st * Scale;
										 vec4 color = texture2DRect(Texture, st);
										 float magnitude = length(color) - Min;
										 color += TINY;
										 color = normalize(color) * (magnitude / Range);
										 gl_FragColor = color ;
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= linkProgram();
		}
		
		void glThree() {
			fragmentShader = GLSL150(
									 uniform sampler2DRect Texture;
									 uniform float Min;
									 uniform float Range;
									 uniform vec2	Scale;
									 
									 in vec2 texCoordVarying;
									 out vec4 fragColor;
									 
									 void main(){
										 vec2 st = texCoordVarying;
										 vec2 st2 = st * Scale;
										 vec4 color = texture(Texture, st);
										 float magnitude = length(color) - Min;
										 color += TINY;
										 color = normalize(color) * (magnitude / Range);
										 fragColor = color ;
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= bindDefaults();
			bInitialized *= linkProgram();
		}
		
	public:
		void update(ofFbo& _fbo, ofTexture& _srcTex, float _min, float _range){
			_fbo.begin();
			begin();
			setUniformTexture("Texture", _srcTex, 0);
			setUniform1f("Min", _min);
			setUniform1f("Range", _range);
			setUniform2f("Scale", _srcTex.getWidth() / _fbo.getWidth(), _srcTex.getHeight()/ _fbo.getHeight());
			renderFrame(_fbo.getWidth(), _fbo.getHeight());
			end();
			_fbo.end();
		}
	};
}

