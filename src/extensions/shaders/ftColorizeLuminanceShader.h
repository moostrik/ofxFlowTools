
#pragma once

#include "ofMain.h"
#include "ftShader.h"

using namespace flowTools;

class ftColorizeLuminanceShader : public ftShader {
public:
	ftColorizeLuminanceShader() {
		
		ofLogVerbose("init ftColorizeLuminanceShader
					 ");
		if (isProgrammableRenderer)
			glThree();
		else
			glTwo();
	}
	
protected:
	void glTwo() {
		fragmentShader = GLSL120(
								 uniform vec4	restColor;
								 
								 void main(){
									 gl_FragColor = restColor;
									 
								 }
								 
								 );
		
		shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
		shader.linkProgram();
		
	}
	
	void glThree() {
		
		fragmentShader = GLSL150(
								 uniform sampler2DRect SourceTexture;
								 uniform sampler2DRect RampTexture;
								 uniform vec2	TextureScale;
								 uniform float	RampSize;
								 
								 in vec2 texCoordVarying;
								 out vec4 fragColor;
								 
								 void main(){
									 vec2 st = texCoordVarying;
									 
									 vec2 stTexture = st * TextureScale;
									 vec4 srcColor = texture(SourceTexture, stTexture);
									 
									 vec2 stRamp = vec2(srcColor.x * RampSize, 0.5);
									 vec4 rampColor = texture(RampTexture, stRamp);
									 
									 fragColor = rampColor;
								 }
								 );
		
		shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
		shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
		shader.bindDefaults();
		shader.linkProgram();
	}
	
public:
	
	void update(ofFbo& _buffer, ofTexture _srcTexture, ofTexture _rampTexture){
		
		_buffer.begin();
		shader.begin();
		shader.setUniformTexture("SourceTexture", _srcTexture, 0);
		shader.setUniformTexture("RampTexture", _rampTexture, 1);
		shader.setUniform2f("TextureScale", _srcTexture.getWidth() / _buffer.getWidth(), _srcTexture.getHeight()/ _buffer.getHeight());
		shader.setUniform1f("RampSize", _rampTexture.getWidth());
		renderFrame(_buffer.getWidth(), _buffer.getHeight());
		shader.end();
		_buffer.end();
	}
};
