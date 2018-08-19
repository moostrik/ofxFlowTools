
#pragma once

#include "ofMain.h"

namespace flowTools {
	
	class ftPingPongFbo : public ofFbo {
	public:
		void allocate(int width, int height, int internalformat = GL_RGBA, int numSamples = 0) {
			ofFboSettings settings;
			settings.numColorbuffers = 2;
			settings.width			= width;
			settings.height			= height;
			settings.internalformat	= internalformat;
			settings.numSamples		= numSamples;
#ifdef TARGET_OPENGLES
			settings.useDepth		= false;
			settings.useStencil		= false;
			settings.textureTarget	= GL_TEXTURE_2D;
#else
			settings.useDepth		= true;
			settings.useStencil		= true;
			settings.textureTarget	= ofGetUsingArbTex() ? GL_TEXTURE_RECTANGLE_ARB : GL_TEXTURE_2D;
#endif
			allocate(settings);
		}
		
		void allocate(ofFboSettings _settings){
			if (_settings.numColorbuffers != 2) {
				ofLogWarning("ftPingPongFbo") << "needs to be allocated with 2 color buffers";
				_settings.numColorbuffers = 2;
			}
			ofFbo::allocate(_settings);
			flag = false;
		}
		
		void swap(){
			flag = !flag;
			bind();
			setActiveDrawBuffer(flag);
			setDefaultTextureIndex(flag);
			unbind();
		}
		
		ofTexture& getBackTexture() { return ofFbo::getTexture(!flag); }
		
		private:
		bool flag;
	};
}
