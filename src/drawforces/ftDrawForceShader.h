
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {

	class ftDrawForceShader : public ftShader {
	public:
		ftDrawForceShader() {
			bInitialized = 1;
			
			if (ofIsGLProgrammableRenderer())
				glThree();
			else
				glTwo();
			
			if (bInitialized)
				ofLogNotice("ftDrawForceShader initialized");
			else
				ofLogWarning("ftDrawForceShader failed to initialize");
		}
		
	protected:
		void glTwo() {
			fragmentShader = GLSL120(
								  uniform vec2		Point;
								  uniform float	Radius;
								  uniform float	EdgeSmooth;
								  uniform vec4		Value;
								  
								  void main(){
									  vec4 color = Value;
									  float d = distance(Point, gl_TexCoord[0].st);
									  float a = max((Radius - d) / Radius, 0.0);
									  float c = ceil(a);
									  color.xyz *= c;
									  color.w *= pow(a, EdgeSmooth + 0.1);
									  gl_FragColor = color;
									  
								  }
								  );
			
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.linkProgram();

			
		}
		
		void glThree() {
			
			fragmentShader = GLSL150(
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
									  float c = ceil(a);
									  color.xyz *= c;
									  color.w *= pow(a, EdgeSmooth + 0.1);
									  fragColor = color;
								  }
								  );
			
			bInitialized *= shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.bindDefaults();
			bInitialized *= shader.linkProgram();
		}
		
	public:
		
		void update(ofFbo& _buffer, ofVec4f _value, ofPoint _point, float _radius, float _edge){
			
			_buffer.begin();
			shader.begin();
			
			shader.setUniform2f("Point", _point.x, _point.y );
			shader.setUniform1f("Radius", _radius );
			shader.setUniform1f("EdgeSmooth", _edge );
			shader.setUniform4f("Value", _value.x, _value.y, _value.z, _value.w);
			renderFrame(_buffer.getWidth(), _buffer.getHeight());
			
			shader.end();
			_buffer.end();
			
		}
		
	private:
	};
}