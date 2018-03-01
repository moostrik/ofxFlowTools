
#pragma once

#include "ofMain.h"
#include "ftShader.h"


namespace flowTools {
	
	class ftTemperatureFieldShader : public ftShader {
	public:
		ftTemperatureFieldShader() {
			bInitialized = 1;
			
			if (ofIsGLProgrammableRenderer())
				glThree();
			else
				glTwo();
			
			if (bInitialized)
				ofLogNotice("ftTemperatureFieldShader initialized");
			else
				ofLogWarning("ftTemperatureFieldShader failed to initialize");
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
										uniform sampler2DRect temperatureTexture;
										uniform vec2 texResolution;
										uniform float temperatureScale;
										uniform float maxHeight;
								  
										void main(){
											vec4 lineStart = gl_PositionIn[0];
											vec2 uv = lineStart.xy * texResolution;
											
											float temperature = texture2DRect(temperatureTexture, uv).x * temperatureScale;
											temperature = min(temperature, maxHeight);
											temperature = max(temperature, -maxHeight);
											
											vec4 lineEnd = lineStart + vec4(0.0, -temperature, 0.0, 0.0);
										 
											float alpha = 0.5 + 0.5 * (abs(temperature) / maxHeight);
											float red = max(0.0, temperature * 1000.);
											float blue = max(0.0, -temperature * 1000.);
											vec4 color = vec4(red, 0.0, blue, alpha);
											
											float arrowLength = 0.75 * temperature;
											
											gl_Position = gl_ModelViewProjectionMatrix * lineStart;
											gl_FrontColor = vec4(1.0,1.0,1.0,0.0);
											EmitVertex();
											
											gl_Position = gl_ModelViewProjectionMatrix * lineEnd;
											gl_FrontColor = color;
											EmitVertex();
											
											EndPrimitive();
										}
										);
			
			ofLogVerbose("Maximum number of output vertices support is: " + ofToString(shader.getGeometryMaxOutputCount()));
			shader.setGeometryInputType(GL_POINTS);
			shader.setGeometryOutputType(GL_LINE_STRIP);
			shader.setGeometryOutputCount(2);
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
									 uniform sampler2DRect temperatureTexture;
									 uniform vec2 texResolution;
									 uniform float temperatureScale;
									 uniform float maxHeight;
									 uniform float lineWidth;
									 
									 layout (points) in;
									 layout (triangle_strip) out;
									 layout (max_vertices=4) out;
									 
									 out vec4 colorVarying;
									 
									 void main(){
										 
										 vec4 lineStart = gl_in[0].gl_Position;
										 vec2 uv = lineStart.xy * texResolution;
										 
										 float temperature = texture(temperatureTexture, uv).x * temperatureScale;
										 temperature = min(temperature, maxHeight);
										 temperature = max(temperature, -maxHeight);
										 
										 vec4 lineEnd = lineStart + vec4(0.0, -temperature, 0.0, 0.0);
										 
										 float alpha = 0.5 + 0.5 * (abs(temperature) / maxHeight);
										 float red = max(0.0, temperature * 1000.);
										 float blue = max(0.0, -temperature * 1000.);
										 vec4 color = vec4(red, 0.0, blue, alpha);
										 
										 float arrowLength = 0.75 * temperature;
										 
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
			
			bInitialized *= shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.setupShaderFromSource(GL_GEOMETRY_SHADER_EXT, geometryShader);
			bInitialized *= shader.bindDefaults();
			bInitialized *= shader.linkProgram();

		}
		
	public:
		
		void update(ofVbo& _fieldVbo, ofTexture& _temperatureTexture, float _temperatureScale, float _barHeight, float _barWidth){
			int width = _temperatureTexture.getWidth();
			int height = _temperatureTexture.getHeight();
			
			shader.begin();
			shader.setUniformTexture("temperatureTexture", _temperatureTexture,0);
			shader.setUniform2f("texResolution", width, height);
			shader.setUniform1f("temperatureScale", _temperatureScale);
			shader.setUniform1f("maxHeight", _barHeight);
			
			glLineWidth(_barWidth); // for openGL 2
			shader.setUniform1f("lineWidth", _barWidth / ofGetWindowWidth());  // for openGL 3
			
			_fieldVbo.draw(GL_POINTS, 0, _fieldVbo.getNumVertices());
			shader.end();
			
			glFlush();
			
		}
	};
}