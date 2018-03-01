
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {
	
	class ftVelocitySpheresShader : public ftShader {
	public:
		ftVelocitySpheresShader() {
			bInitialized = 1;
			
			if (ofIsGLProgrammableRenderer())
				glThree();
			else
				glTwo();
			
			if (bInitialized)
				ofLogNotice("ftVelocitySpheresShader initialized");
			else
				ofLogWarning("ftVelocitySpheresShader failed to initialize");
		}
		
	protected:
		void glTwo() {
			string geometryShader;
			
			
			ofLogWarning("Velocity Dots not supported for GLSL 120");
			
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
			
			
			ofLogVerbose("Maximum number of output vertices support is: " + ofToString(shader.getGeometryMaxOutputCount()));
			shader.setGeometryInputType(GL_POINTS);
			shader.setGeometryOutputCount(5);
			bInitialized *= shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.linkProgram();
			
		}
		
		void glThree() {
			
			string geometryShader;
			
			vertexShader = GLSL150(
								   uniform mat4 modelViewProjectionMatrix;
								   uniform mat4 textureMatrix;
								   uniform sampler2DRect velocityTexture;
								   uniform vec2 texResolution;
								   uniform float displacementScale;
								   uniform float sizeScale;
								   uniform float minDotSize;
								   uniform float maxDotSize;
								   
								   in vec4 position;
								   in vec2 texcoord;
								   in vec4 color;
								   
								   out vec2 texCoordVarying;
								   out vec4 colorVarying;
								   
								   
								   void main()
								   {
									   colorVarying = color;
									   texCoordVarying = (textureMatrix*vec4(texcoord.x,texcoord.y,0,1)).xy;
									   
									   vec2 st = position.xy * texResolution;
									   vec2 vel = texture(velocityTexture, st).xy;
									   
									   vec2 displacement = vel / vec2(displacementScale, displacementScale);
									   gl_Position = modelViewProjectionMatrix * position + vec4(displacement, 0.0, 1.0);
									   
									   gl_PointSize = max(maxDotSize - (length(vel) * sizeScale), minDotSize);
								   }
								   
								   );
			
			fragmentShader = GLSL150(
									 in vec4 colorVarying;
									 out vec4 fragColor;
									 
									 void main()
									 {
										vec2 p = gl_PointCoord * 2.0 - vec2(1.0);
										float d = dot(p,p);
										float r = sqrt(d);
								
										if(d > r)
											discard;
										else
											fragColor = colorVarying * vec4(1.0, 1.0, 1.0, 1.0 - pow(r, 8.5)); // power is for gradient edge
									 }
									 );
			
			bInitialized *= shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.bindDefaults();
			bInitialized *= shader.linkProgram();
		}
		
	public:
		void update(ofVbo& _fieldVbo, ofTexture& _velocityTexture, float _displacementScale, float _sizeScale, float _minDotSize, float _maxDotSize){
			int width = _velocityTexture.getWidth();
			int height = _velocityTexture.getHeight();
			
			shader.begin();
			shader.setUniformTexture("velocityTexture", _velocityTexture,0);
			shader.setUniform2f("texResolution", width, height);
			shader.setUniform1f("displacementScale", (_displacementScale == 0)? 10: _displacementScale );
			shader.setUniform1f("sizeScale", _sizeScale);
			shader.setUniform1f("minDotSize", _minDotSize);
			shader.setUniform1f("maxDotSize", _maxDotSize);
			
			glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
			_fieldVbo.draw(GL_POINTS, 0, _fieldVbo.getNumVertices());
			glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
			
			shader.end();
		}
	};
}