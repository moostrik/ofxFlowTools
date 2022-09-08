
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {

class ftOpticalFlowShader : public ftShader {
public:
  ftOpticalFlowShader() {
    bInitialized = 1;
    if (ofIsGLProgrammableRenderer()) { glFour(); } else { glTwo(); }
    string shaderName = "ftOpticalFlowShader";
    if (bInitialized) { ofLogVerbose(shaderName + " initialized"); }
    else { ofLogWarning(shaderName + " failed to initialize"); }
  }

protected:
  void glTwo() {
    fragmentShader = GLSL120(
    uniform sampler2DRect tex0;
    uniform sampler2DRect tex1;
    uniform float         offset;
    uniform float         threshold;
    uniform vec2          force;
    uniform float         power;

    void main(){
      vec2 st = gl_TexCoord[0].st;
      vec2 off_x = vec2(offset, 0.0);
      vec2 off_y = vec2(0.0, offset);

      //get the difference
      float scr_dif = texture2DRect(tex0, st).x - texture2DRect(tex1, st).x;

      //calculate the gradient
      float gradx; float grady; float gradmag;
      gradx =  texture2DRect(tex1, st + off_x).x - texture2DRect(tex1, st - off_x).x;
      gradx += texture2DRect(tex0, st + off_x).x - texture2DRect(tex0, st - off_x).x;
      grady =  texture2DRect(tex1, st + off_y).x - texture2DRect(tex1, st - off_y).x;
      grady += texture2DRect(tex0, st + off_y).x - texture2DRect(tex0, st - off_y).x;
      gradmag = sqrt((gradx*gradx)+(grady*grady)+TINY);

      vec2 flow;
      flow.x = scr_dif*(gradx/gradmag);
      flow.y = scr_dif*(grady/gradmag);

      flow *= 3.0; // weird normalisation

      float magnitude = length(flow);
      magnitude = max(magnitude, threshold);
      magnitude -= threshold;
      magnitude /= (1-threshold);
      magnitude = pow(magnitude, power);
      flow += TINY; // flow length cannot be 0 for normalization to work on windows
      flow = normalize(flow) * vec2(min(max(magnitude, 0), 1));

      flow *= force;

      // set color
      gl_FragColor = vec4(flow, 0.0, 1.0);
    }
    );

    bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
    bInitialized *= linkProgram();
  }

  void glFour() {
    fragmentShader = GLSL410(
    uniform sampler2DRect tex0;
    uniform sampler2DRect tex1;
    uniform float         offset;
    uniform float         threshold;
    uniform vec2          force;
    uniform float         power;

    in vec2 texCoordVarying;
    out vec4 fragColor;

    void main()
    {
      vec2 st = texCoordVarying;
      vec2 off_x = vec2(offset, 0.0);
      vec2 off_y = vec2(0.0, offset);

      //get the difference
      float scr_dif = texture(tex0, st).x - texture(tex1, st).x;

      //calculate the gradient
      float gradx; float grady; float gradmag; float lambda = 0.01;
      gradx =  texture(tex1, st + off_x).x - texture(tex1, st - off_x).x;
      gradx += texture(tex0, st + off_x).x - texture(tex0, st - off_x).x;
      grady =  texture(tex1, st + off_y).x - texture(tex1, st - off_y).x;
      grady += texture(tex0, st + off_y).x - texture(tex0, st - off_y).x;
      gradmag = sqrt((gradx*gradx)+(grady*grady)+TINY);

      vec2 flow;
      flow.x = scr_dif*(gradx/gradmag);
      flow.y = scr_dif*(grady/gradmag);

      flow *= 3.0; // weird normalisation

      float magnitude = length(flow);
      magnitude = max(magnitude, threshold);
      magnitude -= threshold;
      magnitude /= (1-threshold);
      magnitude = pow(magnitude, power);

      flow += TINY; // flow length cannot be 0 for normalization to work on windows
      flow = normalize(flow) * vec2(min(max(magnitude, 0), 1));

      flow *= force;

      // set color
      fragColor = vec4(flow, 0.0, 1.0);
    }
    );

    bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
    bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
    bInitialized *= bindDefaults();
    bInitialized *= linkProgram();
  }

public:
  void update(ofFbo& _fbo, ofTexture& _currTex, ofTexture& _prevTex, float _offset = 3.0, float _threshold = 0.04, glm::vec2 _force = glm::vec2(1.0, 1.0), float _power = 1.0, bool _inverseX = 0, bool _inverseY = 0){

    _fbo.begin();
    begin();
    setUniformTexture("tex0", _currTex, 0);
    setUniformTexture("tex1", _prevTex, 1);
    setUniform1f("offset", _offset);
    setUniform1f("threshold", _threshold);
    setUniform2f("force", _force * glm::vec2(_inverseX? -1 : 1, _inverseY? -1 : 1));
    setUniform1f("power", _power);
    renderFrame(_fbo.getWidth(), _fbo.getHeight());
    end();
    _fbo.end();
  }
};
}

