
#pragma once
#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {

	class ftEOGShader : public ftShader {
	public:
		ftEOGShader(){
			bInitialized = 1;
			
			if (ofGetGLProgrammableRenderer())
				glThree();
			else
				glTwo();
			
			if (bInitialized)
				ofLogNotice("ftEOGShader initialized");
			else
				ofLogWarning("ftEOGShader failed to initialize");
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
			
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.linkProgram();

		}
		
		void glThree() {
			
			fragmentShader = GLSL150(
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
			
			bInitialized *= shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.bindDefaults();
			bInitialized *= shader.linkProgram();
		}
		
	public:
				
		void update(ofFbo& _drawBuffer, ofTexture& _srcTexture, float _exposure, float _offset, float _gamma){
			_drawBuffer.begin();
			shader.begin();
			shader.setUniformTexture( "tex0" , _srcTexture, 0 );
			shader.setUniform1f("exposure", _exposure );
			shader.setUniform1f("offset", _offset);
			shader.setUniform1f("gamma", _gamma);
			renderFrame(_drawBuffer.getWidth(), _drawBuffer.getHeight());
			shader.end();
			_drawBuffer.end();
		}
		
	};
}