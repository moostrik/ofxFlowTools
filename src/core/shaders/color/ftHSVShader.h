
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {
	
	class ftHSVShader : public ftShader {
	public:
		ftHSVShader(){
            bInitialized = 1;
            if (ofIsGLProgrammableRenderer()) { glThree(); } else { glTwo(); }
			
			if (bInitialized)
				ofLogVerbose("ftHSLShader initialized");
			else
				ofLogWarning("ftHSLShader failed to initialize");
		}
		
	protected:
		void glTwo() {
			fragmentShader = GLSL120(
									 uniform sampler2DRect tex0;
									 uniform float hue;
									 uniform float saturation;
									 uniform float value;
									 
									 vec3 rgb2hsv(vec3 c) {
										 vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
										 vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
										 vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));
										 
										 float d = q.x - min(q.w, q.y);
										 float e = 1.0e-10;
										 return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
									 }
									 
									 vec3 hsv2rgb(vec3 c) {
										 vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
										 vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
										 return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
									 }
									 
									 void main(){
										 vec4 color = texture2DRect(tex0, gl_TexCoord[0].st);
										 vec3 hsv = rgb2hsv(color.rgb);
										 hsv.x += hue;
										 hsv.y *= saturation;
										 vec3 result = hsv2rgb(hsv);
										 gl_FragColor = vec4(result, color.a);
									 }
									 );
			
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.linkProgram();

		}
		
		void glThree() {
			fragmentShader = GLSL150(
									 uniform sampler2DRect tex0;
									 uniform float hue;
									 uniform float saturation;
									 uniform float value;
									 
									 in vec2 texCoordVarying;
									 out vec4 fragColor;
									 
									 vec3 rgb2hsv(vec3 c) {
										 vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
										 vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
										 vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));
										 
										 float d = q.x - min(q.w, q.y);
										 float e = 1.0e-10;
										 return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
									 }
									 
									 vec3 hsv2rgb(vec3 c) {
										 vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
										 vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
										 return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
									 }
									 
									 void main() {
										 vec4 color = texture(tex0, texCoordVarying);
										 vec3 hsv = rgb2hsv(color.rgb);
										 hsv.x += hue;
										 hsv.y *= saturation;
										 hsv.z *= value;
										 vec3 result = hsv2rgb(hsv);
										 fragColor = vec4(result, color.a);
									 }
									 );
			
			bInitialized *= shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.bindDefaults();
			bInitialized *= shader.linkProgram();
		}
		
	public:
		
		
		void update(ofFbo& _drawBuffer, ofTexture& _srcTex, float _hue, float _saturation, float _value){
			_drawBuffer.begin();
			shader.begin();
			shader.setUniformTexture( "tex0" , _srcTex, 0 );
			shader.setUniform1f("hue", _hue );
			shader.setUniform1f("saturation", _saturation);
			shader.setUniform1f("value", _value);
			renderFrame(_drawBuffer.getWidth(), _drawBuffer.getHeight());
			shader.end();
			_drawBuffer.end();
		}
		
	};
}
