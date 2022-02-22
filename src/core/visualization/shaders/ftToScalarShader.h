
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {

class ftToScalarShader : public ftShader {
public:
  ftToScalarShader() {
    bInitialized = 1;
    if (ofIsGLProgrammableRenderer()) { glFour(); } else { glTwo(); }
    string shaderName = "ftToScalarShader";
    if (bInitialized) { ofLogVerbose(shaderName + " initialized"); }
    else { ofLogWarning(shaderName + " failed to initialize"); }
  }

protected:
  void glTwo() {
    fragmentShader = GLSL120(
    uniform sampler2DRect FloatTexture;
    uniform float Scale;
    void main(){
      vec4  velocity = texture2DRect(FloatTexture, gl_TexCoord[0].st);
      velocity.xyz *= vec3(Scale);
      velocity.w = pow(length(velocity.xyz), 0.33);
      velocity.xyz += vec3(0.5);
      gl_FragColor = velocity;
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
      vec4  velocity = texture(FloatTexture, st);
      velocity.xyz *= vec3(Scale);
      velocity.w = pow(length(velocity.xyz), 0.33);
      velocity.xyz += vec3(0.5);
      fragColor = velocity;
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

