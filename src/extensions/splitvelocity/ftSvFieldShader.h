
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {
	
	class ftSvFieldShader : public ftShader {
	public:
		ftSvFieldShader() {
            bInitialized = 1;
			if (ofIsGLProgrammableRenderer()) { glThree(); } else { glTwo(); }
			string shaderName = "ftSvFieldShader";
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
										uniform sampler2DRect fieldTexture;
										uniform vec2 texResolution;
										uniform vec4 baseColor;
										uniform float vectorSize;
										uniform float arrowSize;
										
										void main(){
											
											vec4 lineStart = gl_PositionIn[0];
											vec2 uv = lineStart.xy * texResolution;
											
											vec4 splitVelocity = texture2DRect(fieldTexture, uv) * arrowSize;
											
											vec2 pVel = splitVelocity.xy;
											vec2 nVel = splitVelocity.zw;
											
											
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
			
			ofLogVerbose("Maximum number of output vertices support is: " + ofToString(getGeometryMaxOutputCount()));
			setGeometryInputType(GL_POINTS);
			setGeometryOutputType(GL_LINE_STRIP);
			setGeometryOutputCount(3);
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
								   
								   in vec4	position;
								   in vec2	texcoord;
								   in vec4	color;
								   
								   out vec2 texCoordVarying;
								   out vec4 vColor;
								   
								   void main()
								   {
									   vColor = color;
									   gl_Position = position;
								   }
								   
								   );
			
			geometryShader = GLSL410(
									 uniform mat4 modelViewProjectionMatrix;
									 uniform sampler2DRect fieldTexture;
									 uniform vec2 texResolution;
									 uniform vec4 baseColor;
									 uniform float arrowSize;
									 
									 layout (points) in;
									 layout (line_strip) out;
									 layout (max_vertices=3) out;
									 
									 in vec4 vColor[];
									 out vec4 gColor;
									 
									 void main(){
										 vec4 lineStart = gl_in[0].gl_Position;
										 vec2 uv = lineStart.xy * texResolution;
										 
										 vec4 splitVelocity = texture(fieldTexture, uv) * arrowSize;
										 
										 vec2 pVel = splitVelocity.xy;
										 vec2 nVel = splitVelocity.zw;
										 
										 gl_Position = modelViewProjectionMatrix * (lineStart + vec4(pVel, 0, 0));
										 gColor = vec4(normalize(pVel),0,1);
										 EmitVertex();
										 
										 gl_Position = modelViewProjectionMatrix * lineStart;
										 gColor = vec4(0,0,0,.2);
										 EmitVertex();
										 
										 gl_Position = modelViewProjectionMatrix * (lineStart - vec4(nVel, 0, 0));
										 gColor = vec4(0,normalize(nVel),1);
										 EmitVertex();
										 
										 EndPrimitive();
									 }
									 );
			
			fragmentShader = GLSL410(
									 in vec4 gColor;
									 out vec4 fragColor;
									 
									 void main()
									 {
										 fragColor = gColor;
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= setupShaderFromSource(GL_GEOMETRY_SHADER_EXT, geometryShader);
			bInitialized *= bindDefaults();
			bInitialized *= linkProgram();
		}
		
	public:
		void update(ofVbo& _fieldVbo, ofTexture& _floatTex, float _arrowSize, ofFloatColor _color = ofFloatColor(1,1,1,1)){
			int width = _floatTex.getWidth();
			int height = _floatTex.getHeight();
			begin();
			setUniformTexture("fieldTexture", _floatTex,0);
			setUniform2f("texResolution", width, height);
			setUniform4f("baseColor", _color.r, _color.g, _color.b, _color.a);
			setUniform1f("arrowSize", _arrowSize);
			_fieldVbo.draw(GL_POINTS, 0, _fieldVbo.getNumVertices());
			end();
		}
	};
}

