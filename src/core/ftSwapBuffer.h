
#pragma once

#include "ofMain.h"
//#include "ftUtil.h"
//#include "ofFbo.h"

namespace flowTools {
	
	class ftSwapBuffer {
	public:
		ftSwapBuffer() { bIsAllocated = false; }
		
		void allocate( int _width, int _height, int _internalformat = GL_RGBA) { //}, int _filter = GL_LINEAR){
			
			ofFbo::Settings mySettings;
			mySettings.width = _width;
			mySettings.height = _height;
			mySettings.internalformat = _internalformat;
//			mySettings.maxFilter = _filter;
//			mySettings.minFilter = _filter;
			
			for(int i = 0; i < 2; i++){
				FBOs[i].allocate(mySettings);
			}
			bIsAllocated = true;
			
//			black();
			flag = 0;
			swap();
		}
		
		void swap(){
			backbuffer = &(FBOs[(flag)%2]);
			buffer = &(FBOs[++(flag)%2]);
		}
		
		bool isAllocated() { return bIsAllocated; }
		
//		void black(){
//			for(int i = 0; i < 2; i++){
//				FBOs[i].black();
//			}
//		}
//
//		void white(){
//			for(int i = 0; i < 2; i++){
//				FBOs[i].white();
//			}
//		}
		
		ofFbo& operator[]( int n ){ return FBOs[n]; }
		
		int getWidth() { return FBOs[0].getWidth(); }
		int getHeight() { return FBOs[0].getHeight(); }
//		int getInternalFormat() { return ftUtil::getInternalFormat(FBOs[0]); }
		
		ofFbo* getBuffer() { return buffer; }
		ofTexture& getTexture() { return buffer->getTexture(); }
		
		ofFbo* getBackBuffer() { return backbuffer; }
		ofTexture& getBackTexture() { return backbuffer->getTexture(); }
		
	private:
		ofFbo   *backbuffer;	// Source       ->  Ping
		ofFbo   *buffer;		// Destination  ->  Pong
		
		ofFbo   FBOs[2];    // Real addresses of ping/pong FBO´s
		int     flag;       // Integer for making a quick swap
		
		bool				bIsAllocated;
	};
}
