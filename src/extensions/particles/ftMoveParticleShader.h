
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {

class ftMoveParticleShader : public ftShader {
public:
  ftMoveParticleShader() {
    bInitialized = 1;
    if (ofIsGLProgrammableRenderer()) { glFour(); } else { glTwo(); }
    string shaderName = "ftMoveParticleShader";
    if (bInitialized) { ofLogVerbose(shaderName + " initialized"); }
    else { ofLogWarning(shaderName + " failed to initialize"); }
  }

protected:
  void glTwo() {
    fragmentShader = GLSL120(
    uniform sampler2DRect Backbuffer;
    uniform sampler2DRect ALMSTexture;
    uniform sampler2DRect Velocity;
    uniform sampler2DRect HomeTexture;
    uniform float TimeStep;
    uniform float InverseCellSize;
    uniform vec2  Scale;
    uniform vec2  Dimensions;
    uniform vec2  Gravity;

    void main(){
      vec2 st = gl_TexCoord[0].st;
      vec2 particlePos = texture2DRect(Backbuffer, st).xy;

      vec4 alms = texture2DRect(ALMSTexture, st);
      float age = alms.x;
      float life = alms.y;
      float mass = alms.z;

      if (age > 0.0) {
        vec2 st2 = particlePos * Scale;
        vec2 u = texture2DRect(Velocity, st2).rg / Scale;
        vec2 coord =  TimeStep * InverseCellSize * u;

        particlePos += coord * (1.0 / mass) + Gravity;
      }
      else {
        particlePos = texture2DRect(HomeTexture, st).xy;
      }


      gl_FragColor = vec4(particlePos, 0.0, 1.0); ;
    }
    );

    bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
    bInitialized *= linkProgram();
  }

  void glFour() {
    fragmentShader = GLSL410(
    uniform sampler2DRect Backbuffer;
    uniform sampler2DRect ALMSTexture;
    uniform sampler2DRect Velocity;
    uniform sampler2DRect HomeTexture;
    uniform float TimeStep;
    uniform float InverseCellSize;
    uniform vec2  Scale;
    uniform vec2  Dimensions;
    uniform vec2  Gravity;

    in vec2 texCoordVarying;
    out vec4 fragColor;

    void main(){
      vec2 st = texCoordVarying;
      vec2 particlePos = texture(Backbuffer, st).xy;

      vec4 alms = texture(ALMSTexture, st);
      float age = alms.x;
      float life = alms.y;
      float mass = alms.z;

      if (age > 0.0) {
        vec2 st2 = particlePos * Scale;
        vec2 u = texture(Velocity, st2).rg / Scale;
        vec2 coord =  TimeStep * InverseCellSize * u;

        particlePos += coord * (1.0 / mass) + Gravity;
      }
      else {
        particlePos = texture(HomeTexture, st).xy;
      }


      fragColor = vec4(particlePos, 0.0, 1.0); ;
    }
    );

    bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
    bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
    bInitialized *= bindDefaults();
    bInitialized *= linkProgram();
  }

public:
  void update(ofFbo& _fbo, ofTexture& _backTex, ofTexture& _ALMSTex, ofTexture& _velTex, ofTexture& _homeTex, float _timeStep, float _cellSize, glm::vec2 _gravity){
    _fbo.begin();
    begin();
    setUniformTexture("Backbuffer", _backTex, 0);
    setUniformTexture("ALMSTexture", _ALMSTex, 1);
    setUniformTexture("Velocity", _velTex, 2);
    setUniformTexture("HomeTexture", _homeTex, 3);
    setUniform1f("TimeStep", _timeStep);
    setUniform1f("InverseCellSize", 1.0 / _cellSize);
    setUniform2f("Gravity", _gravity.x * _timeStep, _gravity.y * _timeStep);
    setUniform2f("Scale", _velTex.getWidth() / _fbo.getWidth(), _velTex.getHeight()/ _fbo.getHeight());
    setUniform2f("Dimensions", _fbo.getWidth(), _fbo.getHeight());
    renderFrame(_fbo.getWidth(), _fbo.getHeight());
    end();
    _fbo.end();
  }
};
}

