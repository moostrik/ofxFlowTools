
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {

class ftBuoyancyShader : public ftShader {
public:
  ftBuoyancyShader() {
    bInitialized = 1;
    if (ofIsGLProgrammableRenderer()) { glFour(); } else { glTwo(); }
    string shaderName = "ftBuoyancyShader";
    if (bInitialized) { ofLogVerbose(shaderName + " initialized"); }
    else { ofLogWarning(shaderName + " failed to initialize"); }
    //      load("tempShader/ftVertexShader.vert", "tempShader/" + shaderName + ".frag");
  }

protected:
  void glTwo() {
    fragmentShader = GLSL120(
    uniform sampler2DRect tex_velocity;
    uniform sampler2DRect tex_temperature;
    uniform sampler2DRect tex_density;

    uniform float sigma;
    uniform float weight;
    uniform float ambient_temperature;

    void main(){
      vec2 st = gl_TexCoord[0].st;

      vec2  velocity    = texture2DRect(tex_velocity   , st).xy;
      float temperature = texture2DRect(tex_temperature, st).x;

      float dtemp = temperature - ambient_temperature;
      vec2 buoyancy = vec2(0.0);
      if (dtemp != 0.0) {
        float density = texture2DRect(tex_density, st).a;
        float buoyancy_force = dtemp * sigma - density * weight;
        buoyancy = vec2(0, -1) * buoyancy_force;
      }

      gl_FragColor = vec4(buoyancy, 0.0, 0.0);;
    }
    );

    bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
    bInitialized *= linkProgram();
  }

  void glFour() {
    fragmentShader = GLSL410(
    precision mediump float;
    precision mediump int;

    in vec2 texCoordVarying;
    out vec2 glFragColor;

    uniform sampler2DRect tex_velocity;
    uniform sampler2DRect tex_temperature;
    uniform sampler2DRect tex_density;

    uniform float sigma;
    uniform float weight;
    uniform float ambient_temperature;

    void main(){
      vec2 st = texCoordVarying;

      vec2  velocity    = texture(tex_velocity   , st).xy;
      float temperature = texture(tex_temperature, st).x;

      float dtemp = temperature - ambient_temperature;
      vec2 buoyancy = vec2(0.0);
      if (dtemp != 0.0) {
        float density = texture(tex_density, st).a;
        float buoyancy_force = dtemp * sigma - density * weight;
        buoyancy = vec2(0, -1) * buoyancy_force;
      }

      glFragColor = buoyancy;
    }
    );

    bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
    bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
    bInitialized *= bindDefaults();
    bInitialized *= linkProgram();
  }

public:
  void update(ofFbo& _fbo, ofTexture& _velTex, ofTexture& _temTex, ofTexture _denTex, float _sigma, float _weight, float _ambientTemperature){
    _fbo.begin();
    begin();
    setUniform1f    ("sigma",        _sigma);
    setUniform1f    ("weight",        _weight);
    setUniform1f    ("ambient_temperature", _ambientTemperature);
    setUniformTexture  ("tex_velocity",    _velTex,  0);
    setUniformTexture  ("tex_temperature",    _temTex,  1);
    setUniformTexture  ("tex_density",      _denTex,  2);
    renderFrame(_fbo.getWidth(), _fbo.getHeight());
    end();
    _fbo.end();
  }
};
}



