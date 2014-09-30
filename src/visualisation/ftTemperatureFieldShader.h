
#pragma once

#include "ofMain.h"
#include "ftShader.h"


namespace flowTools {
	
	class ftTemperatureFieldShader : public ftShader {
	public:
		ftTemperatureFieldShader() {
			
			ofLogVerbose("init ftTemperatureFieldShader");
			if (isProgrammableRenderer)
				glThree();
			else
				glTwo();
		}
		
	protected:
		void glTwo() {
			string geometryShader;
			
			vertexShader = GLSL120(
								void main() {
									gl_Position = gl_Vertex;
									gl_FrontColor = gl_Color;
								}
								);
			
			fragmentShader = GLSL120(
								  void main() {
									  gl_FragColor = gl_Color;
								  }
								  );
			
			geometryShader = GLSL120GEO(
								  uniform sampler2DRect fieldTexture;
								  uniform vec2 texResolution;
								  uniform float vectorSize;
								  uniform float maxSize;
								  
								  void main(){
									  
								//	  for(int i = 0; i < gl_VerticesIn; i++){
										  vec4 lineStart = gl_PositionIn[0];
										  vec2 uv = lineStart.xy * texResolution;
										  float line = texture2DRect(fieldTexture, uv).x * vectorSize;
										  if (line > maxSize)
											  line = maxSize;
										  vec4 lineEnd = lineStart + vec4(0.0, -line, 0.0, 0.0);
										  
										  float alpha = 0.5 + 0.5 * (abs(line) / maxSize);
										  float red = max(0.0, line * 1000.);
										  float blue = max(0.0, -line * 1000.);
										  vec4 color = vec4(red, 0.0, blue, alpha);
										  
										  float arrowLength = 0.75 * line;
										  
										  gl_Position = gl_ModelViewProjectionMatrix * lineStart;
										  gl_FrontColor = vec4(1.0,1.0,1.0,0.0);
										  EmitVertex();
										  
										  gl_Position = gl_ModelViewProjectionMatrix * lineEnd;
										  gl_FrontColor = color;
										  EmitVertex();
										  
										  EndPrimitive();
								//	  }
								  }
								  );
			
			ofLogVerbose("Maximum number of output vertices support is: " + ofToString(shader.getGeometryMaxOutputCount()));
			shader.setGeometryInputType(GL_POINTS);
			shader.setGeometryOutputType(GL_LINE_STRIP);
			shader.setGeometryOutputCount(2);
			shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			shader.setupShaderFromSource(GL_GEOMETRY_SHADER_EXT, geometryShader);
			shader.linkProgram();
		}
		
		void glThree() {
			string geometryShader;
			
			vertexShader = GLSL150(
								uniform mat4 modelViewProjectionMatrix;
								uniform mat4 textureMatrix;
								
								in vec4 position;
								in vec2	texcoord;
								in vec4	color;
								
								out vec2 texCoordVarying;
								out vec4 colorVarying;
								
								void main()
								{
									colorVarying = color;
									gl_Position = position;
								}
								
								);
			
			geometryShader = GLSL150(
								  uniform mat4 modelViewProjectionMatrix;
								  uniform sampler2DRect fieldTexture;
								  uniform vec2 texResolution;
								  uniform float vectorSize;
								  uniform float maxSize;
								  uniform float lineWidth;
								  
								  layout (points) in;
								  layout (triangle_strip) out;
								  layout (max_vertices=4) out;
								  
								  out vec4 colorVarying;
								  
								  void main(){
									  
									  vec4 lineStart = gl_in[0].gl_Position;;
									  
									  vec2 uv = lineStart.xy * texResolution;
									  float line = texture(fieldTexture, uv).x * vectorSize;
									  if (line > maxSize)
										  line = maxSize;
									  vec4 lineEnd = lineStart + vec4(0.0, -line, 0.0, 0.0);
									  
									  float alpha = 0.5 + 0.5 * (abs(line) / maxSize);
									  float red = max(0.0, line * 1000.);
									  float blue = max(0.0, -line * 1000.);
									  vec4 color = vec4(red, 0.0, blue, alpha);
									  
									  float arrowLength = 0.75 * line;
									  
									  lineStart.x -= lineWidth * 0.5;
									  gl_Position = modelViewProjectionMatrix * lineStart;
									  colorVarying = vec4(1.0,1.0,1.0,0.0);
									  EmitVertex();
									  
									  
									  lineStart.x += lineWidth;
									  gl_Position = modelViewProjectionMatrix * lineStart;
									  colorVarying = vec4(1.0,1.0,1.0,0.0);
									  EmitVertex();
									  
									  lineEnd.x -= lineWidth * 0.5;
									  gl_Position = modelViewProjectionMatrix * lineEnd;
									  colorVarying= color;
									  EmitVertex();
									  
									  lineEnd.x += lineWidth;
									  gl_Position = modelViewProjectionMatrix * lineEnd;
									  colorVarying= color;
									  EmitVertex();
									  
									  EndPrimitive();
								  }
								  );
			
			fragmentShader = GLSL150(
								  
								  in vec4 colorVarying;
								  out vec4 fragColor;
								  
								  void main()
								  {
									  fragColor = colorVarying;
								  }
								  );
			
			shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			shader.setupShaderFromSource(GL_GEOMETRY_SHADER_EXT, geometryShader);
			shader.bindDefaults();
			shader.linkProgram();

		}
		
	public:
		
		void update(ofVbo& _fieldVbo, ofTexture& _floatTexture, float _vectorSize, float _maxSize){
			int width = _floatTexture.getWidth();
			int height = _floatTexture.getHeight();
			
			
			shader.begin();
			shader.setUniformTexture("fieldTexture", _floatTexture,0);
			shader.setUniform2f("texResolution", width, height);
			shader.setUniform1f("vectorSize", _vectorSize / width * 10.0);
			shader.setUniform1f("maxSize", _maxSize);
			
			glLineWidth(5); // for openGL 2
			shader.setUniform1f("lineWidth", 5.0 / ofGetWindowWidth());  // for openGL 3
			
			_fieldVbo.draw(GL_POINTS, 0, _fieldVbo.getNumVertices());
			shader.end();
			
			glFlush();
			
		}
	};
}