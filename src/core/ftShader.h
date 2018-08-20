#pragma once

#include "ofMain.h"

#define GLSL120(shader)  "#version 120 \n #extension GL_ARB_texture_rectangle : enable \n" #shader
#define GLSL120PI(shader)  "#version 120 \n #extension GL_ARB_texture_rectangle : enable \n #define PI 3.1415926535897932 \n #define HALF_PI 1.5707963267948966 \n" #shader
#define GLSL120GEO(shader)  "#version 120 \n #extension GL_ARB_texture_rectangle : enable \n #extension GL_EXT_geometry_shader4: enable \n" #shader
#define GLSL150(shader)  "#version 150 \n" #shader
#define GLSL150PI(shader)  "#version 150 \n #define PI 3.1415926535897932 \n #define HALF_PI 1.5707963267948966 \n" #shader

namespace flowTools {
	
	class ftShader : public ofShader{
	public:
		ftShader() {
			bInitialized = false;
			if (ofIsGLProgrammableRenderer()) { glThree(); } else { glTwo(); }
			quad.getVertices().resize(4);
			quad.getTexCoords().resize(4);
			quad.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
		}
		
	protected:
		void glTwo()  {
			vertexShader = GLSL120(
								   void main() {
									   gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
									   gl_TexCoord[0] = gl_MultiTexCoord0;
									   gl_FrontColor = gl_Color;
								   }
								   );
			
			fragmentShader = GLSL120(
									 void main()
									 {
										 gl_FragColor = vec4(0.0,1.0,0.0,1.0);
									 }
									 );
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
									   gl_Position = modelViewProjectionMatrix * position;
									   texCoordVarying = (textureMatrix*vec4(texcoord.x,texcoord.y,0,1)).xy;
									   colorVarying = color;
								   }
								   );
			
			fragmentShader = GLSL150(
									 out vec4 fragColor;
									 
									 void main()
									 {
										 fragColor = vec4(0.0,1.0,0.0,1.0);
									 }
									 );
		}
		
	protected:
		
		void renderFrame(float _width, float _height){
			ofPushStyle();
			ofEnableBlendMode(OF_BLENDMODE_DISABLED);
			
			quad.setVertex(0, glm::vec3(0,0,0));
			quad.setVertex(1, glm::vec3(_width,0,0));
			quad.setVertex(2, glm::vec3(_width,_height,0));
			quad.setVertex(3, glm::vec3(0,_height,0));
			
			quad.setTexCoord(0, glm::vec2(0,0));
			quad.setTexCoord(1, glm::vec2(_width,0));
			quad.setTexCoord(2, glm::vec2(_width,_height));
			quad.setTexCoord(3, glm::vec2(0,_height));
			
			quad.draw();
			
			ofPopStyle();
		}
		
		ofMesh		quad;
		string		fragmentShader;
		string		vertexShader;
		bool		bInitialized;
	};
	
}
