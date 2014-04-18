
#pragma once

#include "ofMain.h"


#define STRINGIFY(A) #A

namespace flowTools {
	
	class ftShader{
	public:
		~ftShader() {shader.unload();};
		
	protected:
		
		void renderFrame(float _width, float _height){
			ofSetColor(255,255);
			glBegin(GL_QUADS);
			glTexCoord2f(0, 0); glVertex3f(0, 0, 0);
			glTexCoord2f(_width, 0); glVertex3f(_width, 0, 0);
			glTexCoord2f(_width, _height); glVertex3f(_width, _height, 0);
			glTexCoord2f(0,_height);  glVertex3f(0,_height, 0);
			glEnd();
		}
		
		void renderNormalizedFrame(float _width, float _height){
			ofSetColor(255,255);
			glBegin(GL_QUADS);
			glTexCoord2f(0, 0); glVertex3f(0, 0, 0);
			glTexCoord2f(1, 0); glVertex3f(_width, 0, 0);
			glTexCoord2f(1, 1); glVertex3f(_width, _height, 0);
			glTexCoord2f(0, 1);  glVertex3f(0,_height, 0);
			glEnd();
		}
		
		ofShader shader;
		string fragmentShader;
	};
	
}