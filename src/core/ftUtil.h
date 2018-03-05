
#pragma once

#include "ofMain.h"
#include "ofFbo.h"




namespace flowTools {
	
	class ftUtil {
	public:
		
		static void black(ofFbo& _fbo) { _fbo.begin(); ofClear(0,0,0,0); _fbo.end(); }
		static void white(ofFbo& _fbo) { _fbo.begin(); ofClear(255,255,255,255); _fbo.end(); }
		
//		// draw texture in fbo using dimensions of texture
//		static void draw(ofFbo& _dst, ofFbo& _src) { draw(_dst, _src.getTexture()); };
//		static void draw(ofFbo& _dst, ofTexture& _tex);
//		
//		// draw texture in fbo using dimensions of texture, aligned to the centre of the fbo
//		static void centre(ofFbo& _dst, ofFbo& _src) { centre(_dst, _src.getTexture()); };
//		static void centre(ofFbo& _dst, ofTexture& _tex);
		
		// draw _src in _dst using dimensions of _dst, filling the _dst but distorting the _src
		static void stretch(ofFbo& _dst, ofFbo& _src) { stretch(_dst, _src.getTexture()); };
		static void stretch(ofFbo& _dst, ofTexture& _tex);
		
		// draw _src in _dst using aspectratio of _src, showing the complete _src, but not filling the _dst
		static void fit(ofFbo& _dst, ofFbo& _src) { fit(_dst, _src.getTexture()); };
		static void fit(ofFbo& _dst, ofTexture& _tex);
		
		// draw _src in _dst using aspectratio of _src, filling the _dst
		static void fill(ofFbo& _dst, ofFbo& _src) { fill(_dst, _src.getTexture()); };
		static void fill(ofFbo& _dst, ofTexture& _tex);
		
		// draw _src in _dst using a normalized Region Of Interest
		static void roi(ofFbo& _dst, ofFbo& _src, ofRectangle _roi) { roi(_dst, _src.getTexture(), _roi); };
		static void roi(ofFbo& _dst, ofTexture& _tex, ofRectangle _roi);
		
		// warp _src in _dst using four arbitrary points
		static void warp(ofFbo& _dst, ofFbo& _src, ofPoint _P0, ofPoint _P1, ofPoint _P2, ofPoint _P3) { warp(_dst, _src.getTexture(), _P0, _P1, _P2, _P3); };
		static void warp(ofFbo& _dst, ofTexture& _tex, ofPoint _P0, ofPoint _P1, ofPoint _P2, ofPoint _P3);
		
		// get float pixels from a fbo or texture
		static void toPixels(ofFbo& _fbo, ofFloatPixels& _pixels) { toPixels(_fbo.getTexture(), _pixels); }
		static void toPixels(ofTexture& _tex, ofFloatPixels& _pixels);
		
		// get pixels from a fbo or texture // untested
		static void toPixels(ofFbo& _fbo, ofPixels& _pixels) { toPixels(_fbo.getTexture(), _pixels); }
		static void toPixels(ofTexture& _tex, ofPixels& _pixels);
		
		// get texture internatFormat
		static GLint getInternalFormat(ofFbo& _fbo) { return getInternalFormat(_fbo.getTexture()); };
		static GLint getInternalFormat(ofTexture& _tex) { return _tex.getTextureData().glInternalFormat; };
		
		// get number of channels in texture internatFormat
		static int getNumChannelsFromInternalFormat(GLint format);
		
		// get unsigned char internatFormat from number of channels;
		static GLint getUCharInternalFormat(int _numChannels);
		
		// get float internatFormat from number of channels;
		static GLint getFloatInternalFormat(int _numChannels);
		
		// get if internalFormat is a float;
		bool isFloat(GLint _format);
		
	protected:
		static ofMesh quad;
		
		static void initQuad() {
			quad.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
			quad.getVertices().resize(4);
			quad.getTexCoords().resize(4);
		}
	};
}
