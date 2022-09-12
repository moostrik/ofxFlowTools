
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {

class ftVelocityFieldShader : public ftShader {
public:
  ftVelocityFieldShader() {
    bInitialized = 1;
    glFour();
    string shaderName = "ftVelocityFieldShader";
    if (bInitialized) { ofLogVerbose(shaderName + " initialized"); }
    else { ofLogWarning(shaderName + " failed to initialize"); }
  }

protected:

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
    uniform sampler2DRect velTexture;
    uniform vec2 texResolution;
    uniform vec2 velScale;

    layout (points) in;
    layout (line_strip) out;
    layout (max_vertices=5) out;

    out vec4 colorVarying;

    void main(){
      vec4 lineStart = gl_in[0].gl_Position;
      vec2 uv = lineStart.xy * texResolution;
      vec2 velocity = texture(velTexture, uv).xy;
      vec2 line = velocity * velScale;
      vec4 lineEnd = lineStart + vec4(line, 0.0, 0.0);

      vec4 color = vec4(0.0, 0.0, 0.0, 1.0);
      color.r = min(length(velocity), 1.0);
      color.g = (length(velocity) > 1.0)? 0.0 : 1.0;
      color.b = (length(velocity) > 1.0)? 0.0 : color.r ;

      float pointerLength = 0.75 * length(line);
      vec2 nLine = normalize(line);
      float pointerAngleA = atan(nLine.y, nLine.x) + 0.1;
      float pointerAngleB = atan(nLine.y, nLine.x) - 0.1;

      vec4 pointerLineA = vec4(cos(pointerAngleA) ,sin(pointerAngleA), 0., 0.);
      vec4 pointerLineB = vec4(cos(pointerAngleB) ,sin(pointerAngleB), 0., 0.);
      pointerLineA = normalize(pointerLineA) * pointerLength;
      pointerLineB = normalize(pointerLineB) * pointerLength;
      vec4 pointerA = lineStart +pointerLineA;
      vec4 pointerB = lineStart +pointerLineB;

      gl_Position = modelViewProjectionMatrix * pointerA;
      colorVarying = color;
      EmitVertex();

      gl_Position = modelViewProjectionMatrix * lineEnd;
      colorVarying = color;
      EmitVertex();

      gl_Position = modelViewProjectionMatrix * lineStart;
      colorVarying = color;
      EmitVertex();

      gl_Position = modelViewProjectionMatrix * lineEnd;
      colorVarying = color;
      EmitVertex();

      gl_Position =  modelViewProjectionMatrix * pointerB;
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
  void update(ofVbo& fieldVbo, ofTexture& velTex, glm::vec2 scale, ofFloatColor color = ofFloatColor(1,1,1,1)){
    int width = velTex.getWidth();
    int height = velTex.getHeight();
    begin();
    setUniformTexture("velTexture", velTex,0);
    setUniform2f("texResolution", width, height);
    setUniform2f("velScale", scale);
    fieldVbo.draw(GL_POINTS, 0, fieldVbo.getNumVertices());
    end();
  }
};
}

