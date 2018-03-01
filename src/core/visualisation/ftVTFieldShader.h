
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {
	
	class ftVTFieldShader : public ftShader {
	public:
		ftVTFieldShader() {
			bInitialized = 1;
			
			if (ofIsGLProgrammableRenderer())
				glThree();
			else
				glTwo();
			
			if (bInitialized)
				ofLogNotice("ftVTFieldShader initialized");
			else
				ofLogWarning("ftVTFieldShader failed to initialize");
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
										uniform sampler2DRect velocityTexture;
										uniform sampler2DRect temperatureTexture;
										uniform vec2 texResolution;
										uniform float velocityScale;
										uniform float temperatureScale;
										uniform float maxArrowSize;
										
										void main(){
									  
											vec4 lineStart = gl_PositionIn[0];
											vec2 uv = lineStart.xy * texResolution;
											vec2 velocity = texture2DRect(velocityTexture, uv).xy * velocityScale;
											if (length(velocity) > maxArrowSize)
												velocity = normalize(velocity) * maxArrowSize;
											vec4 lineEnd = lineStart + vec4(velocity, 0.0, 0.0);
											
											float alpha = 0.3 + 0.3 * (length(velocity) / maxArrowSize);
											
											float temperature = texture2DRect(temperatureTexture, uv).x * temperatureScale;
											float warm = pow(max(0.0, temperature), 0.5);
											float cold = pow(max(0.0, -temperature), 0.5);
											float red = 1.0 - cold;
											float green = 1.0 - cold - warm;
											float blue = 1.0 - warm;
											
											vec4 color = vec4(red, green, blue, alpha);
											
											float arrowLength = 0.75 * length(velocity);
											
											vec2 nVel = normalize(velocity);
											float arrowAngleA = atan(nVel.y, nVel.x) + 0.2;
											float arrowAngleB = atan(nVel.y, nVel.x) - 0.2;
											
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
								   
								   in vec4	position;
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
									 uniform sampler2DRect velocityTexture;
									 uniform sampler2DRect temperatureTexture;
									 
									 uniform vec2 texResolution;
									 uniform float velocityScale;
									 uniform float temperatureScale;
									 uniform float maxArrowSize;
									 
									 layout (points) in;
									 layout (line_strip) out;
									 layout (max_vertices=5) out;
									 
									 out vec4 colorVarying;
									 
									 void main(){
										 vec4 lineStart = gl_in[0].gl_Position;;
										 
										 vec2 uv = lineStart.xy * texResolution;
										 vec2 velocity = texture(velocityTexture, uv).xy * velocityScale;
										 if (length(velocity) > maxArrowSize)
											 velocity = normalize(velocity) * maxArrowSize;
										 vec4 lineEnd = lineStart + vec4(velocity, 0.0, 0.0);
										 
										 float alpha = 0.3 + 0.3 * (length(velocity) / maxArrowSize);
										 
										 float temperature = texture(temperatureTexture, uv).x * temperatureScale;
										 float warm = pow(max(0.0, temperature), 0.5);
										 float cold = pow(max(0.0, -temperature), 0.5);
										 float red = 1.0 - cold;
										 float green = 1.0 - cold - warm;
										 float blue = 1.0 - warm;
										 
										 vec4 color = vec4(red, green, blue, alpha);
										 
										 float arrowLength = 0.75 * length(velocity);
										 
										 vec2 nVel = normalize(velocity);
										 float arrowAngleA = atan(nVel.y, nVel.x) + 0.15;
										 float arrowAngleB = atan(nVel.y, nVel.x) - 0.15;
										 
										 vec4 arrowLineA = vec4(cos(arrowAngleA) ,sin(arrowAngleA), 0., 0.);
										 vec4 arrowLineB = vec4(cos(arrowAngleB) ,sin(arrowAngleB), 0., 0.);
										 arrowLineA = normalize(arrowLineA) * arrowLength;
										 arrowLineB = normalize(arrowLineB) * arrowLength;
										 vec4 arrowA = lineStart +arrowLineA;
										 vec4 arrowB = lineStart +arrowLineB;
										 
										 gl_Position = modelViewProjectionMatrix * lineStart;
										 colorVarying = color;
										 EmitVertex();
										 
										 gl_Position = modelViewProjectionMatrix * lineEnd;
										 colorVarying = color;
										 EmitVertex();
										 
										 gl_Position = modelViewProjectionMatrix * arrowA;
										 colorVarying = color;
										 EmitVertex();
										 
										 gl_Position = modelViewProjectionMatrix * lineEnd;
										 colorVarying = color;
										 EmitVertex();
										 
										 gl_Position =  modelViewProjectionMatrix * arrowB;
										 colorVarying = color;
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
		void update(ofVbo& _fieldVbo, ofTexture& _velocityTexture, ofTexture& _temperatureTexture, float _velocityScale, float _temperatureScale, float _maxArrowSize){
			int width = _velocityTexture.getWidth();
			int height = _velocityTexture.getHeight();
			
			shader.begin();
			shader.setUniformTexture("velocityTexture", _velocityTexture,0);
			shader.setUniformTexture("temperatureTexture", _temperatureTexture,1);
			shader.setUniform2f("texResolution", width, height);
			shader.setUniform1f("velocityScale", _velocityScale);
			shader.setUniform1f("temperatureScale", _temperatureScale);
			shader.setUniform1f("maxArrowSize", _maxArrowSize);
			_fieldVbo.draw(GL_POINTS, 0, _fieldVbo.getNumVertices());
			shader.end();
		}
	};
}