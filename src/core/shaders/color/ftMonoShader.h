
#pragma once
#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {
	
	class ftMonoShader : public ftShader {
	public:
		ftMonoShader(){
			bInitialized = 1;
			
			if (ofIsGLProgrammableRenderer())
				glThree();
			else
				glTwo();
			
			if (bInitialized)
				ofLogNotice("ftMonoShader initialized");
			else
				ofLogWarning("ftMonoShader failed to initialize");
		}
		
	protected:
		void glTwo() {
			fragmentShader = GLSL120(
									 uniform sampler2DRect tex0;
									 uniform float treshold;
									 
									 void main(){
										 vec4 color = texture2DRect(tex0,gl_TexCoord[0].st);
										 float invT = 1.0 - treshold;
										 color.x = floor(color.x + invT);
										 color.y = floor(color.y + invT);
										 color.z = floor(color.z + invT);
										 color.w = floor(color.w + invT);
										 gl_FragColor = color;
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= linkProgram();
			
		}
		
		void glThree() {
			
			fragmentShader = GLSL410(
									 uniform sampler2DRect tex0;
									 uniform float treshold;
									 
									 in vec2 texCoordVarying;
									 out vec4 fragColor;
									 
									 void main(){
										 vec4 color = texture(tex0, texCoordVarying);
										 float invT = 1.0 - treshold;
										 color.x = floor(color.x + invT);
										 color.y = floor(color.y + invT);
										 color.z = floor(color.z + invT);
										 color.w = floor(color.w + invT);
										 fragColor = color;
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= bindDefaults();
			bInitialized *= linkProgram();
			
		}
		
	public:
		void update(ofFbo& _drawBuffer, ofTexture& _srcTexture, float _treshold){
			_drawBuffer.begin();
			begin();
			setUniformTexture( "tex0" , _srcTexture, 0 );
			setUniform1f("treshold", _treshold);
			renderFrame(_drawBuffer.getWidth(), _drawBuffer.getHeight());
			end();
			_drawBuffer.end();
		}
	};
}
