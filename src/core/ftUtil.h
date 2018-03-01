
#pragma once

#include "ofMain.h"
#include "ofFbo.h"




namespace flowTools {
	
	class ftUtil {
	public:
		
		static void black(ofFbo& _fbo) { _fbo.begin(); ofClear(0,0,0,0); _fbo.end(); }
		static void white(ofFbo& _fbo) { _fbo.begin(); ofClear(255,255,255,255); _fbo.end(); }
		
		static GLint getInternalFormat(ofFbo& _fbo) { return _fbo.getTexture().getTextureData().glInternalFormat; };
		
		// draw texture in fbo using dimensions of texture
		static void draw(ofFbo& _dst, ofFbo& _src) { draw(_dst, _src.getTexture()); };
		static void draw(ofFbo& _dst, ofTexture& _tex);
		
		// draw texture in fbo using dimensions of texture, aligned to the centre of the fbo
		static void centre(ofFbo& _dst, ofFbo& _src) { draw(_dst, _src.getTexture()); };
		static void centre(ofFbo& _dst, ofTexture& _tex);
		
		// draw texture in fbo using dimensions of fbo, filling the fbo but distorting the texture
		static void stretch(ofFbo& _dst, ofFbo& _src) { stretch(_dst, _src.getTexture()); };
		static void stretch(ofFbo& _dst, ofTexture& _tex);
		
		// draw texture in fbo using aspectratio of texture, showing the complete texture, but not filling the fbo
		static void fit(ofFbo& _dst, ofFbo& _src) { fit(_dst, _src.getTexture()); };
		static void fit(ofFbo& _dst, ofTexture& _tex);
		
		// draw texture in fbo using aspectratio of texture, filling the fbo
		static void fill(ofFbo& _dst, ofFbo& _src) { fill(_dst, _src.getTexture()); };
		static void fill(ofFbo& _dst, ofTexture& _tex);
		
		// draw texture in fbo using a normalized Region Of Interest
		static void roi(ofFbo& _dst, ofFbo& _src, ofRectangle _roi) { roi(_dst, _src.getTexture(), _roi); };
		static void roi(ofFbo& _dst, ofTexture& _tex, ofRectangle _roi);
		
		// warp texture in fbo using four arbitrary points
		static void warp(ofFbo& _dst, ofFbo& _src, ofPoint _P0, ofPoint _P1, ofPoint _P2, ofPoint _P3) { warp(_dst, _src.getTexture(), _P0, _P1, _P2, _P3); };
		static void warp(ofFbo& _dst, ofTexture& _tex, ofPoint _P0, ofPoint _P1, ofPoint _P2, ofPoint _P3);
		
	protected:
		static ofMesh quad;
		
		static void initQuad() {
			quad.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
			quad.getVertices().resize(4);
			quad.getTexCoords().resize(4);
		}
	};
}
