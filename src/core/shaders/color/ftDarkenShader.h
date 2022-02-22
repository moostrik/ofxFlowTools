
#pragma once
#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {

class ftDarkenShader : public ftShader {
public:
  ftDarkenShader(){
    bInitialized = 1;
    if (ofIsGLProgrammableRenderer()) { glFour(); } else { glTwo(); }
    string shaderName = "ftDarkenShader";
    if (bInitialized) { ofLogVerbose(shaderName + " initialized"); }
    else { ofLogWarning(shaderName + " failed to initialize"); }
  }

protected:
  void glTwo() {
    fragmentShader = GLSL120(
    uniform sampler2DRect tex0;
    uniform float treshold;

    void main(){
      vec4 color = texture2DRect(tex0,gl_TexCoord[0].st);
      float invT = 1.0 - treshold;
      color.x = max(0, color.x - treshold) / invT;
      color.y = max(0, color.y - treshold) / invT;
      color.z = max(0, color.z - treshold) / invT;
      gl_FragColor = color;
    }
    );

    bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
    bInitialized *= linkProgram();

  }

  void glFour() {

    fragmentShader = GLSL410(
    uniform sampler2DRect tex0;
    uniform float treshold;

    in vec2 texCoordVarying;
    out vec4 fragColor;

    void main(){
      vec4 color = texture(tex0, texCoordVarying);
      float invT = 1.0 - treshold;
      color.x = max(0, color.x - treshold) / invT;
      color.y = max(0, color.y - treshold) / invT;
      color.z = max(0, color.z - treshold) / invT;
      fragColor = color;
    }
    );

    bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
    bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
    bInitialized *= bindDefaults();
    bInitialized *= linkProgram();

  }

public:
  void update(ofFbo& _drawBuffer, ofTexture& _srcTexture, float _treshold){
    _drawBuffer.begin();
    begin();
    setUniformTexture( "tex0" , _srcTexture, 0 );
    setUniform1f("treshold", _treshold);
    renderFrame(_drawBuffer.getWidth(), _drawBuffer.getHeight());
    end();
    _drawBuffer.end();
  }
};
}
