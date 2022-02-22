
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {

class ftGradientShader : public ftShader {
public:
  ftGradientShader() {
    bInitialized = 1;
    if (ofIsGLProgrammableRenderer()) { glFour(); } else { glTwo(); }
    string shaderName = "ftGradientShader";
    if (bInitialized) { ofLogVerbose(shaderName + " initialized"); }
    else { ofLogWarning(shaderName + " failed to initialize"); }
    //      load("tempShader/ftVertexShader.vert", "tempShader/" + shaderName + ".frag");
  }

protected:
  void glTwo() {
    fragmentShader = GLSL120(
    uniform sampler2DRect tex_velocity;
    uniform sampler2DRect tex_pressure;
    uniform sampler2DRect tex_obstacle;
    uniform sampler2DRect tex_obstacleOffset;

    uniform float halfrdx;

    void main(){
      vec2 st = gl_TexCoord[0].st;

      float oC = texture2DRect(tex_obstacle, st).x;
      if (oC == 1.0) {
        gl_FragColor = vec4(0.0);
        return;
      }

      // pressure
      float pT = texture2DRect(tex_pressure, st + ivec2(0,1)).x;
      float pB = texture2DRect(tex_pressure, st - ivec2(0,1)).x;
      float pR = texture2DRect(tex_pressure, st + ivec2(1,0)).x;
      float pL = texture2DRect(tex_pressure, st - ivec2(1,0)).x;
      float pC = texture2DRect(tex_pressure, st).x;

      // pure Neumann pressure boundary
      // use center pressure if neighbor is an obstacle
      vec4 oN = texture2DRect(tex_obstacleOffset, st);
      pT = mix(pT, pC, oN.x);
      pB = mix(pB, pC, oN.y);
      pR = mix(pR, pC, oN.z);
      pL = mix(pL, pC, oN.w);
      //
      // gradient subtract
      vec2 grad = vec2(halfrdx) * vec2(pR - pL, pT - pB);
      vec2 vOld = texture2DRect(tex_velocity, st).xy;

      gl_FragColor = vec4(vOld - grad, 0.0, 0.0);
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
    uniform sampler2DRect tex_pressure;
    uniform sampler2DRect tex_obstacle;
    uniform sampler2DRect tex_obstacleOffset;

    uniform float halfrdx;

    void main(){

      vec2 st = texCoordVarying;

      float oC = texture(tex_obstacle, st).x;
      if (oC == 1.0) {
        glFragColor = vec2(0.0);
        return;
      }

      // pressure
      float pT = textureOffset(tex_pressure, st, + ivec2(0,1)).x;
      float pB = textureOffset(tex_pressure, st, - ivec2(0,1)).x;
      float pR = textureOffset(tex_pressure, st, + ivec2(1,0)).x;
      float pL = textureOffset(tex_pressure, st, - ivec2(1,0)).x;
      float pC = texture      (tex_pressure, st).x;

      // pure Neumann pressure boundary
      // use center pressure if neighbor is an obstacle
      vec4 oN = texture(tex_obstacleOffset, st);
      pT = mix(pT, pC, oN.x);
      pB = mix(pB, pC, oN.y);
      pR = mix(pR, pC, oN.z);
      pL = mix(pL, pC, oN.w);

      // gradient subtract
      vec2 grad = halfrdx * vec2(pR - pL, pT - pB);
      vec2 vOld = texture(tex_velocity, st).xy;

      glFragColor = vOld - grad;
    }
    );

    bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
    bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
    bInitialized *= bindDefaults();
    bInitialized *= linkProgram();
  }

public:
  void update(ofFbo& _fbo, ofTexture& _backTex, ofTexture& _prsTex, ofTexture& _obsCTex, ofTexture& _obsNTex, float _gridScale){
    _fbo.begin();
    begin();
    setUniform1f    ("halfrdx",      1.0f / _gridScale);
    setUniformTexture  ("tex_velocity",  _backTex,  0);
    setUniformTexture  ("tex_pressure",  _prsTex,  1);
    setUniformTexture  ("tex_obstacle",  _obsCTex,  2);
    setUniformTexture  ("tex_obstacleOffset",  _obsNTex,  3);
    renderFrame(_fbo.getWidth(), _fbo.getHeight());
    end();
    _fbo.end();
  }
};
}


