
#include "ftUtil.h"



namespace flowTools {
	
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
		
		quad.addVertex(glm::vec3(x0,y0,0));
		quad.addVertex(glm::vec3(x1,y0,0));
		quad.addVertex(glm::vec3(x1,y1,0));
		quad.addVertex(glm::vec3(x0,y1,0));
		
		quad.addTexCoord(glm::vec2(0,0));
		quad.addTexCoord(glm::vec2(_tex.getWidth(),0));
		quad.addTexCoord(glm::vec2(_tex.getWidth(),_tex.getHeight()));
		quad.addTexCoord(glm::vec2(0,_tex.getHeight()));
		
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
		
		ofMesh quad;
		quad.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
		
		quad.addVertex(glm::vec3(x0,y0,0));
		quad.addVertex(glm::vec3(x1,y0,0));
		quad.addVertex(glm::vec3(x1,y1,0));
		quad.addVertex(glm::vec3(x0,y1,0));
		
		quad.addTexCoord(glm::vec2(0,0));
		quad.addTexCoord(glm::vec2(_tex.getWidth(),0));
		quad.addTexCoord(glm::vec2(_tex.getWidth(),_tex.getHeight()));
		quad.addTexCoord(glm::vec2(0,_tex.getHeight()));
		
		_dst.begin();
		ofClear(0,0);
		_tex.bind();
		quad.draw();
		_tex.unbind();
		_dst.end();
	}
	
		// draw texture in fbo using a normalized Region Of Interest
	void ftUtil::roi(ofFbo& _dst, ofTexture& _tex, ofRectangle _roi) {
		
		ofMesh quad;
		quad.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
		
		quad.addVertex(glm::vec3(0,0,0));
		quad.addVertex(glm::vec3(_dst.getWidth(),0,0));
		quad.addVertex(glm::vec3(_dst.getWidth(),_dst.getHeight(),0));
		quad.addVertex(glm::vec3(0,_dst.getHeight(),0));
		
		float t0x = _roi.x * _tex.getWidth();
		float t0y = _roi.y * _tex.getHeight();
		float t1x = (_roi.x + _roi.width) * _tex.getWidth();
		float t1y = (_roi.y + _roi.height) * _tex.getHeight();
		
		quad.addTexCoord(glm::vec2(t0x, t0y));
		quad.addTexCoord(glm::vec2(t1x, t0y));
		quad.addTexCoord(glm::vec2(t1x, t1y));
		quad.addTexCoord(glm::vec2(t0x, t1y));
		
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
	
		// get float pixels from a fbo or texture
	void ftUtil::toPixels(ofTexture& _tex, ofFloatPixels& _pixels) {
		ofTextureData& texData = _tex.getTextureData();
		int format = texData.glInternalFormat;
		int readFormat, numChannels;
		
		switch(format){
			case GL_R32F: 		readFormat = GL_RED, 	numChannels = 1; break; // or is it GL_R
			case GL_RG32F: 		readFormat = GL_RG, 	numChannels = 2; break;
			case GL_RGB32F: 	readFormat = GL_RGB, 	numChannels = 3; break;
			case GL_RGBA32F:	readFormat = GL_RGBA,	numChannels = 4; break;
			default:
				ofLogWarning("ftUtil") << "toPixels: " << "can only read float textures to ofFloatPixels";
				return;
		}
		if (_pixels.getWidth() != texData.width || _pixels.getHeight() != texData.height || _pixels.getNumChannels() != numChannels) {
			_pixels.allocate(texData.width, texData.height, numChannels);
		}
		ofSetPixelStoreiAlignment(GL_PACK_ALIGNMENT, texData.width, 4, numChannels);
		glBindTexture(texData.textureTarget, texData.textureID);
		glGetTexImage(texData.textureTarget, 0, readFormat, GL_FLOAT, _pixels.getData());
		glBindTexture(texData.textureTarget, 0);
	}
	
		// get pixels from a fbo or texture // untested
	void ftUtil::toPixels(ofTexture& _tex, ofPixels& _pixels) {
		ofTextureData& texData = _tex.getTextureData();
		int format = texData.glInternalFormat;
		int readFormat, numChannels;
		
		switch(format){
			case GL_R8: 	readFormat = GL_RED, 	numChannels = 1; break; // or is it GL_R
			case GL_RG8: 	readFormat = GL_RG, 	numChannels = 2; break;
			case GL_RGB8: 	readFormat = GL_RGB, 	numChannels = 3; break;
			case GL_RGBA8:	readFormat = GL_RGBA,	numChannels = 4; break;
			default:
				ofLogWarning("ftUtil") << "toPixels: " << "can only read char texturs to ofPixels";
				return;
		}
		if (_pixels.getWidth() != texData.width || _pixels.getHeight() != texData.height || _pixels.getNumChannels() != numChannels) {
			_pixels.allocate(texData.width, texData.height, numChannels);
		}
		ofSetPixelStoreiAlignment(GL_PACK_ALIGNMENT, texData.width, 1, numChannels);
		glBindTexture(texData.textureTarget, texData.textureID);
		glGetTexImage(texData.textureTarget, 0, readFormat, GL_UNSIGNED_BYTE, _pixels.getData());
		glBindTexture(texData.textureTarget, 0);
	}
	
	// get number of channels in texture internalFormat
	int ftUtil::getNumChannelsFromInternalFormat(GLint _internalFormat) {
		switch(_internalFormat){
			case GL_R8:
			case GL_R32F:
				return 1;
			case GL_RG:
			case GL_RG8:
			case GL_RG32F:
				return 2; ;
			case GL_RGB:
			case GL_RGB8:
			case GL_RGB32F:
				return 3;
			case GL_RGBA:
			case GL_RGBA8:
			case GL_RGBA32F:
				return 4;
			default:
				ofLogWarning("ftUtil") << "getNumChannelsFromInternalFormat: " << "flowtools does not use internalFormat " << ofToHex(_internalFormat);
				break;
				return 0;
		}
	}
	
	// get number of channels in texture format
	int ftUtil::getNumChannelsFromFormat(GLint _internalFormat) {
		switch(_internalFormat){
			case GL_RED:
				return 1;
			case GL_RG:
				return 2; ;
			case GL_RGB:
				return 3;
			case GL_RGBA:
				return 4;
			default:
				ofLogWarning("ftUtil") << "getNumChannelsFromFormat: " << "flowtools does not use format " << ofToHex(_internalFormat);
				break;
				return 0;
		}
	}
	
		// get unsigned char format from number of channels;
	int ftUtil::getUCharInternalFormat(int _numChannels) {
		int internalFormat = GL_NONE;
		switch(_numChannels){
			case 1: internalFormat = GL_R8; break;
			case 2: internalFormat = GL_RG8; break;
			case 3: internalFormat = GL_RGB8; break;
			case 4: internalFormat = GL_RGBA8;break;
			default:
				ofLogWarning("ftUtil") << "getUCharInternalFormat: " << "1 to 4 channels, not " << _numChannels;
				break;
		}
		return internalFormat;
	}
	
		// get float format from number of channels;
	int ftUtil::getFloatInternalFormat(int _numChannels) {
		int internalFormat = GL_NONE;
		switch(_numChannels){
			case 1: internalFormat = GL_R32F; break;
			case 2: internalFormat = GL_RG32F; break;
			case 3: internalFormat = GL_RGB32F; break;
			case 4: internalFormat = GL_RGBA32F;break;
			default:
				ofLogWarning("ftUtil") << "getFloatInternalFormat: " << "1 to 4 channels, not " << _numChannels;
				break;
		}
		return internalFormat;
	}
	
		// get type from internal format;
	bool ftUtil::isFloat(GLint _internalFormat) {
		bool isFloat = false;
		switch(_internalFormat){
			case GL_R32F:
			case GL_RG32F:
			case GL_RGB32F:
			case GL_RGBA32F:
				return true;
				break;
			case GL_R8:
			case GL_RG:
			case GL_RG8:
			case GL_RGB:
			case GL_RGB8:
			case GL_RGBA:
			case GL_RGBA8:
				return false;
				break;
			default:
				ofLogWarning("ftUtil") << "isFloat: " << "Internal format " << ofToHex(_internalFormat) << " not supported";
				break;
		}
	}
}

