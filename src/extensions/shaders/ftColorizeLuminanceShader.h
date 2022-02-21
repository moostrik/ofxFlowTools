
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {

class ftColorizeLuminanceShader : public ftShader {
public:
  ftColorizeLuminanceShader() {
    bInitialized = 1;
    if (ofIsGLProgrammableRenderer()) { glFour(); } else { glTwo(); }
    string shaderName = "ftColorizeLuminanceShader";
    if (bInitialized) { ofLogVerbose(shaderName + " initialized"); }
    else { ofLogWarning(shaderName + " failed to initialize"); }
  }

protected:
  void glTwo() {
    fragmentShader = GLSL120(
    uniform vec4  restColor;

    void main(){
      gl_FragColor = restColor;
    }
    );

    setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
    linkProgram();

  }

  void glFour() {

    fragmentShader = GLSL410(
    uniform sampler2DRect SourceTexture;
    uniform vec2  TextureScale;
    uniform vec4  lowColor;
    uniform vec4  highColor;

    in vec2 texCoordVarying;
    out vec4 fragColor;

    void main(){
      vec2 st = texCoordVarying;

      vec2 stTexture = st * TextureScale;
      float l = texture(SourceTexture, stTexture).x;

      fragColor = mix(lowColor, highColor, l);
    }
    );

    setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
    setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
    bindDefaults();
    linkProgram();
  }

public:

  void update(ofFbo& _fbo, ofTexture _srcTex, ofFloatColor _color) {
    update(_fbo, _srcTex, _color, _color);
  }

  void update(ofFbo& _fbo, ofTexture _srcTex, ofFloatColor _lowColor, ofFloatColor _highColor){
    _fbo.begin();
    begin();
    setUniformTexture("SourceTexture", _srcTex, 0);
    setUniform2f("TextureScale", _srcTex.getWidth() / _fbo.getWidth(), _srcTex.getHeight()/ _fbo.getHeight());
    setUniform4f("lowColor", glm::vec4(_lowColor.r, _lowColor.g, _lowColor.b, _lowColor.a));
    setUniform4f("highColor", glm::vec4(_highColor.r, _highColor.g, _highColor.b, _highColor.a));
    renderFrame(_fbo.getWidth(), _fbo.getHeight());
    end();
    _fbo.end();
  }
};
}


