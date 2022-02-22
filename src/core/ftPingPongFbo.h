
#pragma once

#include "ofMain.h"

namespace flowTools {

class ftPingPongFbo {
public:

  void allocate(int width, int height, int internalformat = GL_RGBA, int numSamples = 0) {
    ofFboSettings settings;
    settings.numColorbuffers = 1;   // one buff per fbo
    settings.width            = width;
    settings.height            = height;
    settings.internalformat    = internalformat;
    settings.numSamples        = numSamples;
#ifdef TARGET_OPENGLES
    settings.useDepth        = false;
    settings.useStencil        = false;
    settings.textureTarget    = GL_TEXTURE_2D;
#else
    settings.useDepth        = true;
    settings.useStencil        = true;
    settings.textureTarget    = ofGetUsingArbTex() ? GL_TEXTURE_RECTANGLE_ARB : GL_TEXTURE_2D;
#endif
    allocate(settings);
  }

  void allocate(ofFboSettings _settings){
    fbos[0].allocate(_settings);
    fbos[1].allocate(_settings);
    flag = false;
  }

  void swap(){
    flag = !flag;
  }

  ofFbo& get() { return fbos[flag]; }
  ofFbo& getBack() { return fbos[!flag]; }

  ofTexture& getTexture() { return fbos[flag].getTexture(); }
  ofTexture& getBackTexture() { return fbos[!flag].getTexture(); }

  void bind() { fbos[flag].bind(); }
  void unbind() { fbos[flag].unbind(); }
  void begin() { fbos[flag].begin(); }
  void end() { fbos[flag].end(); }

  float getWidth() { return fbos[flag].getWidth(); }
  float getHeight() { return fbos[flag].getHeight(); }

  void draw(float x, float y, float w, float h) const     { fbos[flag].draw(x,y,w,h); }
  void drawBack(float x, float y, float w, float h) const { fbos[!flag].draw(x,y,w,h); }
  void draw(float x, float y)                             { fbos[flag].draw(x,y); }
  void drawBack(float x, float y)                         { fbos[!flag].draw(x,y); }

private:
  ofFbo fbos [2];
  bool flag;

};

class ftPingPongFboTex : public ofFbo {
public:
  void allocate(int width, int height, int internalformat = GL_RGBA, int numSamples = 0) {
    ofFboSettings settings;
    settings.numColorbuffers = 2;
    settings.width          = width;
    settings.height         = height;
    settings.internalformat = internalformat;
    settings.numSamples     = numSamples;
#ifdef TARGET_OPENGLES
    settings.useDepth       = false;
    settings.useStencil     = false;
    settings.textureTarget  = GL_TEXTURE_2D;
#else
    settings.useDepth    = true;
    settings.useStencil    = true;
    settings.textureTarget  = ofGetUsingArbTex() ? GL_TEXTURE_RECTANGLE_ARB : GL_TEXTURE_2D;
#endif
    allocate(settings);
  }

  void allocate(ofFboSettings _settings){
    if (_settings.numColorbuffers != 2) {
      ofLogWarning("ftPingPongFbo") << "needs to be allocated with 2 color buffers";
      _settings.numColorbuffers = 2;
    }
    if (_settings.numSamples != 0) {
      ofLogWarning("ftPingPongFbo") << "does not work with multisampling";
      _settings.numSamples = 0;
    }
    ofFbo::allocate(_settings);
    flag = false;
  }

  void swap(){
    flag = !flag;
    setDefaultTextureIndex(flag);
  }

  ofTexture& getBackTexture() { return ofFbo::getTexture(!flag); }

  bool flag;
};

}
