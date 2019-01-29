
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {
	
	class ftEOGShader : public ftShader {
	public:
		ftEOGShader(){
            bInitialized = 1;
			if (ofIsGLProgrammableRenderer()) { glFour(); } else { glTwo(); }
			string shaderName = "ftEOGShader";
			if (bInitialized) { ofLogVerbose(shaderName + " initialized"); }
			else { ofLogWarning(shaderName + " failed to initialize"); }
		}
		
	protected:
		void glTwo() {
			fragmentShader = GLSL120(
									 uniform sampler2DRect tex0;
									 uniform float exposure;
									 uniform float offset;
									 uniform float gamma;
									 
									 void main(){
										 vec4 color = texture2DRect(tex0,gl_TexCoord[0].st);
										 vec3 eog = pow((color.xyz * vec3(exposure)) + vec3(offset), vec3(1.0/gamma));
										 gl_FragColor = vec4(eog, color.w);
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= linkProgram();
		}
		
		void glFour() {
			fragmentShader = GLSL410(
									 uniform sampler2DRect tex0;
									 uniform float exposure;
									 uniform float offset;
									 uniform float gamma;
									 
									 in vec2 texCoordVarying;
									 out vec4 fragColor;
									 
									 void main(){
										 vec4 color = texture(tex0, texCoordVarying);
										 vec3 eog = pow((color.xyz * vec3(exposure)) + vec3(offset), vec3(1.0/gamma));
										 fragColor = vec4(eog, color.w);
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= bindDefaults();
			bInitialized *= linkProgram();
		}
		
	public:
		void update(ofFbo& _drawBuffer, ofTexture& _srcTex, float _exposure, float _offset, float _gamma){
			_drawBuffer.begin();
			begin();
			setUniformTexture( "tex0" , _srcTex, 0 );
			setUniform1f("exposure", _exposure );
			setUniform1f("offset", _offset);
			setUniform1f("gamma", _gamma);
			renderFrame(_drawBuffer.getWidth(), _drawBuffer.getHeight());
			end();
			_drawBuffer.end();
		}
		
	};
}


