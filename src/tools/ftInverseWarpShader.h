
#pragma once

#include "ofMain.h"
#include "ftShader.h"


namespace flowTools {
	
	class ftInverseWarpShader : public ftShader {
	public:
		ftInverseWarpShader() {
			bInitialized = 1;
			
			if (ofIsGLProgrammableRenderer())
				glThree();
			else
				glTwo();
			
			if (bInitialized)
				ofLogNotice("ftInverseWarpShader initialized");
			else
				ofLogWarning("ftInverseWarpShader failed to initialize");
		}
		
	protected:
		void glTwo() {
			fragmentShader = GLSL120(
									 uniform sampler2DRect tex0;
									 
									 uniform vec2	Scale;
									 
									 uniform vec2 Upper_left;
									 uniform vec2 Upper_right;
									 uniform vec2 Lower_left;
									 uniform vec2 Lower_right;
									 
									 void main(){
										 vec2 st = gl_TexCoord[0].st;
										 
										 vec2 st2 = st * Scale;
										 
										 
										 float sourceXUp = st.x * (Upper_right.x - Upper_left.x) + Upper_left.x;
										 float sourceXDown = st.x * (Lower_right.x - Lower_left.x) + Lower_left.x;
										 float sourceX = mix(sourceXUp, sourceXDown, st.y);
										 
										 float sourceYLeft = st.y * (Lower_left.y - Upper_left.y) + Upper_left.y;
										 float sourceYRight = st.y * (Lower_right.y - Upper_right.y) + Upper_right.y;
										 float sourceY = mix(sourceYLeft, sourceYRight, st.x);
										 
										 gl_FragColor = texture2DRect(tex0,vec2(sourceX * Scale.x, sourceY * Scale.y));
									 }
								  );
			
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.linkProgram();
		}
		
		void glThree() {
			fragmentShader = GLSL150(
								  uniform sampler2DRect Backbuffer;
								  uniform sampler2DRect AddTexture;
								  uniform float force;
								  uniform vec2	Scale;
								  
								  in vec2 texCoordVarying;
								  out vec4 fragColor;
								  
								  void main(){
									  vec2 st = texCoordVarying;
									  vec2 st2 = st * Scale;
									  
									  vec4 color = texture(Backbuffer, st) + texture(AddTexture, st2) * force;
									  fragColor = color ;
								  }
								  );
			
			bInitialized *= shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.bindDefaults();
			bInitialized *= shader.linkProgram();
		}
		
	public:
		
		void update(ofFbo& _buffer, ofTexture& _texture, ofPoint _ul, ofPoint _ur, ofPoint _ll, ofPoint _lr){
			_buffer.begin();
			shader.begin();
			shader.setUniformTexture("tex0", _texture, 0);
			shader.setUniform2f("Scale", _texture.getWidth(), _texture.getHeight());
			shader.setUniform2f("Upper_left",  _ul.x, _ul.y);
			shader.setUniform2f("Upper_right", _ur.x, _ur.y);
			shader.setUniform2f("Lower_left",  _ll.x, _ll.y);
			shader.setUniform2f("Lower_right", _lr.x, _lr.y);
			renderNormalizedFrame(_buffer.getWidth(), _buffer.getHeight());
			shader.end();
			_buffer.end();
		}
	};
}