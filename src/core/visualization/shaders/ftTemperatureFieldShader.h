
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {
	
	class ftTemperatureFieldShader : public ftShader {
	public:
		ftTemperatureFieldShader() {
            bInitialized = 1;
			if (ofIsGLProgrammableRenderer()) { glFour(); } else { glTwo(); }
			string shaderName = "ftTemperatureFieldShader";
			if (bInitialized) { ofLogVerbose(shaderName + " initialized"); }
			else { ofLogWarning(shaderName + " failed to initialize"); }
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
			ofLogVerbose("Maximum number of output vertices support is: " + ofToString(getGeometryMaxOutputCount()));
			setGeometryInputType(GL_POINTS);
			setGeometryOutputType(GL_LINE_STRIP);
			setGeometryOutputCount(2);

			bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= setupShaderFromSource(GL_GEOMETRY_SHADER_EXT, geometryShader);
			bInitialized *= linkProgram();
		}
		
		void glFour() {
			string geometryShader;
			
			vertexShader = GLSL410(
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
			
			geometryShader = GLSL410(
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
			
			fragmentShader = GLSL410(
									 in vec4 colorVarying;
									 out vec4 fragColor;
									 
									 void main()
									 {
										 fragColor = colorVarying;
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= setupShaderFromSource(GL_GEOMETRY_SHADER_EXT, geometryShader);
			bInitialized *= bindDefaults();
			bInitialized *= linkProgram();
		}
		
	public:
		void update(ofVbo& _fieldVbo, ofTexture& _temTex, float _temperatureScale, float _barHeight, float _barWidth){
			int width = _temTex.getWidth();
			int height = _temTex.getHeight();
			
			begin();
			setUniformTexture("temperatureTexture", _temTex,0);
			setUniform2f("texResolution", width, height);
			setUniform1f("temperatureScale", _temperatureScale);
			setUniform1f("maxHeight", _barHeight);
			
			glLineWidth(_barWidth); // for openGL 2
			setUniform1f("lineWidth", _barWidth / ofGetWindowWidth());  // for openGL 3
			
			_fieldVbo.draw(GL_POINTS, 0, _fieldVbo.getNumVertices());
			end();
			
			glFlush();
			
		}
	};
}
