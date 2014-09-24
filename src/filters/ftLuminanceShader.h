
#pragma once
#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {
	
	class ftLuminanceShader : public ftShader {
	public:
		ftLuminanceShader(){
			ofLogVerbose("init ftLuminanceShader");
			
			if (isProgrammableRenderer)
				glThree();
			else
				glTwo();
		}
		
	protected:
		void glTwo() {
			fragmentShader = GLSL(120,
								  uniform sampler2DRect tex0;
								  
								  void main(){
									  vec4 color = texture2DRect(tex0,gl_TexCoord[0].st);
									  float alpha = color.a;
									  float p = 0.3 *color.g + 0.59*color.r + 0.11*color.b;
									  //p *= 10.0;
									  color = vec4(p,p,p,1.0);
									  
									  gl_FragColor =  vec4(p, p, p, alpha);
								  }
								  );
			
			shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			shader.linkProgram();
		}
		
		void glThree() {
			fragmentShader = GLSL(150,
								  uniform sampler2DRect tex0;
								  
								  in vec2 texCoordVarying;
								  out vec4 fragColor;
								  
								  void main(){
									  vec4 color = texture(tex0,texCoordVarying);
									  float alpha = color.a;
									  float p = 0.3 *color.g + 0.59*color.r + 0.11*color.b;
									  //p *= 10.0;
									  color = vec4(p,p,p,1.0);
									  
									  fragColor =  vec4(p, p, p, alpha);
								  }
								  );
			
			shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			shader.bindDefaults();
			shader.linkProgram();
		}
		
	public:
		
		void update(ofFbo& _drawBuffer, ofTexture& _srcTexture){
			_drawBuffer.begin();
			shader.begin();
			shader.setUniformTexture( "tex0" , _srcTexture, 0 );
			renderFrame(_drawBuffer.getWidth(), _drawBuffer.getHeight());
			shader.end();
			_drawBuffer.end();
		}
	};
}