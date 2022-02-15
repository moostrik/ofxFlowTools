
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {

class ftDensityBridgeShader : public ftShader {
public:
  ftDensityBridgeShader() {\
    bInitialized = 1;
    if (ofIsGLProgrammableRenderer()) { glFour(); } else { glTwo(); }
    string shaderName = "ftDensityBridgeShader";
    if (bInitialized) { ofLogVerbose(shaderName + " initialized"); }
    else { ofLogWarning(shaderName + " failed to initialize"); }
  }

protected:
  void glTwo() {
    fragmentShader = GLSL120(
    uniform sampler2DRect	tex0;
    uniform sampler2DRect	tex1;
    uniform vec2 scale0;
    uniform vec2 scale1;
    uniform float speed;

    void main(){
      vec2 st0 = gl_TexCoord[0].xy * scale0;
      vec2 st1 = gl_TexCoord[0].xy * scale1;

      vec4 den = texture2DRect(tex0, st0);
      vec4 vel = texture2DRect(tex1, st1);

      float alpha = length(vel.xy); // magnitude of the velocity
      den.w = alpha * speed;
      den.xyz *= den.w;

      gl_FragColor = den;
    }
    );

    bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
    bInitialized *= linkProgram();
  }

  void glFour() {
    fragmentShader = GLSL410(

          uniform sampler2DRect tex0;
        uniform sampler2DRect tex1;
    uniform vec2 scale0;
    uniform vec2 scale1;
    uniform float speed;

    in vec2 texCoordVarying;
    out vec4 fragColor;

    void main(){
      vec2 st0 = texCoordVarying * scale0;
      vec2 st1 = texCoordVarying * scale1;

      vec4 den = texture(tex0, st0);
      vec4 vel = texture(tex1, st1);

      float alpha = length(vel.xy); // magnitude of the velocity
      den.w = alpha * speed;
      den.xyz *= den.w;

      fragColor = den;
    }
    );

    bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
    bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
    bInitialized *= bindDefaults();
    bInitialized *= linkProgram();
  }

public:
  void update(ofFbo& _fbo, ofTexture& _denTex, ofTexture& _velTex, float _speed){
    _fbo.begin();
    begin();
    setUniformTexture("tex0", _denTex, 0);
    setUniformTexture("tex1", _velTex, 1);
    setUniform2f("scale0", _denTex.getWidth() / _fbo.getWidth() , _denTex.getHeight() / _fbo.getHeight() );
    setUniform2f("scale1", _velTex.getWidth() / _fbo.getWidth() , _velTex.getHeight() / _fbo.getHeight());
    setUniform1f("speed", _speed);
    renderFrame(_fbo.getWidth(), _fbo.getHeight());
    end();
    _fbo.end();
  }
};
}



