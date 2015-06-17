#pragma once

#include "ofMain.h"

#define GLSL120(shader)  "#version 120 \n #extension GL_ARB_texture_rectangle : enable \n" #shader
#define GLSL120GEO(shader)  "#version 120 \n #extension GL_ARB_texture_rectangle : enable \n #extension GL_EXT_geometry_shader4: enable \n" #shader
#define GLSL150(shader)  "#version 150 \n" #shader

namespace flowTools {
	
	class ftShader{
	public:
		ftShader() {
			
			ofLogVerbose("init standard shader");
			if (ofGetGLProgrammableRenderer())
				glThree();
			else
				glTwo();
			
			quad.getVertices().resize(4);
			quad.getTexCoords().resize(4);
			quad.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
		}
		
	protected:
		void glTwo()  {
			
			vertexShader = GLSL120(
								   void main() {
									   gl_Position = gl_Vertex;
									   gl_FrontColor = gl_Color;
								   }
								   );
			
			
			fragmentShader = GLSL120(
									 void main()
									 {
										 gl_FragColor = vec4(0.0,1.0,0.0,1.0);
									 }
									 );
			
			shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			shader.linkProgram();
		}
		
		void glThree() {
			vertexShader = GLSL150(
								   uniform mat4 modelViewProjectionMatrix;
								   uniform mat4 textureMatrix;
								   
								   in vec4	position;
								   in vec2	texcoord;
								   in vec4	color;
								   
								   out vec2 texCoordVarying;
								   out vec4 colorVarying;
								   
								   void main()
								   {
									   colorVarying = color;
									   texCoordVarying = (textureMatrix*vec4(texcoord.x,texcoord.y,0,1)).xy;
									   gl_Position = modelViewProjectionMatrix * position;
								   }
								   );
			
			fragmentShader = GLSL150(
									 out vec4 fragColor;
									 
									 void main()
									 {
										 fragColor = vec4(0.0,1.0,0.0,1.0);
									 }
									 );
			
			shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			shader.bindDefaults();
			shader.linkProgram();
		}
		
		~ftShader() {shader.unload();};
		
	public:
		void update(ofFbo& _drawBuffer, ofTexture& _backBuffer, ofTexture& _someTexture, float _someFloat = 1.0){
			
			_drawBuffer.begin();
			
			shader.begin();
			shader.setUniformTexture("CurrTexture", _backBuffer, 0);
			shader.setUniformTexture("LastTexture", _someTexture, 1);
			shader.setUniform1f("force", _someFloat);
			
			renderFrame(_drawBuffer.getWidth(), _drawBuffer.getHeight());
			
			shader.end();
			
			_drawBuffer.end();
			
		}
		
	protected:
		
		void renderFrame(float _width, float _height){
			quad.setVertex(0, ofVec3f(0,0,0));
			quad.setVertex(1, ofVec3f(_width,0,0));
			quad.setVertex(2, ofVec3f(_width,_height,0));
			quad.setVertex(3, ofVec3f(0,_height,0));
			
			quad.setTexCoord(0, ofVec2f(0,0));
			quad.setTexCoord(1, ofVec2f(_width,0));
			quad.setTexCoord(2, ofVec2f(_width,_height));
			quad.setTexCoord(3, ofVec2f(0,_height));
			
			quad.draw();
		}
		
		void renderNormalizedFrame(float _width, float _height){
			quad.setVertex(0, ofVec3f(0,0,0));
			quad.setVertex(1, ofVec3f(_width,0,0));
			quad.setVertex(2, ofVec3f(_width,_height,0));
			quad.setVertex(3, ofVec3f(0,_height,0));
			
			quad.setTexCoord(0, ofVec2f(0,0));
			quad.setTexCoord(1, ofVec2f(1,0));
			quad.setTexCoord(2, ofVec2f(1,1));
			quad.setTexCoord(3, ofVec2f(0,1));
			
			quad.draw();
		}
		
		
		ofMesh quad;
		
		ofShader shader;
		string fragmentShader;
		string vertexShader;
	};
	
}
