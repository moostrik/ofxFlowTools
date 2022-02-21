
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {

class ftSplitVelocityShader : public ftShader {
public:
  ftSplitVelocityShader() {
    bInitialized = 1;
    if (ofIsGLProgrammableRenderer()) { glFour(); } else { glTwo(); }
    string shaderName = "ftSplitVelocityShader";
    if (bInitialized) { ofLogVerbose(shaderName + " initialized"); }
    else { ofLogWarning(shaderName + " failed to initialize"); }
  }

protected:
  void glTwo() {
    fragmentShader = GLSL120(
          uniform sampler2DRect VelocityTexture;
        uniform float Force;
    uniform vec2  Scale;

    void main(){
      vec2 st = gl_TexCoord[0].st;
      vec2 st2 = st * Scale;

      vec2 velocity = texture2DRect(VelocityTexture, st2).xy * Force;
      vec2 pVel = max(velocity, vec2(0,0));
      vec2 nVel = abs(min(velocity, vec2(0,0)));

      gl_FragColor = vec4(pVel, nVel);
    }
    );

    bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
    bInitialized *= linkProgram();
  }

  void glFour() {
    fragmentShader = GLSL410(
          uniform sampler2DRect VelocityTexture;
        uniform float Force;
    uniform vec2  Scale;

    in vec2 texCoordVarying;
    out vec4 fragColor;

    void main(){
      vec2 st = texCoordVarying;
      vec2 st2 = st * Scale;

      vec2 velocity = texture(VelocityTexture, st2).xy * Force;
      vec2 pVel = max(velocity, vec2(0,0));
      vec2 nVel = abs(min(velocity, vec2(0,0)));
      fragColor = vec4(pVel, nVel);
    }
    );

    bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
    bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
    bInitialized *= bindDefaults();
    bInitialized *= linkProgram();
  }

public:
  void update(ofFbo& _fbo, ofTexture& _velTex, float _force){
    _fbo.begin();
    begin();
    setUniformTexture("VelocityTexture", _velTex, 0);
    setUniform1f("Force", _force);
    setUniform2f("Scale", _velTex.getWidth() / _fbo.getWidth(), _velTex.getHeight()/ _fbo.getHeight());
    renderFrame(_fbo.getWidth(), _fbo.getHeight());
    end();
    _fbo.end();
  }
};
}

