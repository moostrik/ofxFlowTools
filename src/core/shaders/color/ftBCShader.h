
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {
	
	class ftContrastShader : public ftShader {
	public:
		ftContrastShader(){
            bInitialized = 1;
			if (ofIsGLProgrammableRenderer()) { glThree(); } else { glTwo(); }
			string shaderName = "ftContrastShader";
			if (bInitialized) { ofLogVerbose(shaderName + " initialized"); }
			else { ofLogWarning(shaderName + " failed to initialize"); }
		}
		
	protected:
		void glTwo() {
			fragmentShader = GLSL120(
									 uniform sampler2DRect tex0;
									 uniform float contrast;
									 uniform float brightness;
									 
									 void main(){
										 vec4 color = texture2DRect(tex0,gl_TexCoord[0].st);
										 float alpha = color.a;
										 float p = 0.3 *color.r + 0.59*color.g + 0.11*color.b;
										 p = p * brightness;
										 color = vec4(p,p,p,1.0);
										 color = mix( vec4(1.0,1.0,1.0,1.0),color,contrast);
										 
										 gl_FragColor =  vec4(color.r , color.g, color.b, alpha);
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= linkProgram();
		}
		
		void glThree() {
			fragmentShader = GLSL410(
									 uniform sampler2DRect tex0;
									 uniform float contrast;
									 uniform float brightness;
									 
									 in vec2 texCoordVarying;
									 out vec4 fragColor;
									 
									 void main(){
										 vec4 color = texture(tex0, texCoordVarying);
										 float alpha = color.a;
										 float p = 0.3 *color.r + 0.59*color.g + 0.11*color.b;
										 p = p * brightness;
										 color = vec4(p,p,p,1.0);
										 color = mix( vec4(1.0,1.0,1.0,1.0),color,contrast);
										 
										 fragColor =  vec4(color.r , color.g, color.b, alpha);
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= bindDefaults();
			bInitialized *= linkProgram();
		}
		
	public:
		void update(ofFbo& _drawBuffer, ofTexture& _srcTex, float _contrast, float _brightness){
			_drawBuffer.begin();
			begin();
			setUniformTexture( "tex0" , _srcTex, 0 );
			setUniform1f("contrast", _contrast);
			setUniform1f("brightness", _brightness);
			renderFrame(_drawBuffer.getWidth(), _drawBuffer.getHeight());
			end();
			_drawBuffer.end();
		}
	};
}


