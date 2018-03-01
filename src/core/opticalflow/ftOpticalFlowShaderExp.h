
#pragma once

#include "ofMain.h"
#include "ftShader.h"


namespace flowTools {
	
	class ftOpticalFlowShaderExp : public ftShader {
	public:
		ftOpticalFlowShaderExp() {
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
									   uniform float			force;
									   uniform float			offset;
									   uniform float			lambda;
									   uniform float			threshold;
									   uniform float			inverseX;
									   uniform float			inverseY;
									   uniform float			FlowPower;
									   
									   void main(){
										   vec2 st = gl_TexCoord[0].st;
										   
										   vec2	off_x = vec2(offset, 0.0);
										   vec2	off_y = vec2(0.0, offset);
										   
										   //get the difference
										   vec4 scr_dif = texture2DRect(CurrTexture, st) - texture2DRect(LastTexture, st);
										   
										   //calculate the gradient
										   vec4 gradx; vec4 grady; vec4 gradmag; vec4 vx; vec4 vy;
										   gradx =	texture2DRect(LastTexture, st + off_x) - texture2DRect(LastTexture, st - off_x);
										   gradx += texture2DRect(CurrTexture, st + off_x) - texture2DRect(CurrTexture, st - off_x);
										   grady =	texture2DRect(LastTexture, st + off_y) - texture2DRect(LastTexture, st -off_y);
										   grady += texture2DRect(CurrTexture, st + off_y) - texture2DRect(CurrTexture, st - off_y);
										   
										   gradmag = sqrt((gradx*gradx)+(grady*grady)+vec4(lambda));
										   vx = scr_dif*(gradx/gradmag);
										   vy = scr_dif*(grady/gradmag);
										   
										   vec2	flow = vec2(0.0);
										   flow.x = -(vx.x + vx.y + vx.z) / 3.0 * inverseX;
										   flow.y = -(vy.x + vy.y + vy.z) / 3.0 * inverseY;
										   
										   // apply treshold
										   float strength = length(flow);
										   if (strength * threshold > 0.0) {
											   if (strength < threshold) {
												   flow = vec2(0.0);
											   }
											   else {
												   strength = (strength - threshold) / (1.0 - threshold);
												   flow = normalize(flow) * vec2(strength);
												   
											   }
										   }
										   
										   // apply force
										   flow *= vec2(force);
										   
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
									 uniform float			force;
									 uniform float			offset;
									 uniform float			lambda;
									 uniform float			threshold;
									 uniform float			inverseX;
									 uniform float			inverseY;
								  
									 in vec2 texCoordVarying;
									 out vec4 fragColor;
									 
									 float rgbSum(vec3 _value) {
										 return  _value.r + _value.g + _value.b;
									 }
									 float rgbAvg(vec3 _value) {
										 return  rgbSum(_value) / 3.0;
									 }
									 
									 
									 void main()
									 {
										 vec2 st = texCoordVarying;
										 
										 vec4 cSobelH = vec4(0);
										 cSobelH += textureOffset(CurrTexture, st, ivec2(-1,-1)) * -.25;
										 cSobelH += textureOffset(CurrTexture, st, ivec2(-1, 0)) * -.5;
										 cSobelH += textureOffset(CurrTexture, st, ivec2(-1,+1)) * -.25;
										 cSobelH += textureOffset(CurrTexture, st, ivec2(+1,-1)) * +.25;
										 cSobelH += textureOffset(CurrTexture, st, ivec2(+1, 0)) * +.5;
										 cSobelH += textureOffset(CurrTexture, st, ivec2(+1,+1)) * +.25;
									  
										 vec4 cSobelV = vec4(0);
										 cSobelV += textureOffset(CurrTexture, st, ivec2(-1,-1)) * -.25;
										 cSobelV += textureOffset(CurrTexture, st, ivec2( 0,-1)) * -.5;
										 cSobelV += textureOffset(CurrTexture, st, ivec2(+1,-1)) * -.25;
										 cSobelV += textureOffset(CurrTexture, st, ivec2(-1,+1)) * +.25;
										 cSobelV += textureOffset(CurrTexture, st, ivec2( 0,+1)) * +.5;
										 cSobelV += textureOffset(CurrTexture, st, ivec2(+1,+1)) * +.25;
										 
										 vec4 lSobelH = vec4(0);
										 lSobelH += textureOffset(LastTexture, st, ivec2(-1,-1)) * -.25;
										 lSobelH += textureOffset(LastTexture, st, ivec2(-1, 0)) * -.5;
										 lSobelH += textureOffset(LastTexture, st, ivec2(-1,+1)) * -.25;
										 lSobelH += textureOffset(LastTexture, st, ivec2(+1,-1)) * +.25;
										 lSobelH += textureOffset(LastTexture, st, ivec2(+1, 0)) * +.5;
										 lSobelH += textureOffset(LastTexture, st, ivec2(+1,+1)) * +.25;
									  
										 vec4 lSobelV = vec4(0);
										 lSobelV += textureOffset(LastTexture, st, ivec2(-1,-1)) * -.25;
										 lSobelV += textureOffset(LastTexture, st, ivec2( 0,-1)) * -.5;
										 lSobelV += textureOffset(LastTexture, st, ivec2(+1,-1)) * -.25;
										 lSobelV += textureOffset(LastTexture, st, ivec2(-1,+1)) * +.25;
										 lSobelV += textureOffset(LastTexture, st, ivec2( 0,+1)) * +.5;
										 lSobelV += textureOffset(LastTexture, st, ivec2(+1,+1)) * +.25;
									  
									  
										 vec3 dt_ = texture(LastTexture , st).xyz - texture(CurrTexture , st).xyz; // dt
										 vec3 dx_ = cSobelH.xyz + lSobelH.xyz;
										 vec3 dy_ = cSobelV.xyz + lSobelV.xyz;
										 
//										 dt_ = pow(length(dt_), 0.618) * normalize(dt_);
//										 dx_ = pow(length(dx_), 0.618) * normalize(dx_);
//										 dy_ = pow(length(dy_), 0.618) * normalize(dx_);
										 
										 float dt = rgbAvg(dt_);
										 float dx = rgbAvg(dx_);
										 float dy = rgbAvg(dy_);
//										 float dt = pow(rgbAvg(dt_), 0.95);
//										 float dx = pow(rgbAvg(dx_), 0.95);
//										 float dy = pow(rgbAvg(dy_), 0.95);

										 
										 // gradient length
										 float dd = sqrt(dx*dx + dy*dy + .00001);
										 // optical flow
										 vec2 flow = dt * vec2(dx, dy) / dd;
										 
										 
//										vec2	off_x = vec2(offset, 0.0);
//										vec2	off_y = vec2(0.0, offset);
//									  
//										//get the difference
//										vec4 scr_dif = texture(CurrTexture, st) - texture(LastTexture, st);
//									  
//										//calculate the gradient
//										vec4 gradx; vec4 grady; vec4 gradmag; vec4 vx; vec4 vy;
//										gradx =  texture(LastTexture, st + off_x) - texture(LastTexture, st - off_x);
//										gradx += texture(CurrTexture, st + off_x) - texture(CurrTexture, st - off_x);
//										grady =  texture(LastTexture, st + off_y) - texture(LastTexture, st - off_y);
//										grady += texture(CurrTexture, st + off_y) - texture(CurrTexture, st - off_y);
//
//										gradmag = sqrt((gradx*gradx)+(grady*grady)+vec4(lambda));
//										vx = scr_dif*(gradx/gradmag);
//										vy = scr_dif*(grady/gradmag);
//									  
//										vec2	flow = vec2(0.0);
//										flow.x = -(vx.x + vx.y + vx.z) / 3.0;
//										flow.y = -(vy.x + vy.y + vy.z) / 3.0;
									  
										 // apply treshold
										 float strength = length(flow);
										 if (strength * threshold > 0.0) {
											 if (strength < threshold) {
												 flow = vec2(0.0);
											 }
											 else {
												 strength = (strength - threshold) / (1.0 - threshold);
												 flow = normalize(flow) * vec2(strength);
												 
											 }
										 }
										 
										 // apply force
										 flow *= vec2(force);
									
										 // invert
										 flow *= vec2(inverseX, inverseY);
										 
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
		void update(ofFbo& _buffer, ofTexture& _currentTexture, ofTexture& _lastTexture, float _force = 1.0, float _offset = 3.0, float _lambda = 0.01, float _threshold = 0.04, int _inverseX = 0, int _inverseY = 0){
			
			_buffer.begin();
			
			shader.begin();
			shader.setUniformTexture("CurrTexture", _currentTexture, 0);
			shader.setUniformTexture("LastTexture", _lastTexture, 1);
			shader.setUniform1f("force", _force);
			shader.setUniform1f("offset", _offset);
			shader.setUniform1f("lambda", _lambda);
			shader.setUniform1f("threshold", _threshold);
			shader.setUniform1f("inverseX", _inverseX);
			shader.setUniform1f("inverseY", _inverseY);
			
			renderFrame(_buffer.getWidth(), _buffer.getHeight());
	
			shader.end();
			
			_buffer.end();
			
		}
	};
}