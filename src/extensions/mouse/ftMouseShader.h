
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {
	
	class ftMouseShader : public ftShader {
	public:
		ftMouseShader() {
            bInitialized = 1;
			if (ofIsGLProgrammableRenderer()) { glFour(); } else { glTwo(); }
			string shaderName = "ftMouseShader";
			if (bInitialized) { ofLogVerbose(shaderName + " initialized"); }
			else { ofLogWarning(shaderName + " failed to initialize"); }
		}
		
	protected:
		void glTwo() {
			fragmentShader = GLSL120(
									 uniform vec2	Point;
									 uniform float	Radius;
									 uniform float	EdgeSmooth;
									 uniform vec4	Value;
									 
									 void main(){
										 vec4 color = Value;
										 float d = distance(Point, gl_TexCoord[0].st);
										 float a = max((Radius - d) / Radius, 0.0);
										 a = pow(a, max(EdgeSmooth, 0.00001));
										 gl_FragColor = color * a;
										 
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= linkProgram();
		}
		
		void glFour() {
			fragmentShader = GLSL410(
									 uniform vec2	Point;
									 uniform float	Radius;
									 uniform float	EdgeSmooth;
									 uniform vec4	Value;
									 
									 in vec2 texCoordVarying;
									 out vec4 fragColor;
									 
									 void main(){
										 vec4 color = Value;
										 float d = distance(Point, texCoordVarying);
										 float a = max((Radius - d) / Radius, 0.0);
										 a = pow(a, max(EdgeSmooth, 0.00001));
										 fragColor = color * a;
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= bindDefaults();
			bInitialized *= linkProgram();
		}
		
	public:
		void update(ofFbo& _fbo, glm::vec4 _value, ofPoint _point, float _radius, float _edge){
			_fbo.begin();
			begin();
			setUniform2f("Point", _point.x, _point.y );
			setUniform1f("Radius", _radius );
			setUniform1f("EdgeSmooth", _edge );
			setUniform4f("Value", _value);
			renderFrame(_fbo.getWidth(), _fbo.getHeight());
			end();
			_fbo.end();
		}
	};
}

