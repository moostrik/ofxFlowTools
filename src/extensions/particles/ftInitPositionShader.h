
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {

class ftInitPositionShader : public ftShader {
public:
  ftInitPositionShader() {
    bInitialized = 1;
    if (ofIsGLProgrammableRenderer()) { glFour(); } else { glTwo(); }
    string shaderName = "ftInitPositionShader";
    if (bInitialized) { ofLogVerbose(shaderName + " initialized"); }
    else { ofLogWarning(shaderName + " failed to initialize"); }
  }

protected:
  void glTwo() {
    fragmentShader = GLSL120(
    uniform vec2 Dimensions;
    void main(){
      vec2 st = gl_TexCoord[0].st;
      gl_FragColor = vec4(st, 0.0, 1.0);
    }
    );

    bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
    bInitialized *= linkProgram();
  }

  void glFour() {
    fragmentShader = GLSL410(
    uniform vec2 Dimensions;
    in vec2 texCoordVarying;
    out vec4 fragColor;

    void main(){
      vec2 st = texCoordVarying;

      fragColor = vec4(st, 0.0, 1.0);
    }
    );

    bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
    bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
    bInitialized *= bindDefaults();
    bInitialized *= linkProgram();
  }

public:
  void update(ofFbo& _fbo){
    _fbo.begin();
    begin();
    setUniform2f("Dimensions", _fbo.getWidth(), _fbo.getHeight());
    renderFrame(_fbo.getWidth(), _fbo.getHeight());
    end();
    _fbo.end();
  }
};
}

