
#pragma once

#include "ofMain.h"
#include "ftShader.h"


namespace flowTools {
	
	class ftVelocityFieldShader : public ftShader {
	public:
		ftVelocityFieldShader() {
			
			string vertexShader;
			string geometryShader;
			
			
			vertexShader = STRINGIFY(void main() {
				gl_Position = gl_Vertex;
				gl_FrontColor = gl_Color;
			}
									 );
			
			fragmentShader = STRINGIFY(void main() {
				gl_FragColor = gl_Color;
			}
									   );
			
			geometryShader = STRINGIFY(uniform sampler2DRect fieldTexture;
									   uniform vec2 texResolution;
									   uniform float vectorSize;
									   uniform float maxSize;
									   
									   void main(){
										   
										   for(int i = 0; i < gl_VerticesIn; i++){
											   vec4 lineStart = gl_PositionIn[i];
											   vec2 uv = lineStart.xy * texResolution;
											   vec2 line = texture2DRect(fieldTexture, uv).xy * vectorSize;
											   if (length(line) > maxSize)
												   line = normalize(line) * maxSize;
											   vec4 lineEnd = lineStart + vec4(line, 0.0, 0.0);
											   
											   float alpha = 0.3 + 0.3 * (1.0 - length(line) / maxSize);
											   vec4 color = vec4(1.0, 1.0, 1.0, alpha);
											   
											   float arrowLength = 0.75 * length(line);
											   
											   vec2 nLine = normalize(line);
											   float arrowAngleA = atan(nLine.y, nLine.x) + 0.1;
											   float arrowAngleB = atan(nLine.y, nLine.x) - 0.1;
											   
											   vec4 arrowLineA = vec4(cos(arrowAngleA) ,sin(arrowAngleA), 0., 0.);
											   vec4 arrowLineB = vec4(cos(arrowAngleB) ,sin(arrowAngleB), 0., 0.);
											   arrowLineA = normalize(arrowLineA) * arrowLength;
											   arrowLineB = normalize(arrowLineB) * arrowLength;
											   vec4 arrowA = lineStart +arrowLineA;
											   vec4 arrowB = lineStart +arrowLineB;
											   
											   gl_Position = gl_ModelViewProjectionMatrix * lineStart;
											   gl_FrontColor = color;
											   EmitVertex();
											   
											   gl_Position = gl_ModelViewProjectionMatrix * lineEnd;
											   gl_FrontColor = color;
											   EmitVertex();
											   
											   gl_Position = gl_ModelViewProjectionMatrix * arrowA;
											   gl_FrontColor = color;
											   EmitVertex();
											   
											   gl_Position = gl_ModelViewProjectionMatrix * lineEnd;
											   gl_FrontColor = color;
											   EmitVertex();
											   
											   gl_Position = gl_ModelViewProjectionMatrix * arrowB;
											   gl_FrontColor = color;
											   EmitVertex();
											   
											   EndPrimitive();
										   }
									   }
									   );
			
			shader.setGeometryInputType(GL_POINTS);
			shader.setGeometryOutputType(GL_LINE_STRIP);
			shader.setGeometryOutputCount(5);
			shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			shader.setupShaderFromSource(GL_GEOMETRY_SHADER_EXT, geometryShader);
			shader.linkProgram();
		}
		
		void update(ofVbo& _fieldVbo, ofTexture& _floatTexture, float _vectorSize, float _maxSize){
			int width = _floatTexture.getWidth();
			int height = _floatTexture.getHeight();
			
			shader.begin();
			shader.setUniformTexture("fieldTexture", _floatTexture,0);
			shader.setUniform2f("texResolution", width, height);
			shader.setUniform1f("vectorSize", _vectorSize / width * 10.0);
			shader.setUniform1f("maxSize", _maxSize);
			_fieldVbo.draw(GL_POINTS, 0, _fieldVbo.getNumVertices());
			shader.end();
			
			glFlush();

		}
	};
}