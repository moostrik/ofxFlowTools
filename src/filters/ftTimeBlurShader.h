
#pragma once

#include "ofMain.h"
#include "ftShader.h"
#include "ftSwapBuffer.h"

namespace flowTools {
	
	class ftTimeBlurShader : public ftShader {
	public:
		
		ofParameterGroup	parameters;
		ftTimeBlurShader(){
			internalFormat = GL_RGBA;
			
			bInitialized = 1;
			
			if (ofGetGLProgrammableRenderer())
				glThree();
			else
				glTwo();
			
			if (bInitialized)
				ofLogNotice("ftTimeBlurShader initialized");
			else
				ofLogWarning("ftTimeBlurShader failed to initialize");
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
			bInitialized *= blurShader[0].setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentHorizontalBlurShader);
			bInitialized *= blurShader[0].linkProgram();
			
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
			bInitialized *= blurShader[1].setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentVerticalBlurShader);
			bInitialized *= blurShader[1].linkProgram();
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
			bInitialized *= blurShader[0].setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= blurShader[0].setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentHorizontalBlurShader);
			bInitialized *= blurShader[0].linkProgram();
			
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
			bInitialized *= blurShader[1].setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= blurShader[1].setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentVerticalBlurShader);
			bInitialized *= blurShader[1].linkProgram();
			
		}

	public:
		
		
		void update(ofFbo& _buffer, float _decay, int _radius = 5, int _passes = 1){
			if (pingPong.getWidth() != _buffer.getWidth() ||
				pingPong.getHeight() != _buffer.getHeight() ||
				pingPong.getInternalFormat() != _buffer.getTextureReference().getTextureData().glTypeInternal) {
				pingPong.allocate(_buffer.getWidth(),  _buffer.getHeight(), _buffer.getTextureReference().getTextureData().glTypeInternal );
				
			}
			
			
			ofPushStyle();
			ofEnableBlendMode(OF_BLENDMODE_ALPHA);
			pingPong.src->begin();
			ofSetColor(0,0,0,255 * _decay);
			ofRect(0,0, pingPong.getWidth(), pingPong.getHeight());
			ofEnableBlendMode(OF_BLENDMODE_ADD);
			ofSetColor(255, 255, 255, 255);
			_buffer.draw(0,0, pingPong.getWidth(), pingPong.getHeight());
			pingPong.src->end();
			
			ofEnableBlendMode(OF_BLENDMODE_DISABLED);
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
			
			ofPopStyle();
		}
		
	protected:
		
		void allocate(int _width, int _height, int _internalFormat = GL_RGBA){
			pingPong.allocate( _width, _height, _internalFormat);
			printf("now");
		}
		
		
		ofShader    blurShader[2];
		ftSwapBuffer pingPong;
		
		int		internalFormat;
		int		width;
		int		height;
		
	};
}