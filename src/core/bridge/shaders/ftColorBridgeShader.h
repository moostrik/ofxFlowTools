
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {

class ftColorBridgeShader : public ftShader {
public:
  ftColorBridgeShader() {
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
    uniform vec2 scale0;
    uniform vec4 color0;
    uniform vec4 color1;
    uniform vec4 color2;
    uniform float speed;

    in vec2 texCoordVarying;
    out vec4 fragColor;

    vec3 hsv2rgb(vec3 c) {
      vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
      vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
      return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
    }

    void main(){
      vec2 st0 = texCoordVarying * scale0;
      vec4 vel = texture(tex0, st0);

      float alpha = length(vel.xy); // magnitude of the velocity
      alpha = min(alpha, 1.0);
      float a0 = clamp(alpha, 0.00, 0.33) * 3.0;
      float a1 = (clamp(alpha, 0.33, 0.66)-0.33) * 3.0;
      float a2 = (clamp(alpha, 0.66, 0.99)-0.66) * 3.0;

      vec4 hsva = vec4(0,0,0,0);
      hsva = mix(hsva, color0, a0);
      hsva = mix(hsva, color1, a1);
      hsva = mix(hsva, color2, a2);

      vec3 rgb = hsv2rgb(hsva.xyz);
      fragColor = vec4(rgb.x, rgb.y, rgb.z, hsva.w);
    }
    );

    bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
    bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
    bInitialized *= bindDefaults();
    bInitialized *= linkProgram();
  }

public:
  void update(ofFbo& _fbo, ofTexture& _velTex, ofFloatColor lowColor, ofFloatColor midColor, ofFloatColor highColor, float _speed){
    glm::vec4 lowHsba;
    lowHsba.w = lowColor.a;
    lowColor.getHsb(lowHsba.x, lowHsba.y, lowHsba.z);
    glm::vec4 midHsba;
    midHsba.w = midColor.a;
    midColor.getHsb(midHsba.x, midHsba.y, midHsba.z);
    glm::vec4 highHsba;
    highHsba.w = highColor.a;
    highColor.getHsb(highHsba.x, highHsba.y, highHsba.z);

    _fbo.begin();
    begin();
    setUniformTexture("tex0", _velTex, 0);
    setUniform2f("scale0", _velTex.getWidth() / _fbo.getWidth() , _velTex.getHeight() / _fbo.getHeight());
    setUniform4f("color0", lowHsba);
    setUniform4f("color1", midHsba);
    setUniform4f("color2", highHsba);
    setUniform1f("speed", _speed);
    renderFrame(_fbo.getWidth(), _fbo.getHeight());
    end();
    _fbo.end();
  }
};
}



