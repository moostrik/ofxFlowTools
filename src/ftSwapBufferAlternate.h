
#pragma once

#include "ofMain.h"
#include "ftFbo.h"

namespace flowTools {
	
	class ftSwapBuffer : public ftFbo {
	public:
		void allocate( int _width, int _height, int _internalformat = GL_RGBA, int _filter = GL_LINEAR){
			
			ofFbo::Settings mySettings;
			mySettings.width = _width;
			mySettings.height = _height;
			mySettings.internalformat = _internalformat;
			mySettings.maxFilter = _filter;
			mySettings.minFilter = _filter;
			mySettings.numColorbuffers = 2;
			
			
			ofFbo::allocate(mySettings);
			clear();
			
		//	for(int i = 0; i < 2; i++){
		//		FBOs[i].allocate(mySettings);
		//	}
			
			
			ofLogWarning("ftSwapBuffer") << getDefaultTextureIndex() << " " << getNumTextures() << " " << flag;
			
			
			
			swap();
			
			ofLogWarning("ftSwapBuffer") << getDefaultTextureIndex() << " " << getNumTextures() << " " << flag;
			
		}
		
		void swap(){
			flag = (flag+1) %2;
			
			
	//		glDrawBuffer(GL_COLOR_ATTACHMENT0 + flag);
	//		glReadBuffer(GL_COLOR_ATTACHMENT0 + flag);
			
			setDefaultTextureIndex(flag);
			
			
		//	FBO.setActiveDrawBuffer(++(flag)%2);
		//	FBO.setDefaultTexturendex(++(flag)%2);	//		FBO.setActiveDrawBuffer(flag);
		//	++(flag)%2;
		//	FBO.setDefaultTextureIndex(++(flag)%2);
		//	backbuffer = &(FBOs[(flag)%2]);
		//	buffer = &(FBOs[++(flag)%2]);
			
		}
		
		
		ftFbo* getBuffer() { return this; }
		ofTexture& getTexture() { return getTextureReference(); }
		
//		ftFbo* getBackBuffer() { return backbuffer; }
		ofTexture& getBackTexture() { return getTextureReference(); }
		
		
	private:
//		ftFbo   *backbuffer;	// Source       ->  Ping
//		ftFbo   *buffer;		// Destination  ->  Pong
		
//		ftFbo	FBO;
//		ftFbo   FBOs[2];    // Real addresses of ping/pong FBO´s
		int     flag;       // Integer for making a quick swap
	};
}