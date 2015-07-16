
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {
	
	class ftVelocityFieldShader : public ftShader {
	public:
		ftVelocityFieldShader() {
			bInitialized = 1;
			
			if (ofGetGLProgrammableRenderer())
				glThree();
			else
				glTwo();
			
			if (bInitialized)
				ofLogNotice("ftVelocityFieldShader initialized");
			else
				ofLogWarning("ftVelocityFieldShader failed to initialize");
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
								  uniform float maxArrowSize;
									   
								  void main(){
										   
									  vec4 lineStart = gl_PositionIn[0];
									  vec2 uv = lineStart.xy * texResolution;
									  vec2 line = texture2DRect(fieldTexture, uv).xy * vectorSize;
									  if (length(line) > maxArrowSize)
										  line = normalize(line) * maxArrowSize;
									  vec4 lineEnd = lineStart + vec4(line, 0.0, 0.0);
											   
									  float alpha = 0.3 + 0.3 * (1.0 - length(line) / maxArrowSize);
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
								  );
			
			ofLogVerbose("Maximum number of output vertices support is: " + ofToString(shader.getGeometryMaxOutputCount()));
			shader.setGeometryInputType(GL_POINTS);
			shader.setGeometryOutputType(GL_LINE_STRIP);
			shader.setGeometryOutputCount(5);
			bInitialized *= shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.setupShaderFromSource(GL_GEOMETRY_SHADER_EXT, geometryShader);
			bInitialized *= shader.linkProgram();

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
									 uniform float maxArrowSize;
									
									 layout (points) in;
									 layout (line_strip) out;
									 layout (max_vertices=5) out;
									 
									 void main(){
										 vec4 lineStart = gl_in[0].gl_Position;;
										 
										 vec2 uv = lineStart.xy * texResolution;
										 vec2 line = texture(fieldTexture, uv).xy * vectorSize;
										 if (length(line) > maxArrowSize)
											 line = normalize(line) * maxArrowSize;
										 vec4 lineEnd = lineStart + vec4(line, 0.0, 0.0);
										 
										 float alpha = 0.3 + 0.3 * (1.0 - length(line) / maxArrowSize);
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
										 
										 gl_Position = modelViewProjectionMatrix * lineStart;
										 EmitVertex();
										 
										 gl_Position = modelViewProjectionMatrix * lineEnd;
										 EmitVertex();
										 
										 gl_Position = modelViewProjectionMatrix * arrowA;
										 EmitVertex();
										 
										 gl_Position = modelViewProjectionMatrix * lineEnd;
										 EmitVertex();
										 
										 gl_Position =  modelViewProjectionMatrix * arrowB;
										 EmitVertex();
										 
										 EndPrimitive();
										 
										}
										);
			
			fragmentShader = GLSL150(
									 out vec4 fragColor;
									 
									 void main()
									 {
										 fragColor = vec4(1.0,1.0,1.0,1.0);
									 }
									 );
			
			bInitialized *= shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.setupShaderFromSource(GL_GEOMETRY_SHADER_EXT, geometryShader);
			bInitialized *= shader.bindDefaults();
			bInitialized *= shader.linkProgram();
		}
	
	public:	
		void update(ofVbo& _fieldVbo, ofTexture& _floatTexture, float _velocityScale, float _maxArrowSize){
			int width = _floatTexture.getWidth();
			int height = _floatTexture.getHeight();
			
			shader.begin();
			shader.setUniformTexture("fieldTexture", _floatTexture,0);
			shader.setUniform2f("texResolution", width, height);
			shader.setUniform1f("vectorSize", _velocityScale);
			shader.setUniform1f("maxArrowSize", _maxArrowSize);
			_fieldVbo.draw(GL_POINTS, 0, _fieldVbo.getNumVertices());
			shader.end();
		}
	};
}