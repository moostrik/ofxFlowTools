
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {
	
	class ftPressureFieldShader : public ftShader {
	public:
		ftPressureFieldShader() {
            bInitialized = 1;
			if (ofIsGLProgrammableRenderer()) { glThree(); } else { glTwo(); }
			string shaderName = "ftPressureFieldShader";
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
										uniform sampler2DRect pressureTexture;
										uniform vec2 texResolution;
										uniform float pressureScale;
										uniform float maxRadius;
										
										void main(){
											vec4 centre = gl_PositionIn[0];
											vec2 uv = centre.xy * texResolution;
											
											float pressure = texture2DRect(pressureTexture, uv).x * pressureScale;
											pressure = min(pressure, maxRadius);
											pressure = max(pressure, -maxRadius);
											float aspectRatio = texResolution.x / texResolution.y;
											
											vec4 north = centre + vec4(pressure, 0, 0, 0);
											vec4 east  = centre + vec4(0, pressure * aspectRatio, 0, 0);
											vec4 south = centre + vec4(-pressure, 0, 0, 0);
											vec4 west  = centre + vec4(0, -pressure * aspectRatio, 0, 0);
											
											float normalizedPressure = pressure / maxRadius;
											float highPressure = pow(max(0.0, normalizedPressure), 0.5);
											float lowPressure = pow(max(0.0, -normalizedPressure), 0.5);
											float red = 1.0 - lowPressure;
											float green = 1.0;
											float blue = 1.0 - highPressure;
											float alpha = 0.3 + 0.3 * (abs(normalizedPressure));
											
											vec4 color = vec4(red, green, blue, alpha);
											
											gl_Position = gl_ModelViewProjectionMatrix * north;
											gl_FrontColor = color;
											EmitVertex();
											
											gl_Position = gl_ModelViewProjectionMatrix * east;
											gl_FrontColor = color;
											EmitVertex();
											
											gl_Position = gl_ModelViewProjectionMatrix * south;
											gl_FrontColor = color;
											EmitVertex();
											
											gl_Position = gl_ModelViewProjectionMatrix * west;
											gl_FrontColor = color;
											EmitVertex();
											
											gl_Position = gl_ModelViewProjectionMatrix * north;
											gl_FrontColor = color;
											EmitVertex();
										}
										);
			
			ofLogVerbose("Maximum number of output vertices support is: " + ofToString(getGeometryMaxOutputCount()));
			setGeometryInputType(GL_POINTS);
			setGeometryOutputType(GL_LINE_STRIP);
			setGeometryOutputCount(5);
			
			bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= setupShaderFromSource(GL_GEOMETRY_SHADER_EXT, geometryShader);
			bInitialized *= linkProgram();
		}
		
		void glThree() {
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
									 uniform sampler2DRect pressureTexture;
									 uniform vec2 texResolution;
									 uniform float pressureScale;
									 uniform float maxRadius;
									 
									 layout (points) in;
									 layout (line_strip) out;
									 layout (max_vertices=5) out;
									 
									 out vec4 colorVarying;
									 
									 void main(){
										 
										 vec4 centre = gl_in[0].gl_Position;
										 vec2 uv = centre.xy * texResolution;
										 
										 float pressure = texture(pressureTexture, uv).x * pressureScale;
										 pressure = min(pressure, maxRadius);
										 pressure = max(pressure, -maxRadius);
										 float aspectRatio = texResolution.x / texResolution.y;
										 
										 vec4 north = centre + vec4(pressure, 0, 0, 0);
										 vec4 east  = centre + vec4(0, pressure * aspectRatio, 0, 0);
										 vec4 south = centre + vec4(-pressure, 0, 0, 0);
										 vec4 west  = centre + vec4(0, -pressure * aspectRatio, 0, 0);
										 
										 float normalizedPressure = pressure / maxRadius;
										 float highPressure = pow(max(0.0, normalizedPressure), 0.5);
										 float lowPressure = pow(max(0.0, -normalizedPressure), 0.5);
										 float red = 1.0 - lowPressure;
										 float green = 1.0;// - lowPressure - highPressure;
										 float blue = 1.0 - highPressure;
										 float alpha = 0.3 + 0.3 * (abs(normalizedPressure));
										 
										 vec4 color = vec4(red, green, blue, alpha);
										 
										 gl_Position = modelViewProjectionMatrix * north;
										 colorVarying = color;
										 EmitVertex();
										 
										 gl_Position = modelViewProjectionMatrix * east;
										 colorVarying = color;
										 EmitVertex();
										 
										 gl_Position = modelViewProjectionMatrix * south;
										 colorVarying = color;
										 EmitVertex();
										 
										 gl_Position = modelViewProjectionMatrix * west;
										 colorVarying = color;
										 EmitVertex();
										 
										 gl_Position = modelViewProjectionMatrix * north;
										 colorVarying = color;
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
		void update(ofVbo& _fieldVbo, ofTexture& _prsTexture, float _pressureScale, float _radius){
			int width = _prsTexture.getWidth();
			int height = _prsTexture.getHeight();
			//			glEnable(GL_CULL_FACE);
			begin();
			setUniformTexture("pressureTexture", _prsTexture, 0);
			setUniform2f("texResolution", width, height);
			setUniform1f("pressureScale", _pressureScale);
			setUniform1f("maxRadius", _radius);
			_fieldVbo.draw(GL_POINTS, 0, _fieldVbo.getNumVertices());
			end();
			//			glDisable(GL_CULL_FACE);
			glFlush();
		}
	};
}

