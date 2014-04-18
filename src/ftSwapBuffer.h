
#pragma once

#include "ofMain.h"
#include "ftFbo.h"

namespace flowTools {
	
	class ftSwapBuffer {
	public:
		void allocate( int _width, int _height, int _internalformat = GL_RGBA, int _filter = GL_LINEAR){
			
			ofFbo::Settings mySettings;
			mySettings.width = _width;
			mySettings.height = _height;
			mySettings.internalformat = _internalformat;
			mySettings.maxFilter = _filter;
			mySettings.minFilter = _filter;
			
			for(int i = 0; i < 2; i++){
				FBOs[i].allocate(mySettings);
			}
			
			clear();
			
			flag = 0;
			swap();
			flag = 0;
		}
		
		void swap(){
			src = &(FBOs[(flag)%2]);
			dst = &(FBOs[++(flag)%2]);
		}
		
		void clear(){
			for(int i = 0; i < 2; i++){
				FBOs[i].clear();
			}
		}
				
		ofFbo& operator[]( int n ){ return FBOs[n];}
		
		int getWidth() { return FBOs[0].getWidth(); };
		int getHeight() { return FBOs[0].getHeight(); };
		int getInternalFormat() { return FBOs[0].getInternalFormat(); };
		
		ftFbo   *src;       // Source       ->  Ping
		ftFbo   *dst;       // Destination  ->  Pong
		
	private:
		ftFbo   FBOs[2];    // Real addresses of ping/pong FBO´s
		int     flag;       // Integer for making a quick swap
	};
}