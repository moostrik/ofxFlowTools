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
		
		averageDirection = ofVec2f(0,0);
		averageMagnitude = 0;
		
		parameters.setName("average " + _name);
		parameters.add(pMagnitude.set("magnitude", 10, 1, 100));
		parameters.add(pDirection.set("direction", ofVec2f(0,0), ofVec2f(-1,-1), ofVec2f(1,1)));
		
	}
	
	void ftAverageVelocity::setSize(int _width, int _height) {
		if ( _width != averageFbo.getWidth() || _height != averageFbo.getHeight() ) {
			
			averageFbo.allocate(_width, _height, GL_RGB32F);
			delete[] floatPixelData;
			floatPixelData = new float [(_width * _height) * 2];
		}
	}
	
	void ftAverageVelocity::setTexture(ofTexture &_textureReference) {
		averageFbo.clear();
		averageFbo.stretchIntoMe(_textureReference);
	}
	
	void ftAverageVelocity::update() {
		
		int width = averageFbo.getWidth();
		int height = averageFbo.getHeight();
		
		ofTextureData& data = averageFbo.getTexture().getTextureData();
		
		ofFloatPixels dinges;
		dinges.getBytesPerChannel();
		
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
		ofVec2f velocity, cumVelocity(0,0);
		float	magnitude, cumMagnitude(0);
		
		for (int i=0; i<totalPixels; i++) {
			velocity.x = floatPixelData[i*2];
			velocity.y = floatPixelData[i*2+1];
			cumVelocity += velocity;
			
			magnitude = velocity.length();
			cumMagnitude += magnitude;
		}
		
		averageMagnitude = cumMagnitude / totalPixels * 100;
		averageDirection = cumVelocity.normalize();
		
		pMagnitude.set(averageMagnitude);
		pDirection.set(averageDirection);
		
//		cout << averageMagnitude << " " << averageDirection << " " << endl;
	}
}