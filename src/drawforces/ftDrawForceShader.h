
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {

	class ftDrawForceShader : public ftShader {
	public:
		ftDrawForceShader() {
			
			fragmentShader = STRINGIFY(uniform vec2		Point;
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
			
			shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			shader.linkProgram();
			
		}
		
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