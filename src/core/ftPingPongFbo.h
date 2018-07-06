
#pragma once

#include "ofMain.h"

namespace flowTools {
	
	class ftPingPongFbo : public ofFbo {
	public:
		void allocate(int width, int height, int internalformat = GL_RGBA, int numSamples = 0) {
			ofFboSettings settings;
			settings.width			= width;
			settings.height			= height;
			settings.internalformat	= internalformat;
			settings.numSamples		= numSamples;
			settings.numColorbuffers = 2;
			allocate(settings);
		}
		
		void allocate(ofFboSettings _settings){
			if (_settings.numColorbuffers != 2) {
				ofLogError("ftPingPongFbo") << "needs to be allocated with 2 color buffers";
				return;
			}
			ofFbo::allocate(_settings);
			flag = 0;
		}
		
		void swap(){
			flag = 1-flag;
			bind();
			setActiveDrawBuffer(flag);
			setDefaultTextureIndex(flag);
			unbind();
		}
		
//		ofTexture& getTexture() { return ofFbo::getTexture(flag); }
		ofTexture& getBackTexture() { return ofFbo::getTexture(1-flag); }
		
		private:
		int flag;
	};
}
