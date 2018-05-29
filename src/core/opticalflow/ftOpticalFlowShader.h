
#pragma once

#include "ofMain.h"
#include "ftShader.h"


namespace flowTools {
	
	class ftOpticalFlowShader : public ftShader {
	public:
		ftOpticalFlowShader() {
			bInitialized = 1;
			
			if (ofIsGLProgrammableRenderer())
				glThree();
			else
				glTwo();
			
			if (bInitialized)
				ofLogNotice("ftOpticalFlowShader initialized");
			else
				ofLogWarning("ftOpticalFlowShader failed to initialize");
		}
		
	protected:
		void glTwo() {
			fragmentShader = GLSL120(
									 uniform sampler2DRect	CurrTexture;
									 uniform sampler2DRect	LastTexture;
									 uniform float			offset;
									 uniform float			threshold;
									 uniform float			force;
									 
									 void main(){
										 vec2 st = gl_TexCoord[0].st;
										 vec2	off_x = vec2(offset, 0.0);
										 vec2	off_y = vec2(0.0, offset);
										 
										 //get the difference
										 float scr_dif = texture2DRect(CurrTexture, st).x - texture2DRect(LastTexture, st).x;
										 
										 //calculate the gradient
										 float gradx; float grady; float gradmag; float lambda = 0.01;
										 gradx =  texture2DRect(LastTexture, st + off_x).x - texture2DRect(LastTexture, st - off_x).x;
										 gradx += texture2DRect(CurrTexture, st + off_x).x - texture2DRect(CurrTexture, st - off_x).x;
										 grady =  texture2DRect(LastTexture, st + off_y).x - texture2DRect(LastTexture, st - off_y).x;
										 grady += texture2DRect(CurrTexture, st + off_y).x - texture2DRect(CurrTexture, st - off_y).x;
										 gradmag = sqrt((gradx*gradx)+(grady*grady)+vec4(lambda));
										 
										 vec2 flow;
										 flow.x = scr_dif*(gradx/gradmag);
										 flow.y = scr_dif*(grady/gradmag);
										 
										 // apply treshold & force
										 float magnitude = length(flow);
										 magnitude = max(magnitude, threshold);
										 magnitude -= threshold;
										 flow = flow * vec2(magnitude) * vec2(force);
										 
										 // clamp to 0 - 1;
										 flow = normalize(flow) * vec2(min(length(flow), 1));
										 
										 // set color
										 gl_FragColor = vec4(flow, 0.0, 1.0);
									 }
									 );
			
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.linkProgram();
		}
		
		void glThree() {
			
			fragmentShader = GLSL150(
									 uniform sampler2DRect	CurrTexture;
									 uniform sampler2DRect	LastTexture;
									 uniform float			offset;
									 uniform float			threshold;
									 uniform vec2			invert;
									 uniform float			power;
									 
									 in vec2 texCoordVarying;
									 out vec4 fragColor;
									 
									 void main()
									 {
										 vec2 st = texCoordVarying;
										 vec2 off_x = vec2(offset, 0.0);
										 vec2 off_y = vec2(0.0, offset);
										 
										 //get the difference
										 float scr_dif = texture(CurrTexture, st).x - texture(LastTexture, st).x;
										 
										 //calculate the gradient
										 float gradx; float grady; float gradmag; float lambda = 0.01;
										 gradx =  texture(LastTexture, st + off_x).x - texture(LastTexture, st - off_x).x;
										 gradx += texture(CurrTexture, st + off_x).x - texture(CurrTexture, st - off_x).x;
										 grady =  texture(LastTexture, st + off_y).x - texture(LastTexture, st - off_y).x;
										 grady += texture(CurrTexture, st + off_y).x - texture(CurrTexture, st - off_y).x;
										 gradmag = sqrt((gradx*gradx)+(grady*grady)+lambda);
										 
										 vec2 flow;
										 flow.x = scr_dif*(gradx/gradmag);
										 flow.y = scr_dif*(grady/gradmag);
										 
										 flow *= invert;
										 
										 // apply treshold & force
										 float magnitude = length(flow);
										 magnitude = max(magnitude, threshold);
										 magnitude -= threshold;
										 magnitude /= (1-threshold);
										 magnitude = pow(magnitude, power);
//										 flow = flow * vec2(magnitude) * vec2(force);
										 
										 // clamp to 0 - 1;
										 flow = normalize(flow) * vec2(min(magnitude, 1));
										 
										 // set color
										 fragColor = vec4(flow, 0.0, 1.0);
									 }
									 
									 );
			
			bInitialized *= shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.bindDefaults();
			bInitialized *= shader.linkProgram();
		}
		
	public:	
		void update(ofFbo& _buffer, ofTexture& _currentTexture, ofTexture& _lastTexture, float _offset = 3.0, float _threshold = 0.04, float _force = 1.0, bool _inverseX = 0, bool _inverseY = 0){
			
			_buffer.begin();
			shader.begin();
			shader.setUniformTexture("CurrTexture", _currentTexture, 0);
			shader.setUniformTexture("LastTexture", _lastTexture, 1);
			shader.setUniform1f("offset", _offset);
			shader.setUniform1f("threshold", _threshold);
			shader.setUniform2f("invert", ofVec2f(_inverseX? -1 : 1, _inverseY? -1 : 1));
			shader.setUniform1f("power", _force);
			renderFrame(_buffer.getWidth(), _buffer.getHeight());
			shader.end();
			_buffer.end();
		}
	};
}
