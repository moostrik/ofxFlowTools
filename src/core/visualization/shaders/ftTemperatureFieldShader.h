
#pragma once

#include "ofMain.h"
#include "ftShader.h"

namespace flowTools {

class ftTemperatureFieldShader : public ftShader {
public:
  ftTemperatureFieldShader() {
    bInitialized = 1;
    shaderName = "ftTemperatureFieldShader";
    if (loadFromFile()) {
      ofLogNotice("ftShader") << "loaded " << shaderName << " from file";
    } else glFour();
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
    uniform sampler2DRect prsTexture;
    uniform vec2 texResolution;
    uniform vec2 prsScale;

    layout (points) in;
    layout (triangle_strip) out;
    layout (max_vertices=3) out;

    out vec4 colorVarying;

    void main(){
      vec4 centre = gl_in[0].gl_Position;
      vec2 uv = centre.xy * texResolution;
      float prs = texture(prsTexture, uv).x;
      float prsLineLength = prs * -prsScale.y;
      float prsLineWidth = prsScale.x;
      vec4 lineEnd = centre + vec4(0.0, prsLineLength, 0.0, 0.0);

      vec4 color = vec4(0.0, 0.0, 0.0, 1.0);
      color.r = (prs > 0)? 1.0 : (length(prs) > 1.0)? 0.0: length(prs);
      color.g = (prs < 0)? 1.0 : (length(prs) > 1.0)? 0.0: length(prs);
      color.b = color.g;

      gl_Position = modelViewProjectionMatrix * (centre + vec4(-0.5 * prsLineWidth, 0, 0, 0));
      colorVarying = color;
      EmitVertex();

      gl_Position = modelViewProjectionMatrix * lineEnd;
      colorVarying = color;
      EmitVertex();

      gl_Position = modelViewProjectionMatrix * (centre + vec4(0.5 * prsLineWidth, 0, 0, 0));;
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
    bInitialized *= setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
    bInitialized *= setupShaderFromSource(GL_GEOMETRY_SHADER_EXT, geometryShader);
    bInitialized *= bindDefaults();
    bInitialized *= linkProgram();
  }

public:
  void update(ofVbo& fieldVbo, ofTexture& prsTex, glm::vec2 scale){
    int width = prsTex.getWidth();
    int height = prsTex.getHeight();

    begin();
    setUniformTexture("prsTexture", prsTex,0);
    setUniform2f("texResolution", width, height);
    setUniform2f("prsScale", scale);
    fieldVbo.draw(GL_POINTS, 0, fieldVbo.getNumVertices());
    end();

    glFlush();

  }
};
}
