//
//  averageVelocity.cpp
//  beerToolsApp
//
//  Created by Ties East on 27/5/15.
//
//

#include "ftAverageVelocity.h"

namespace flowTools {
	
	void ftAverageVelocity::setup(int _width, int _height, string _name) {
		
		averageFbo.allocate(_width, _height, GL_RGB32F);
		floatPixelData = new float [(_width * _height) * 2];
		
		direction = ofVec2f(0,0);
		totalMagnitude = 0;
		averageMagnitude = 0;
		
		parameters.setName("average " + _name);
		parameters.add(pDirection.set("direction", ofVec2f(0,0), ofVec2f(-1,-1), ofVec2f(1,1)));
		parameters.add(pMagnitude.set("magnitude", 0, 0, 100));
		parameters.add(pAverageMagnitude.set("magnitude", 0, 0, 1));
		
	}
	
	void ftAverageVelocity::setSize(int _width, int _height) {
		if ( _width != averageFbo.getWidth() || _height != averageFbo.getHeight() ) {
			
			averageFbo.allocate(_width, _height, GL_RG32F);
			delete[] floatPixelData;
			floatPixelData = new float [(_width * _height) * 2];
		}
	}
	
	void ftAverageVelocity::setTexture(ofTexture &_textureReference) {
		averageFbo.black();
		averageFbo.stretchIntoMe(_textureReference);
	}
	
	void ftAverageVelocity::update() {
		
		int width = averageFbo.getWidth();
		int height = averageFbo.getHeight();
		
		ofTextureData& data = averageFbo.getTexture().getTextureData();
		
		ofSetPixelStoreiAlignment(GL_PACK_ALIGNMENT,width,4,2);
		glBindTexture(data.textureTarget, data.textureID);
		glGetTexImage(data.textureTarget, 0, GL_RG, GL_FLOAT, floatPixelData);
		glBindTexture(data.textureTarget, 0);
		
		// ----- presumably slower method -----
//		averageFbo.bind();
//		glReadPixels(0, 0, width, height, GL_RG, GL_FLOAT, floatPixelData);
//		averageFbo.unbind();
		// ----- presumably slower method -----
		
		int		totalPixels = width * height;
		ofVec2f velocity, totalVelocity(0,0);
		float	magnitude;
		highMagnitude = 0;
		
		totalMagnitude = 0;
		for (int i=0; i<totalPixels; i++) {
			velocity.x = floatPixelData[i*2];
			velocity.y = floatPixelData[i*2+1];
			totalVelocity += velocity;
			
			magnitude = velocity.length();
			totalMagnitude += magnitude;
			highMagnitude = max(highMagnitude, magnitude);
		}
		
		averageMagnitude = totalMagnitude / totalPixels;
		direction = totalVelocity.normalize();
		
		pMagnitude.set(totalMagnitude);
		pDirection.set(direction);
	}
}