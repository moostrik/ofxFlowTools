
#pragma once

#include "ofMain.h"
#include "ofFbo.h"

namespace flowTools {
	
	class ftFbo : public ofFbo {
	public:
		
		void clear() {
			ofLogWarning("clear() was used by flowtools to clear the contents of the fbo. OF 0.9 uses this method to destroy the fbo. Use 'black()' instead'");
			black();
		}
		
		void black() { begin(); ofClear(0, 0); end(); }
		void white() { begin(); ofClear(255); end(); }
		
		// draw texture in fbo using dimensions of texture
		void drawIntoMe(ofFbo& _fbo) { drawIntoMe(_fbo.getTexture()); };
		void drawIntoMe(ofTexture& _tex) {  begin(); _tex.draw(0, 0); end(); };
		
		// draw texture in fbo using dimensions of texture, aligned to the centre of the fbo
		void centreIntoMe(ofFbo& _fbo) { drawIntoMe(_fbo.getTexture()); };
		void centreIntoMe(ofTexture& _tex) { begin(); _tex.draw((getWidth() - _tex.getWidth()) / 2, (getHeight() - _tex.getHeight()) / 2, 0, 0); end(); };
		
		// draw texture in fbo using dimensions of fbo, filling the fbo but distorting the texture
		void stretchIntoMe(ofFbo& _fbo) { stretchIntoMe(_fbo.getTexture()); };
		void stretchIntoMe(ofTexture& _tex) {  begin(); _tex.draw(0, 0, getWidth(), getHeight()); end(); };
		
		// draw texture in fbo using aspectratio of texture, showing the complete texture, but not filling thye fbo
		void fitIntoMe(ofFbo& _fbo) { fitIntoMe(_fbo.getTexture()); };
		void fitIntoMe(ofTexture& _tex) {
			
			float meRatio = float(getWidth()) / float(getHeight());   // 0.5625
			float texRatio = float(_tex.getWidth()) / float(_tex.getHeight());   // 1.3333
			
			float width, height;
			float x0, y0, x1, y1;
			
			if (meRatio > texRatio) {
				height = getHeight();
				width = height * texRatio;
				
			}
			else {
				width = getWidth();
				height = width / texRatio;
			}
			
			x0 = (getWidth() - width) / 2;
			x1 = x0 + width;
			y0 = (getHeight() - height) / 2;
			y1 = y0 + height;
			
			ofMesh quad;
			quad.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
			
			quad.addVertex(ofVec3f(x0,y0,0));
			quad.addVertex(ofVec3f(x1,y0,0));
			quad.addVertex(ofVec3f(x1,y1,0));
			quad.addVertex(ofVec3f(x0,y1,0));
			
			quad.addTexCoord(ofVec2f(0,0));
			quad.addTexCoord(ofVec2f(_tex.getWidth(),0));
			quad.addTexCoord(ofVec2f(_tex.getWidth(),_tex.getHeight()));
			quad.addTexCoord(ofVec2f(0,_tex.getHeight()));
			
			begin();
			ofClear(0,0);
			_tex.bind();
			quad.draw();
			_tex.unbind();
			end();
		}
		
		// draw texture in fbo using aspectratio of texture, filling the fbo
		void fillMe(ofFbo& _fbo) { fillMe(_fbo.getTexture()); };
		void fillMe(ofTexture& _tex) {
			
			float meRatio = float(getWidth()) / float(getHeight());   // 0.5625
			float texRatio = float(_tex.getWidth()) / float(_tex.getHeight());   // 1.3333
			
			float width, height;
			if (meRatio < texRatio) {
				height = getHeight();
				width = height * texRatio;
			}
			else {
				width = getWidth();
				height = width / texRatio;
			}
			
			float x0 = (getWidth() - width) / 2;
			float x1 = x0 + width;
			float y0 = (getHeight() - height) / 2;
			float y1 = y0 + height;
			
			ofMesh quad;
			quad.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
			
			quad.addVertex(ofVec3f(x0,y0,0));
			quad.addVertex(ofVec3f(x1,y0,0));
			quad.addVertex(ofVec3f(x1,y1,0));
			quad.addVertex(ofVec3f(x0,y1,0));
			
			quad.addTexCoord(ofVec2f(0,0));
			quad.addTexCoord(ofVec2f(_tex.getWidth(),0));
			quad.addTexCoord(ofVec2f(_tex.getWidth(),_tex.getHeight()));
			quad.addTexCoord(ofVec2f(0,_tex.getHeight()));
			
			begin();
			ofClear(0,0);
			_tex.bind();
			quad.draw();
			_tex.unbind();
			end();
		}
		
		GLint getInternalFormat() { return getTexture().getTextureData().glInternalFormat; };
				
	};
}