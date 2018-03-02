
#include "ftUtil.h"



namespace flowTools {
	
	ofMesh ftUtil::quad = ofMesh();
	
//	// draw texture in fbo using dimensions of texture
//	void ftUtil::draw(ofFbo& _dst, ofTexture& _tex) {
//		_dst.begin();
//		_tex.draw(0, 0);
//		_dst.end();
//	};
//	
//	// draw texture in fbo using dimensions of texture, aligned to the centre of the fbo
//	void ftUtil::centre(ofFbo& _dst, ofTexture& _tex) {
//		_dst.begin();
//		_tex.draw((_dst.getWidth() - _tex.getWidth()) / 2, (_dst.getHeight() - _tex.getHeight()) / 2, 0, 0);
//		_dst.end();
//	};
	
	// draw texture in fbo using dimensions of fbo, filling the fbo but distorting the texture
	void ftUtil::stretch(ofFbo& _dst, ofTexture& _tex) {
		_dst.begin();
		_tex.draw(0, 0, _dst.getWidth(), _dst.getHeight());
		_dst.end();
	};
	
	// draw texture in fbo using aspectratio of texture, showing the complete texture, but not filling the fbo
	void ftUtil::fit(ofFbo& _dst, ofTexture& _tex) {
		
		float meRatio = float(_dst.getWidth()) / float(_dst.getHeight());   // 0.5625
		float texRatio = float(_tex.getWidth()) / float(_tex.getHeight());   // 1.3333
		
		float width, height;
		float x0, y0, x1, y1;
		
		if (meRatio > texRatio) {
			height = _dst.getHeight();
			width = height * texRatio;
			
		}
		else {
			width = _dst.getWidth();
			height = width / texRatio;
		}
		
		x0 = (_dst.getWidth() - width) / 2;
		x1 = x0 + width;
		y0 = (_dst.getHeight() - height) / 2;
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
		
		_dst.begin();
		ofClear(0,0);
		_tex.bind();
		quad.draw();
		_tex.unbind();
		_dst.end();
	}
	
	// draw texture in fbo using aspectratio of texture, filling the fbo
	void ftUtil::fill(ofFbo& _dst, ofTexture& _tex) {
		
		float meRatio = float(_dst.getWidth()) / float(_dst.getHeight());   // 0.5625
		float texRatio = float(_tex.getWidth()) / float(_tex.getHeight());   // 1.3333
		
		float width, height;
		if (meRatio < texRatio) {
			height = _dst.getHeight();
			width = height * texRatio;
		}
		else {
			width = _dst.getWidth();
			height = width / texRatio;
		}
		
		float x0 = (_dst.getWidth() - width) / 2;
		float x1 = x0 + width;
		float y0 = (_dst.getHeight() - height) / 2;
		float y1 = y0 + height;
		
		if (!quad.hasVertices()) { initQuad(); }
		
		quad.setVertex(0, ofVec3f(x0,y0,0));
		quad.setVertex(1, ofVec3f(x1,y0,0));
		quad.setVertex(2, ofVec3f(x1,y1,0));
		quad.setVertex(3, ofVec3f(x0,y1,0));
		
		quad.setTexCoord(0, ofVec2f(0,0));
		quad.setTexCoord(1, ofVec2f(_tex.getWidth(),0));
		quad.setTexCoord(2, ofVec2f(_tex.getWidth(),_tex.getHeight()));
		quad.setTexCoord(3, ofVec2f(0,_tex.getHeight()));
		
		_dst.begin();
		ofClear(0,0);
		_tex.bind();
		quad.draw();
		_tex.unbind();
		_dst.end();
	}
	
	// draw texture in fbo using a normalized Region Of Interest
	void ftUtil::roi(ofFbo& _dst, ofTexture& _tex, ofRectangle _roi) {
		
		if (!quad.hasVertices()) { initQuad(); }
		
		quad.setVertex(0, ofVec3f(0,0,0));
		quad.setVertex(1, ofVec3f(_dst.getWidth(),0,0));
		quad.setVertex(2, ofVec3f(_dst.getWidth(),_dst.getHeight(),0));
		quad.setVertex(3, ofVec3f(0,_dst.getHeight(),0));
		
		float t0x = _roi.x * _tex.getWidth();
		float t0y = _roi.y * _tex.getHeight();
		float t1x = (_roi.x + _roi.width) * _tex.getWidth();
		float t1y = (_roi.y + _roi.height) * _tex.getHeight();
		
		quad.setTexCoord(0, ofVec2f(t0x, t0y));
		quad.setTexCoord(1, ofVec2f(t1x, t0y));
		quad.setTexCoord(2, ofVec2f(t1x, t1y));
		quad.setTexCoord(3, ofVec2f(t0x, t1y));
		
		_dst.begin();
		ofClear(0,0);
		_tex.bind();
		quad.draw();
		_tex.unbind();
		_dst.end();
	}
	
		// warp texture in fbo using four arbitrary points
	void ftUtil::warp(ofFbo& _dst, ofTexture& _tex, ofPoint _P0, ofPoint _P1, ofPoint _P2, ofPoint _P3) {
		
	}
	
	void ftUtil::toPixels(ofTexture& _tex, ofFloatPixels& _pixels) {
		ofTextureData& texData = _tex.getTextureData();
		int format = texData.glInternalFormat;
		int readFormat, numChannels;
		
		switch(format){
			case GL_R32F: 		readFormat = GL_R, 		numChannels = 1; break;
			case GL_RG32F: 		readFormat = GL_RG, 	numChannels = 2; break;
			case GL_RGB32F: 	readFormat = GL_RGB, 	numChannels = 3; break;
			case GL_RGBA32F:	readFormat = GL_RGBA,	numChannels = 4; break;
			default:
				ofLogWarning("ftUtil") << "toPixels: " << "can only read float textures to ofFloatPixels";
				return;
		}
		
		_pixels.allocate(texData.width, texData.height, numChannels);
		ofSetPixelStoreiAlignment(GL_PACK_ALIGNMENT, texData.width, 4, numChannels);
		glBindTexture(texData.textureTarget, texData.textureID);
		glGetTexImage(texData.textureTarget, 0, readFormat, GL_FLOAT, _pixels.getData());
		glBindTexture(texData.textureTarget, 0);
	}
	
	void ftUtil::toPixels(ofTexture& _tex, ofPixels& _pixels) {
		ofTextureData& texData = _tex.getTextureData();
		int format = texData.glInternalFormat;
		int readFormat, numChannels;
		
		switch(format){
			case GL_R8: 	readFormat = GL_R, 		numChannels = 1; break;
			case GL_RG8: 	readFormat = GL_RG, 	numChannels = 2; break;
			case GL_RGB8: 	readFormat = GL_RGB, 	numChannels = 3; break;
			case GL_RGBA8:	readFormat = GL_RGBA,	numChannels = 4; break;
			default:
				ofLogWarning("ftUtil") << "toPixels: " << "can only read char texturs to ofPixels";
				return;
		}
		
		_pixels.allocate(texData.width, texData.height, numChannels);
		ofSetPixelStoreiAlignment(GL_PACK_ALIGNMENT, texData.width, 1, numChannels);
		glBindTexture(texData.textureTarget, texData.textureID);
		glGetTexImage(texData.textureTarget, 0, readFormat, GL_FLOAT, _pixels.getData());
		glBindTexture(texData.textureTarget, 0);
	}
}
