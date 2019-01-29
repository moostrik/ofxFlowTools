
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {
	
	class ftSvToScalarShader : public ftShader {
	public:
		ftSvToScalarShader() {
            bInitialized = 1;
			if (ofIsGLProgrammableRenderer()) { glFour(); } else { glTwo(); }
			string shaderName = "ftSvToScalarShader";
			if (bInitialized) { ofLogVerbose(shaderName + " initialized"); }
			else { ofLogWarning(shaderName + " failed to initialize"); }
		}
		
	protected:
		void glTwo() {
			fragmentShader = GLSL120(
									 uniform sampler2DRect FloatTexture;
									 uniform float Scale;
									 void main(){
										 vec4 velocity = texture2DRect(FloatTexture, gl_TexCoord[0].st);
										 
										 vec3 cLeft = vec3(velocity.x) * vec3(0.75, 0.00, 0.00); // red
										 vec3 cDown = vec3(velocity.y) * vec3(0.00, 0.09, 0.75); // blue
										 vec3 cRight= vec3(velocity.z) * vec3(0.00, 0.75, 0.17); // green
										 vec3 cUp	= vec3(velocity.w) * vec3(0.75, 0.59, 0.00); // ocre
										 
										 vec3 color = cLeft + cDown + cRight + cUp;
										 color *= vec3(Scale);
										 
										 gl_FragColor = vec4(color, 1.0);
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= linkProgram();
		}
		
		void glFour() {
			fragmentShader = GLSL410(
									 uniform sampler2DRect FloatTexture;
									 uniform float Scale;
									 
									 in vec2 texCoordVarying;
									 out vec4 fragColor;
									 
									 void main(){
										 vec2 st = texCoordVarying;
										 vec4	velocity = texture(FloatTexture, st);
										 
										 vec3 cLeft = vec3(velocity.x) * vec3(0.75, 0.00, 0.00); // red
										 vec3 cDown = vec3(velocity.y) * vec3(0.00, 0.09, 0.75); // blue
										 vec3 cRight= vec3(velocity.z) * vec3(0.00, 0.75, 0.17); // green
										 vec3 cUp	= vec3(velocity.w) * vec3(0.75, 0.59, 0.00); // ocre
										 
										 vec3 color = cLeft + cDown + cRight + cUp;
										 color *= vec3(Scale);
										 
										 fragColor = vec4(color, 1.0);
									 }
									 );
			
			bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= bindDefaults();
			bInitialized *= linkProgram();
		}
		
	public:
		void update(ofFbo& _fbo, ofTexture& _floatTex, float _scale){
			_fbo.begin();
			begin();
			setUniformTexture("FloatTexture", _floatTex, 0);
			setUniform1f("Scale", _scale);
			renderFrame(_fbo.getWidth(), _fbo.getHeight());
			end();
			_fbo.end();
		}
	};
}

