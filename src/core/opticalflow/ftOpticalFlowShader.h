
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
									 uniform sampler2DRect	tex0;
									 uniform sampler2DRect	tex1;
									 uniform float			offset;
									 uniform float			threshold;
									 uniform vec2			force;
									 uniform float			power;
									 
									 void main(){
										 vec2 st = gl_TexCoord[0].st;
										 vec2 off_x = vec2(offset, 0.0);
										 vec2 off_y = vec2(0.0, offset);
										 
										 //get the difference
										 float scr_dif = texture2DRect(tex0, st).x - texture2DRect(tex1, st).x;
										 
										 //calculate the gradient
										 float gradx; float grady; float gradmag; float lambda = 0.01;
										 gradx =  texture2DRect(tex1, st + off_x).x - texture2DRect(tex1, st - off_x).x;
										 gradx += texture2DRect(tex0, st + off_x).x - texture2DRect(tex0, st - off_x).x;
										 grady =  texture2DRect(tex1, st + off_y).x - texture2DRect(tex1, st - off_y).x;
										 grady += texture2DRect(tex0, st + off_y).x - texture2DRect(tex0, st - off_y).x;
										 gradmag = sqrt((gradx*gradx)+(grady*grady)+lambda);
										 
										 vec2 flow;
										 flow.x = scr_dif*(gradx/gradmag);
										 flow.y = scr_dif*(grady/gradmag);
										 
										 // apply force (to normalize)
										 flow *= force;
										 
										 // apply treshold and clamp
										 float magnitude = length(flow);
										 magnitude = max(magnitude, threshold);
										 magnitude -= threshold;
										 magnitude /= (1-threshold);
										 magnitude = pow(magnitude, power);
										 flow = normalize(flow) * vec2(min(magnitude, 1));
										 
										 // set color
										 gl_FragColor = vec4(flow, 0.0, 1.0);
									 }
									 );
			
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.linkProgram();
		}
		
		void glThree() {
			
			fragmentShader = GLSL150(
									 uniform sampler2DRect	tex0;
									 uniform sampler2DRect	tex1;
									 uniform float			offset;
									 uniform float			threshold;
									 uniform vec2			force;
									 uniform float			power;
									 
									 in vec2 texCoordVarying;
									 out vec4 fragColor;
									 
									 void main()
									 {
										 vec2 st = texCoordVarying;
										 vec2 off_x = vec2(offset, 0.0);
										 vec2 off_y = vec2(0.0, offset);
										 
										 //get the difference
										 float scr_dif = texture(tex0, st).x - texture(tex1, st).x;
										 
										 //calculate the gradient
										 float gradx; float grady; float gradmag; float lambda = 0.01;
										 gradx =  texture(tex1, st + off_x).x - texture(tex1, st - off_x).x;
										 gradx += texture(tex0, st + off_x).x - texture(tex0, st - off_x).x;
										 grady =  texture(tex1, st + off_y).x - texture(tex1, st - off_y).x;
										 grady += texture(tex0, st + off_y).x - texture(tex0, st - off_y).x;
										 gradmag = sqrt((gradx*gradx)+(grady*grady)+lambda);
										 
										 vec2 flow;
										 flow.x = scr_dif*(gradx/gradmag);
										 flow.y = scr_dif*(grady/gradmag);
										 
										 // apply force (to normalize)
										 flow *= force;
										 
										 // apply treshold and clamp
										 float magnitude = length(flow);
										 magnitude = max(magnitude, threshold);
										 magnitude -= threshold;
										 magnitude /= (1-threshold);
										 magnitude = pow(magnitude, power);
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
		void update(ofFbo& _fbo, ofTexture& _currTex, ofTexture& _prevTex, float _offset = 3.0, float _threshold = 0.04, ofDefaultVec2 _force = ofDefaultVec2(1.0, 1.0), float _power = 1.0, bool _inverseX = 0, bool _inverseY = 0){
			
			_fbo.begin();
			shader.begin();
			shader.setUniformTexture("tex0", _currTex, 0);
			shader.setUniformTexture("tex1", _prevTex, 1);
			shader.setUniform1f("offset", _offset);
			shader.setUniform1f("threshold", _threshold);
			shader.setUniform2f("force", _force * ofDefaultVec2(_inverseX? -1 : 1, _inverseY? -1 : 1));
			shader.setUniform1f("power", _power);
			renderFrame(_fbo.getWidth(), _fbo.getHeight());
			shader.end();
			_fbo.end();
		}
	};
}
