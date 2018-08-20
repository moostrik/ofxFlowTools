
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
//			load("tempShader/ftVertexShader.vert", "tempShader/" + shaderName + ".frag");
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
									 
									 in vec2 texCoordVarying;
									 out vec4 fragColor;
									 
									 void main(){
										 vec2 st = texCoordVarying;
										 fragColor = texture(Backbuffer, st) * vec4(-1, -1, 0, 1);
//										 fragColor = vec4(1, 0, 0, 1);
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= bindDefaults();
			bInitialized *= linkProgram();
		}
		
	public:
		void update(ofFbo& _fbo, ofTexture& _backTex){
			_fbo.begin();
			_backTex.draw(0,0);
			begin();
			setUniformTexture("Backbuffer", _backTex, 0);
			borderVbo.draw(GL_LINES, 0, borderVbo.getNumVertices());
			end();
			_fbo.end();
		}
		void setup(int _w, int _h){
			ofMesh borderMesh;
			borderMesh.setMode(OF_PRIMITIVE_LINES);
			borderMesh.getVertices().resize(8);
			borderMesh.getTexCoords().resize(8);
			
			borderMesh.setVertex(0, glm::vec3(0, .5, 0));
			borderMesh.setTexCoord(0, glm::vec2(0, 1));
			borderMesh.setVertex(1, glm::vec3(_w, .5, 0));
			borderMesh.setTexCoord(1, glm::vec2(_w, 1));
			
			borderMesh.setVertex(2, glm::vec3(0, _h-.5, 0));
			borderMesh.setTexCoord(2, glm::vec2(0, _h-1));
			borderMesh.setVertex(3, glm::vec3(_w, _h-.5, 0));
			borderMesh.setTexCoord(3, glm::vec2(_w, _h-1));
			
			borderMesh.setVertex(4, glm::vec3(.5, 0, 0));
			borderMesh.setTexCoord(4, glm::vec2(1, 0));
			borderMesh.setVertex(5, glm::vec3(.5, _h, 0));
			borderMesh.setTexCoord(5, glm::vec2(1, _h));
			
			borderMesh.setVertex(6, glm::vec3(_w-.5, 0, 0));
			borderMesh.setTexCoord(6, glm::vec2(_w-1, 0));
			borderMesh.setVertex(7, glm::vec3(_w-.5, _h, 0));
			borderMesh.setTexCoord(7, glm::vec2(_w-1, _h));
			
			borderVbo.setMesh(borderMesh, GL_DYNAMIC_DRAW, false, true, false);
		}
		ofVbo borderVbo;
	};
}


