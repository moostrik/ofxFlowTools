
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {
	
	class ftBorderShader : public ftShader {
	public:
		ftBorderShader() {
			bInitialized = true;
			if (ofIsGLProgrammableRenderer()) { glThree(); } else { glTwo(); }
			string shaderName = "ftBorderShader";
			if (bInitialized) { ofLogVerbose(shaderName + " initialized"); }
			else { ofLogWarning(shaderName + " failed to initialize"); }
			load("tempShader/ftVertexShader.vert", "tempShader/" + shaderName + ".frag");
		}
		
	protected:
		void glTwo() {
			fragmentShader = GLSL120(
									 uniform sampler2DRect Backbuffer;
									 uniform sampler2DRect Obstacle;
									 uniform sampler2DRect Velocity;
									 
									 uniform float TimeStep;
									 uniform float Dissipation;
									 uniform float InverseCellSize;
									 uniform vec2	Scale;
									 
									 void main(){
										 vec2 st = gl_TexCoord[0].st;
										 vec2 st2 = st * Scale;
										 
										 float inverseSolid = 1.0 - ceil(texture2DRect(Obstacle, st2).x - 0.5);
										 
										 vec2 u = texture2DRect(Velocity, st2).rg / Scale;
										 vec2 coord =  st - TimeStep * InverseCellSize * u;
										 
										 gl_FragColor = Dissipation * texture2DRect(Backbuffer, coord) * inverseSolid;
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= linkProgram();
		}
		
		void glThree() {
			fragmentShader = GLSL150(
									 uniform sampler2DRect Backbuffer;
									 uniform float Scale;
									 
									 in vec2 texCoordVarying;
									 out vec4 fragColor;
									 
									 void main(){
										 vec2 st = texCoordVarying;
										 fragColor = texture(Backbuffer, st) * vec4(Scale);
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= bindDefaults();
			bInitialized *= linkProgram();
		}
		
	public:
		void update(ofFbo& _fbo, ofTexture& _backTex, float _scale){
			_fbo.begin();
			ofClear(0,0,0,0);
			_backTex.bind();
			renderFrame(_fbo.getWidth(), _fbo.getHeight());
			_backTex.unbind();
			
			begin();
			setUniformTexture("Backbuffer", _backTex, 0);
			setUniform1f("Scale", _scale);
			ofPushStyle();
			ofEnableBlendMode(OF_BLENDMODE_DISABLED);
			borderVbo.draw(GL_TRIANGLES, 0, borderVbo.getNumVertices());
			ofPopStyle();
			end();
			_fbo.end();
		}
		void setup(int _w, int _h){
			ofMesh borderMesh;
			borderMesh.setMode(OF_PRIMITIVE_TRIANGLES);
			borderMesh.getVertices().resize(6*4);
			borderMesh.getTexCoords().resize(6*4);
			
			int x0, x1, y0, y1;
			int u0, u1, v0, v1;
			
			//top;
			
			x0 = 1;
			x1 = _w-1;
			y0 = 0;
			y1 = 1;
			
			u0 = x0;
			u1 = x1;
			v0 = y0 + 1;
			v1 = y1 + 1;
			
			borderMesh.setVertex(0, glm::vec3(x0, y0, 0));
			borderMesh.setVertex(1, glm::vec3(x1, y0, 0));
			borderMesh.setVertex(2, glm::vec3(x0, y1, 0));
			
			borderMesh.setVertex(3, glm::vec3(x1, y0, 0));
			borderMesh.setVertex(4, glm::vec3(x1, y1, 0));
			borderMesh.setVertex(5, glm::vec3(x0, y1, 0));
			
			borderMesh.setTexCoord(0, glm::vec2(u0, v0));
			borderMesh.setTexCoord(1, glm::vec2(u1, v0));
			borderMesh.setTexCoord(2, glm::vec2(u0, v1));
			
			borderMesh.setTexCoord(3, glm::vec2(u1, v0));
			borderMesh.setTexCoord(4, glm::vec2(u1, v1));
			borderMesh.setTexCoord(5, glm::vec2(u0, v1));
			
			//bottom;
			
			x0 = 1;
			x1 = _w-1;
			y0 = _h-1;
			y1 = _h;
			
			u0 = x0;
			u1 = x1;
			v0 = y0 - 1;
			v1 = y1 - 1;
			
			borderMesh.setVertex(6, glm::vec3(x0, y0, 0));
			borderMesh.setVertex(7, glm::vec3(x1, y0, 0));
			borderMesh.setVertex(8, glm::vec3(x0, y1, 0));
			
			borderMesh.setVertex(9, glm::vec3(x1, y0, 0));
			borderMesh.setVertex(10, glm::vec3(x1, y1, 0));
			borderMesh.setVertex(11, glm::vec3(x0, y1, 0));
			
			borderMesh.setTexCoord(6, glm::vec2(u0, v0));
			borderMesh.setTexCoord(7, glm::vec2(u1, v0));
			borderMesh.setTexCoord(8, glm::vec2(u0, v1));
			
			borderMesh.setTexCoord(9, glm::vec2(u1, v0));
			borderMesh.setTexCoord(10, glm::vec2(u1, v1));
			borderMesh.setTexCoord(11, glm::vec2(u0, v1));
			
			//left;
			
			x0 = 0;
			x1 = 1;
			y0 = 1;
			y1 = _h-1;
			
			u0 = x0 + 1;
			u1 = x1 + 1;
			v0 = y0;
			v1 = y1;
			
			borderMesh.setVertex(12, glm::vec3(x0, y0, 0));
			borderMesh.setVertex(13, glm::vec3(x1, y0, 0));
			borderMesh.setVertex(14, glm::vec3(x0, y1, 0));
			
			borderMesh.setVertex(15, glm::vec3(x1, y0, 0));
			borderMesh.setVertex(16, glm::vec3(x1, y1, 0));
			borderMesh.setVertex(17, glm::vec3(x0, y1, 0));
			
			borderMesh.setTexCoord(12, glm::vec2(u0, v0));
			borderMesh.setTexCoord(13, glm::vec2(u1, v0));
			borderMesh.setTexCoord(14, glm::vec2(u0, v1));
			
			borderMesh.setTexCoord(15, glm::vec2(u1, v0));
			borderMesh.setTexCoord(16, glm::vec2(u1, v1));
			borderMesh.setTexCoord(17, glm::vec2(u0, v1));
			
			//right;
			
			x0 = _w - 1;
			x1 = _w;
			y0 = 1;
			y1 = _h-1;
			
			u0 = x0 - 1;
			u1 = x1 - 1;
			v0 = y0;
			v1 = y1;
			
			borderMesh.setVertex(18, glm::vec3(x0, y0, 0));
			borderMesh.setVertex(19, glm::vec3(x1, y0, 0));
			borderMesh.setVertex(20, glm::vec3(x0, y1, 0));
			
			borderMesh.setVertex(21, glm::vec3(x1, y0, 0));
			borderMesh.setVertex(22, glm::vec3(x1, y1, 0));
			borderMesh.setVertex(23, glm::vec3(x0, y1, 0));
			
			borderMesh.setTexCoord(18, glm::vec2(u0, v0));
			borderMesh.setTexCoord(19, glm::vec2(u1, v0));
			borderMesh.setTexCoord(20, glm::vec2(u0, v1));
			
			borderMesh.setTexCoord(21, glm::vec2(u1, v0));
			borderMesh.setTexCoord(22, glm::vec2(u1, v1));
			borderMesh.setTexCoord(23, glm::vec2(u0, v1));
			
			
			// LINES
			
//			borderMesh.setVertex(0, glm::vec3(1, .5, 0));
//			borderMesh.setTexCoord(0, glm::vec2(1, 1));
//			borderMesh.setVertex(1, glm::vec3(_w-1, .5, 0));
//			borderMesh.setTexCoord(1, glm::vec2(_w-1, 1));
//
//			borderMesh.setVertex(2, glm::vec3(1, _h, 0));
//			borderMesh.setTexCoord(2, glm::vec2(1, _h-1));
//			borderMesh.setVertex(3, glm::vec3(_w-1, _h, 0));
//			borderMesh.setTexCoord(3, glm::vec2(_w-1, _h-1));
//
//			borderMesh.setVertex(4, glm::vec3(.5, 1.5, 0));
//			borderMesh.setTexCoord(4, glm::vec2(1, 1));
//			borderMesh.setVertex(5, glm::vec3(.5, _h-.5, 0));
//			borderMesh.setTexCoord(5, glm::vec2(1, _h-1));
//
//			borderMesh.setVertex(6, glm::vec3(_w-.5, 1.5, 0));
//			borderMesh.setTexCoord(6, glm::vec2(_w-1, 1));
//			borderMesh.setVertex(7, glm::vec3(_w-.5, _h-.5, 0));
//			borderMesh.setTexCoord(7, glm::vec2(_w-1, _h-1));
			
			borderVbo.setMesh(borderMesh, GL_DYNAMIC_DRAW, false, true, false);
		}
		ofVbo borderVbo;
		
		
		
		void renderFrame(float _width, float _height){
			ofPushStyle();
			ofEnableBlendMode(OF_BLENDMODE_DISABLED);
			
			quad.setVertex(0, glm::vec3(1,1,0));
			quad.setVertex(1, glm::vec3(_width-1,1,0));
			quad.setVertex(2, glm::vec3(_width-1,_height-1,0));
			quad.setVertex(3, glm::vec3(1,_height-1,0));
			
			quad.setTexCoord(0, glm::vec2(1,1));
			quad.setTexCoord(1, glm::vec2(_width-1,1));
			quad.setTexCoord(2, glm::vec2(_width-1,_height-1));
			quad.setTexCoord(3, glm::vec2(1,_height-1));
			
			quad.draw();
			
			ofPopStyle();
		}
	};
}


