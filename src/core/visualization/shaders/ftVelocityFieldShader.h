
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {

class ftVelocityFieldShader : public ftShader {
public:
  ftVelocityFieldShader() {
    bInitialized = 1;
    if (ofIsGLProgrammableRenderer()) { glFour(); } else { glTwo(); }
    string shaderName = "ftVelocityFieldShader";
    if (bInitialized) { ofLogVerbose(shaderName + " initialized"); }
    else { ofLogWarning(shaderName + " failed to initialize"); }
  }

protected:
  void glTwo() {
    string geometryShader;

    vertexShader = GLSL120(
          void main() {
            gl_Position = gl_Vertex;
            gl_FrontColor = gl_Color;
          }
          );

    geometryShader = GLSL120GEO(
    uniform sampler2DRect fieldTexture;
    uniform vec2 texResolution;
    uniform vec4 baseColor;
    uniform float arrowSize;

    void main(){
      vec4 lineStart = gl_PositionIn[0];
      vec2 uv = lineStart.xy * texResolution;
      vec2 line = texture2DRect(fieldTexture, uv).xy * arrowSize;
      vec4 lineEnd = lineStart + vec4(line, 0.0, 0.0);

      float alpha = 0.3 + 0.3 * (1.0 - length(line) / arrowSize);
      vec4 color = baseColor;
      color.w = 1.0;//*= alpha;

      float arrowLength = 0.75 * length(line);

      vec2 nLine = normalize(line);
      float arrowAngleA = atan(nLine.y, nLine.x) + 0.1;
      float arrowAngleB = atan(nLine.y, nLine.x) - 0.1;

      vec4 arrowLineA = vec4(cos(arrowAngleA) ,sin(arrowAngleA), 0., 0.);
      vec4 arrowLineB = vec4(cos(arrowAngleB) ,sin(arrowAngleB), 0., 0.);
      arrowLineA = normalize(arrowLineA) * arrowLength;
      arrowLineB = normalize(arrowLineB) * arrowLength;
      vec4 arrowA = lineStart +arrowLineA;
      vec4 arrowB = lineStart +arrowLineB;

      gl_Position = gl_ModelViewProjectionMatrix * lineStart;
      gl_FrontColor = color;
      EmitVertex();

      gl_Position = gl_ModelViewProjectionMatrix * lineEnd;
      gl_FrontColor = color;
      EmitVertex();

      gl_Position = gl_ModelViewProjectionMatrix * arrowA;
      gl_FrontColor = color;
      EmitVertex();

      gl_Position = gl_ModelViewProjectionMatrix * lineEnd;
      gl_FrontColor = color;
      EmitVertex();

      gl_Position = gl_ModelViewProjectionMatrix * arrowB;
      gl_FrontColor = color;
      EmitVertex();

      EndPrimitive();
    }
    );

    fragmentShader = GLSL120(
          void main() {
            gl_FragColor = gl_Color;
          }
          );

    ofLogVerbose("Maximum number of output vertices support is: " + ofToString(getGeometryMaxOutputCount()));
    setGeometryInputType(GL_POINTS);
    setGeometryOutputType(GL_LINE_STRIP);
    setGeometryOutputCount(5);

    bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
    bInitialized *= setupShaderFromSource(GL_GEOMETRY_SHADER_EXT, geometryShader);
    bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
    bInitialized *= linkProgram();
  }

  void glFour() {
    string geometryShader;

    vertexShader = GLSL410(
    uniform mat4 modelViewProjectionMatrix;
    uniform mat4 textureMatrix;

    in vec4 position;
    in vec2  texcoord;
    in vec4  color;

    out vec2 texCoordVarying;
    out vec4 colorVarying;

    void main()
    {
      colorVarying = color;
      gl_Position = position;
    }

    );

    geometryShader = GLSL410(
    uniform mat4 modelViewProjectionMatrix;
    uniform sampler2DRect fieldTexture;
    uniform vec2 texResolution;
    uniform vec4 baseColor;
    uniform float arrowSize;

    layout (points) in;
    layout (line_strip) out;
    layout (max_vertices=5) out;

    out vec4 colorVarying;

    void main(){
      vec4 lineStart = gl_in[0].gl_Position;
      vec2 uv = lineStart.xy * texResolution;
      vec2 line = texture(fieldTexture, uv).xy * arrowSize;
      vec4 lineEnd = lineStart + vec4(line, 0.0, 0.0);

      float alpha = 0.3 + 0.3 * (1.0 - length(line) / arrowSize);
      vec4 color = baseColor;
      color.w = 1.0;//*= alpha;

      float arrowLength = 0.75 * length(line);

      vec2 nLine = normalize(line);
      float arrowAngleA = atan(nLine.y, nLine.x) + 0.1;
      float arrowAngleB = atan(nLine.y, nLine.x) - 0.1;

      vec4 arrowLineA = vec4(cos(arrowAngleA) ,sin(arrowAngleA), 0., 0.);
      vec4 arrowLineB = vec4(cos(arrowAngleB) ,sin(arrowAngleB), 0., 0.);
      arrowLineA = normalize(arrowLineA) * arrowLength;
      arrowLineB = normalize(arrowLineB) * arrowLength;
      vec4 arrowA = lineStart +arrowLineA;
      vec4 arrowB = lineStart +arrowLineB;

      gl_Position = modelViewProjectionMatrix * lineStart;
      colorVarying = color;
      EmitVertex();

      gl_Position = modelViewProjectionMatrix * lineEnd;
      colorVarying = color;
      EmitVertex();

      gl_Position = modelViewProjectionMatrix * arrowA;
      colorVarying = color;
      EmitVertex();

      gl_Position = modelViewProjectionMatrix * lineEnd;
      colorVarying = color;
      EmitVertex();

      gl_Position =  modelViewProjectionMatrix * arrowB;
      colorVarying = color;
      EmitVertex();

      EndPrimitive();

    }
    );

    fragmentShader = GLSL410(
          in vec4 colorVarying;
        out vec4 fragColor;

    void main()
    {
      fragColor = colorVarying;
    }
    );

    bInitialized *= setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
    bInitialized *= setupShaderFromSource(GL_GEOMETRY_SHADER_EXT, geometryShader);
    bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
    bInitialized *= bindDefaults();
    bInitialized *= linkProgram();
  }

public:
  void update(ofVbo& _fieldVbo, ofTexture& _floatTex, float _arrowSize, ofFloatColor _color = ofFloatColor(1,1,1,1)){
    int width = _floatTex.getWidth();
    int height = _floatTex.getHeight();
    begin();
    setUniformTexture("fieldTexture", _floatTex,0);
    setUniform2f("texResolution", width, height);
    setUniform4f("baseColor", _color.r, _color.g, _color.b, _color.a);
    setUniform1f("arrowSize", _arrowSize);
    _fieldVbo.draw(GL_POINTS, 0, _fieldVbo.getNumVertices());
    end();
  }
};
}

