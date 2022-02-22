
#pragma once
#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {

class ftInverseShader : public ftShader {
public:
  ftInverseShader(){
    bInitialized = 1;
    if (ofIsGLProgrammableRenderer()) { glFour(); } else { glTwo(); }
    string shaderName = "ftInverseShader";
    if (bInitialized) { ofLogVerbose(shaderName + " initialized"); }
    else { ofLogWarning(shaderName + " failed to initialize"); }
  }

protected:
  void glTwo() {
    fragmentShader = GLSL120(
    uniform sampler2DRect tex0;

    void main(){
      vec4 color = texture2DRect(tex0,gl_TexCoord[0].st);
      color.xyz = vec3(1.0) - color.xyz;
      gl_FragColor = color;
    }
    );

  bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
  bInitialized *= linkProgram();

}

void glFour() {

  fragmentShader = GLSL410(
  uniform sampler2DRect tex0;

  in vec2 texCoordVarying;
  out vec4 fragColor;

  void main(){
    vec4 color = texture(tex0, texCoordVarying);
    color.xyz = vec3(1.0) - color.xyz;
    fragColor = color;
  }
  );

  bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
  bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
  bInitialized *= bindDefaults();
  bInitialized *= linkProgram();

}

public:
void update(ofFbo& _drawBuffer, ofTexture& _srcTexture){
  _drawBuffer.begin();
  begin();
  setUniformTexture( "tex0" , _srcTexture, 0 );
  renderFrame(_drawBuffer.getWidth(), _drawBuffer.getHeight());
  end();
  _drawBuffer.end();
}
};
}
