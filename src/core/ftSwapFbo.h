
#pragma once

#include "ofMain.h"
#include "ftFbo.h"

namespace flowTools {
	
	class ftSwapFbo : public ftFbo {
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
			swap();
			
		}
		
		ofTexture& getBackTexture() { if (flag) return getTextureReference(GL_COLOR_ATTACHMENT0); else getTextureReference(GL_COLOR_ATTACHMENT1); }
		
		void swap(){
			flag = (flag+1) %2;
			
			if (flag) {
				glDrawBuffer(GL_COLOR_ATTACHMENT0);
				glReadBuffer(GL_COLOR_ATTACHMENT1);
			}
			else {
				glDrawBuffer(GL_COLOR_ATTACHMENT1);
				glReadBuffer(GL_COLOR_ATTACHMENT0);
			}
		}
		
		private:
		int     flag;       // Integer for making a quick swap
	};
}