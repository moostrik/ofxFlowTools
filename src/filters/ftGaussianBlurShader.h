
#pragma once

#include "ofMain.h"
#include "ftShader.h"
#include "ftSwapBuffer.h"

namespace flowTools {

	class ftGaussianBlurShader : public ftShader {
	public:
		
		ofParameterGroup	parameters;
		ftGaussianBlurShader(){
			
			ofLogVerbose("init ftGaussianBlurShader");
			internalFormat = GL_RGBA;
			if (isProgrammableRenderer)
				glThree();
			else
				glTwo();
		}
		
	protected:
		void glTwo() {
			string fragmentHorizontalBlurShader = GLSL120(
													   uniform sampler2DRect backbuffer;
													   uniform float radius;
													   
													   const float total = (1. + 8. + 28. + 56.) * 2. + 70.;
													   
													   void main(void) {
														   vec2 st = gl_TexCoord[0].st;
														   
														   vec4 color = vec4(0.0,0.0,0.0,0.0);
														   color += (1. / total) * texture2DRect(backbuffer, st - radius * vec2(4. / 4., 0.));
														   color += (8. / total)  * texture2DRect(backbuffer, st - radius * vec2(3. / 4., 0.));
														   color += (28. / total)  * texture2DRect(backbuffer, st - radius * vec2(2. / 4., 0.));
														   color += (56. / total)  * texture2DRect(backbuffer, st - radius * vec2(1. / 4., 0.));
														   
														   color +=  (70. / total) * texture2DRect(backbuffer, st);
														   
														   color += (1. / total) * texture2DRect(backbuffer, st + radius * vec2(4. / 4., 0.));
														   color += (8. / total)  * texture2DRect(backbuffer, st + radius * vec2(3. / 4., 0.));
														   color += (28. / total)  * texture2DRect(backbuffer, st + radius * vec2(2. / 4., 0.));
														   color += (56. / total)  * texture2DRect(backbuffer, st + radius * vec2(1. / 4., 0.));
														   
														   gl_FragColor = color;
													   }
													   );
			blurShader[0].unload();
			blurShader[0].setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentHorizontalBlurShader);
			blurShader[0].linkProgram();
			
			string fragmentVerticalBlurShader = GLSL120(
													 uniform sampler2DRect backbuffer;
													 uniform float radius;
													 
													 const float total = (1. + 8. + 28. + 56.) * 2. + 70.;
													 
													 void main(void) {
														 vec2 st = gl_TexCoord[0].st;
														 
														 vec4 color = vec4(0.0,0.0,0.0,0.0);
														 color += (1. / total) * texture2DRect(backbuffer, st - radius * vec2(0., 4. / 4.));
														 color += (8. / total)  * texture2DRect(backbuffer, st - radius * vec2(0., 3. / 4.));
														 color += (28. / total)  * texture2DRect(backbuffer, st - radius * vec2(0., 2. / 4.));
														 color += (56. / total)  * texture2DRect(backbuffer, st - radius * vec2(0., 1. / 4.));
														 
														 color +=  (70. / total) * texture2DRect(backbuffer, st);
														 
														 color += (1. / total) * texture2DRect(backbuffer, st + radius * vec2(0., 4. / 4.));
														 color += (8. / total)  * texture2DRect(backbuffer, st + radius * vec2(0., 3. / 4.));
														 color += (28. / total)  * texture2DRect(backbuffer, st + radius * vec2(0., 2. / 4.));
														 color += (56. / total)  * texture2DRect(backbuffer, st + radius * vec2(0., 1. / 4.));
														 
														 gl_FragColor = color;
													 }
													 );
			blurShader[1].unload();
			blurShader[1].setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentVerticalBlurShader);
			blurShader[1].linkProgram();

			
		}
		
		void glThree() {
			string fragmentHorizontalBlurShader = GLSL150(
													   uniform sampler2DRect backbuffer;
													   uniform float radius;
													   
													   const float total = (1. + 8. + 28. + 56.) * 2. + 70.;
													   
													   in vec2 texCoordVarying;
													   out vec4 fragColor;
													   
													   void main(void) {
														   vec2 st = texCoordVarying;
														   
														   vec4 color = vec4(0.0,0.0,0.0,0.0);
														   color += (1. / total) * texture(backbuffer, st - radius * vec2(4. / 4., 0.));
														   color += (8. / total)  * texture(backbuffer, st - radius * vec2(3. / 4., 0.));
														   color += (28. / total)  * texture(backbuffer, st - radius * vec2(2. / 4., 0.));
														   color += (56. / total)  * texture(backbuffer, st - radius * vec2(1. / 4., 0.));
														   
														   color +=  (70. / total) * texture(backbuffer, st);
														   
														   color += (1. / total) * texture(backbuffer, st + radius * vec2(4. / 4., 0.));
														   color += (8. / total)  * texture(backbuffer, st + radius * vec2(3. / 4., 0.));
														   color += (28. / total)  * texture(backbuffer, st + radius * vec2(2. / 4., 0.));
														   color += (56. / total)  * texture(backbuffer, st + radius * vec2(1. / 4., 0.));
														   
														   fragColor = color;
													   }
													   );
			blurShader[0].unload();
			blurShader[0].setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			blurShader[0].setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentHorizontalBlurShader);
			blurShader[0].bindDefaults();
			blurShader[0].linkProgram();
			
			string fragmentVerticalBlurShader = GLSL150(
													 uniform sampler2DRect backbuffer;
													 uniform float radius;
													 
													 const float total = (1. + 8. + 28. + 56.) * 2. + 70.;
													 
													 in vec2 texCoordVarying;
													 out vec4 fragColor;
													 
													 void main(void) {
														 vec2 st = texCoordVarying;
														 
														 vec4 color = vec4(0.0,0.0,0.0,0.0);
														 color += (1. / total) * texture(backbuffer, st - radius * vec2(0., 4. / 4.));
														 color += (8. / total)  * texture(backbuffer, st - radius * vec2(0., 3. / 4.));
														 color += (28. / total)  * texture(backbuffer, st - radius * vec2(0., 2. / 4.));
														 color += (56. / total)  * texture(backbuffer, st - radius * vec2(0., 1. / 4.));
														 
														 color +=  (70. / total) * texture(backbuffer, st);
														 
														 color += (1. / total) * texture(backbuffer, st + radius * vec2(0., 4. / 4.));
														 color += (8. / total)  * texture(backbuffer, st + radius * vec2(0., 3. / 4.));
														 color += (28. / total)  * texture(backbuffer, st + radius * vec2(0., 2. / 4.));
														 color += (56. / total)  * texture(backbuffer, st + radius * vec2(0., 1. / 4.));
														 
														 fragColor = color;
													 }
													 );
			blurShader[1].unload();
			blurShader[1].setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			blurShader[1].setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentVerticalBlurShader);
			blurShader[1].bindDefaults();
			blurShader[1].linkProgram();
		}
		
	public:
	
		void update(ofFbo& _buffer, int _passes, int _radius){
			if (pingPong.getWidth() != _buffer.getWidth() ||
				pingPong.getHeight() != _buffer.getHeight() ||
				pingPong.getInternalFormat() != _buffer.getTextureReference().getTextureData().glTypeInternal) {
				
				allocate(_buffer.getWidth(),  _buffer.getHeight(), _buffer.getTextureReference().getTextureData().glTypeInternal );
				
			}
			
			
			pingPong.clear();
			pingPong.src->scaleIntoMe(_buffer);
	
			for(int i = 0; i < _passes; i++) {
				for(int j = 0; j < 2; j++) {
					pingPong.dst->begin();
					blurShader[j].begin();
					blurShader[j].setUniformTexture("backbuffer", pingPong.src->getTextureReference(), 0 );
					blurShader[j].setUniform1f("radius", _radius);
					renderFrame(pingPong.getWidth(), pingPong.getHeight());
					blurShader[j].end();
					pingPong.dst->end();
					
					pingPong.swap();
				}
			}
	
			_buffer.begin();
			pingPong.src->draw(0,0, pingPong.getWidth(), pingPong.getHeight());
			_buffer.end();
		}
		
	protected:
		
		void allocate(int _width, int _height, int _internalFormat = GL_RGBA){
			pingPong.allocate( _width, _height, _internalFormat);
			pingPong.clear();
		}
		
		ofShader    blurShader[2];
		ftSwapBuffer pingPong;
		
		int		internalFormat;
		int		width;
		int		height;
		
	};
}