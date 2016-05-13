
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {
	
	class ftSplitVelocityFieldShader : public ftShader {
	public:
		ftSplitVelocityFieldShader() {
			bInitialized = 1;
			
			if (ofIsGLProgrammableRenderer())
				glThree();
			else
				glTwo();
			
			if (bInitialized)
				ofLogNotice("ftSplitVelocityFieldShader initialized");
			else
				ofLogWarning("ftSplitVelocityFieldShader failed to initialize");
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
								  uniform vec4 baseColor;
								  uniform float vectorSize;
								  uniform float maxArrowSize;
									   
								  void main(){
										   
									  vec4 lineStart = gl_PositionIn[0];
									  vec2 uv = lineStart.xy * texResolution;
									  
									  vec4 splitVelocity = texture2DRect(fieldTexture, uv) * vectorSize;
									  
									  vec2 pVel = splitVelocity.xy;
									  if (length(pVel) > maxArrowSize)
										  pVel = normalize(pVel) * maxArrowSize;
									  vec2 nVel = splitVelocity.zw;
									  if (length(nVel) > maxArrowSize)
										  nVel = normalize(nVel) * maxArrowSize;
									  
											   
									  gl_Position = gl_ModelViewProjectionMatrix * (lineStart + vec4(pVel, 0, 0));
									  gl_FrontColor = vec4(normalize(pVel),0,1);
									  EmitVertex();
											   
									  gl_Position = gl_ModelViewProjectionMatrix * lineStart;
									  gl_FrontColor = vec4(0,0,0,.2);
									  EmitVertex();
											   
									  gl_Position = gl_ModelViewProjectionMatrix * (lineStart - vec4(nVel, 0, 0));
									  gl_FrontColor = vec4(0,normalize(nVel),1);
									  EmitVertex();
											   
									  EndPrimitive();
								  }
								  );
			
			ofLogVerbose("Maximum number of output vertices support is: " + ofToString(shader.getGeometryMaxOutputCount()));
			shader.setGeometryInputType(GL_POINTS);
			shader.setGeometryOutputType(GL_LINE_STRIP);
			shader.setGeometryOutputCount(3);
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
									 uniform vec4 baseColor;
									 uniform float vectorSize;
									 uniform float maxArrowSize;
									
									 layout (points) in;
									 layout (line_strip) out;
									 layout (max_vertices=5) out;
									 
									 void main(){
										 vec4 lineStart = gl_in[0].gl_Position;
										 vec2 uv = lineStart.xy * texResolution;
									  
										 vec4 splitVelocity = texture(fieldTexture, uv) * vectorSize;
									  
										 vec2 pVel = splitVelocity.xy;
										 if (length(pVel) > maxArrowSize)
											pVel = normalize(pVel) * maxArrowSize;
										 vec2 nVel = splitVelocity.zw;
										 if (length(nVel) > maxArrowSize)
											nVel = normalize(nVel) * maxArrowSize;
									  
										 gl_Position = gl_ModelViewProjectionMatrix * (lineStart + vec4(pVel, 0, 0));
									//	 gl_FrontColor = vec4(normalize(pVel),0,1);
										 EmitVertex();
										 
										 gl_Position = gl_ModelViewProjectionMatrix * lineStart;
									//	 gl_FrontColor = vec4(0,0,0,.2);
										 EmitVertex();
										 
										 gl_Position = gl_ModelViewProjectionMatrix * (lineStart - vec4(nVel, 0, 0));
									//	 gl_FrontColor = vec4(0,normalize(nVel),1);
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
		void update(ofVbo& _fieldVbo, ofTexture& _floatTexture, float _velocityScale, float _maxArrowSize, ofFloatColor _color = ofFloatColor(1,1,1,1)){
			int width = _floatTexture.getWidth();
			int height = _floatTexture.getHeight();
			
			shader.begin();
			shader.setUniformTexture("fieldTexture", _floatTexture,0);
			shader.setUniform2f("texResolution", width, height);
			shader.setUniform4f("baseColor", _color.r, _color.g, _color.b, _color.a);
			shader.setUniform1f("vectorSize", _velocityScale);
			shader.setUniform1f("maxArrowSize", _maxArrowSize);
			_fieldVbo.draw(GL_POINTS, 0, _fieldVbo.getNumVertices());
			shader.end();
		}
	};
}