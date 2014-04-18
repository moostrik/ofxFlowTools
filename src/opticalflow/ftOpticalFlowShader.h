
#pragma once

#include "ofMain.h"
#include "ftShader.h"


namespace flowTools {
	
	class ftOpticalFlowShader : public ftShader {
	public:
		ftOpticalFlowShader() {
			
			fragmentShader = STRINGIFY(uniform sampler2DRect	CurrTexture;
									   uniform sampler2DRect	LastTexture;
									   uniform float			force;
									   uniform float			offset;
									   uniform float			lambda;
									   uniform float			threshold;
									   uniform float			inverseX;
									   uniform float			inverseY;
									   
									   void main(){
										   vec2 st = gl_TexCoord[0].st;
										   
										   vec2	off_x = vec2(offset, 0.0);
										   vec2	off_y = vec2(0.0, offset);
										   
										   vec4	scr_dif;
										   vec4	gradx;
										   vec4	grady;
										   vec4	gradmag;
										   vec4	vx;
										   vec4	vy;
										   vec4	f4l = vec4(lambda);
										   vec4	flow = vec4(0.0);
										   
										   //get the difference
										   scr_dif = texture2DRect(CurrTexture, st) - texture2DRect(LastTexture, st);
										   
										   //calculate the gradient
										   gradx =	texture2DRect(LastTexture, st + off_x) - texture2DRect(LastTexture, st - off_x);
										   gradx += texture2DRect(CurrTexture, st + off_x) - texture2DRect(CurrTexture, st - off_x);
										   grady =	texture2DRect(LastTexture, st + off_y) - texture2DRect(LastTexture, st -off_y);
										   grady += texture2DRect(CurrTexture, st + off_y) - texture2DRect(CurrTexture, st - off_y);
										   
										   gradmag = sqrt((gradx*gradx)+(grady*grady)+f4l);
										   vx = scr_dif*(gradx/gradmag);
										   vy = scr_dif*(grady/gradmag);
										   
										   flow.x = -(vx.x + vx.y + vx.z) / 3.0 * inverseX;
										   flow.y = -(vy.x + vy.y + vy.z) / 3.0 * inverseY;
										   
										   if (length(flow.xy) < threshold) flow = vec4(0.0);
										   else flow *= vec4(force);
										   flow.w = length(flow.xy);
										   
										   gl_FragColor = flow;
									   }
									   );
			
			shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			shader.linkProgram();
		}
		
		void update(ofFbo& _buffer, ofTexture& _currentTexture, ofTexture& _lastTexture, float _speed = 1.0, float _offset = 3.0, float _lambda = 0.01, float _threshold = 0.04, int _inverseX = 0, int _inverseY = 0){
			
			
			_buffer.begin();
			shader.begin();
			shader.setUniformTexture("CurrTexture", _currentTexture, 0);
			shader.setUniformTexture("LastTexture", _lastTexture, 1);
			shader.setUniform1f("force", _speed);
			shader.setUniform1f("offset", _offset);
			shader.setUniform1f("lambda", _lambda);
			shader.setUniform1f("threshold", _threshold);
			shader.setUniform1f("inverseX", _inverseX);
			shader.setUniform1f("inverseY", _inverseY);
			
			ofPushStyle();
			ofEnableBlendMode(OF_BLENDMODE_DISABLED);
			ofSetColor(255);
			renderFrame(_buffer.getWidth(), _buffer.getHeight());
			ofPopStyle();
			
			shader.end();
			_buffer.end();
			
		}
	};
}