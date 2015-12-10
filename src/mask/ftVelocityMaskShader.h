
#pragma once

#include "ofMain.h"
#include "ftShader.h"


namespace flowTools {
	
	class ftVelocityMaskShader : public ftShader {
	public:
		ftVelocityMaskShader() {
			bInitialized = 1;
			
			if (ofIsGLProgrammableRenderer())
				glThree();
			else
				glTwo();
			
			if (bInitialized)
				ofLogNotice("ftVelocityMaskShader initialized");
			else
				ofLogWarning("ftVelocityMaskShader failed to initialize");
			}
		
	protected:
		void glTwo() {
			
			fragmentShader = GLSL120(
									 uniform sampler2DRect	colorTex;
									 uniform sampler2DRect	velocityTex;
									 uniform float	force;
									 uniform vec2 colorScale;
									 uniform vec2 velocityScale;
								  
									 void main(){
										 vec2 stc = gl_TexCoord[0].xy * colorScale;
										 vec2 stv = gl_TexCoord[0].xy * velocityScale;
										 
										 vec4 color = texture2DRect(colorTex, stc);
										 vec4 vel = texture2DRect(velocityTex, stv);
										 float alpha = length(vel.xy) * force;
										 color.rgb *= vec3(alpha);
										 color.w = alpha;
									  
										 gl_FragColor = color;
									 }
									 );
			
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.linkProgram();
			
		}
		
		void glThree() {
			
			fragmentShader = GLSL150(
									 uniform sampler2DRect colorTex;
									 uniform sampler2DRect velocityTex;
									 uniform float force;
									 uniform vec2 colorScale;
									 uniform vec2 velocityScale;
								  
									 in vec2 texCoordVarying;
									 out vec4 fragColor;
									
									 void main(){
										 vec2 stc = texCoordVarying * colorScale;
										 vec2 stv = texCoordVarying * velocityScale;
									  
										 vec4 color = texture(colorTex, stc);
										 vec4 vel = texture(velocityTex, stv);
										 float alpha = length(vel.xy) * force;
										 color.rgb *= vec3(alpha);
										 color.w = alpha;
									  
										fragColor = color;
									 }
									 );
			
			bInitialized *= shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
			bInitialized *= shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
			bInitialized *= shader.bindDefaults();
			bInitialized *= shader.linkProgram();
		}
		
	public:
		
		void update(ofFbo& _buffer, ofTexture& _densityTexture, ofTexture& _velocityTexture, float _force){
			_buffer.begin();
			shader.begin();
			shader.setUniformTexture("colorTex", _densityTexture, 0);
			shader.setUniformTexture("velocityTex", _velocityTexture, 1);
			shader.setUniform1f("force", _force);
			shader.setUniform2f("colorScale", _densityTexture.getWidth() / _buffer.getWidth() , _densityTexture.getHeight() / _buffer.getHeight() );
			shader.setUniform2f("velocityScale", _velocityTexture.getWidth() / _buffer.getWidth() , _velocityTexture.getHeight() / _buffer.getHeight());
			renderFrame(_buffer.getWidth(), _buffer.getHeight());
			shader.end();
			_buffer.end();
		}
	};
}