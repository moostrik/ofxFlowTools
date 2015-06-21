//
//  averageVelocity.cpp
//  beerToolsApp
//
//  Created by Ties East on 27/5/15.
//
//

#include "ftAverageVelocity.h"



void ftAverageVelocity::setup(int _width, int _height, string _name) {
	
	ofFbo::Settings mySettings;
	
	mySettings.width = _width;
	mySettings.height = _height;
	mySettings.internalformat = GL_RGB32F;
	mySettings.maxFilter = GL_LINEAR;
	mySettings.minFilter = GL_LINEAR;
	
	averageFbo.allocate(mySettings);
	
	averageFloats = new float [(_width * _height) * 2];
	
	direction = ofVec2f(0,0);
	amplitude = 0;
	area = 0;
	
	parameters.setName("average " + _name);
	parameters.add(pDirection.set("velocity", ofVec2f(0,0), ofVec2f(-1,-1), ofVec2f(1,1)));
	parameters.add(pAmplitudeMultiplier.set("amp multiplier", 10, 1, 100));
	parameters.add(pAmplitude.set("amplitude", 0, 0, 1));
	parameters.add(pAreaTreshold.set("area treshold", 0.2, 0, 5));
	parameters.add(pArea.set("area", 0, 0, 1));
	
}

void ftAverageVelocity::setSize(int _width, int _height) {
	if ( _width != averageFbo.getWidth() || _height != averageFbo.getHeight() ) {
		ofFbo::Settings mySettings;
		
		mySettings.width = _width;
		mySettings.height = _height;
		mySettings.internalformat = GL_RG32F;
		mySettings.maxFilter = GL_LINEAR;
		mySettings.minFilter = GL_LINEAR;
		
		averageFbo.allocate(mySettings);
		
		delete[] averageFloats;
		averageFloats = new float [(_width * _height) * 2];
		
	}
}

void ftAverageVelocity::setTexture(ofTexture &_textureReference) {
	averageFbo.stretchIntoMe(_textureReference);
}

void ftAverageVelocity::update() {
	
	int width = averageFbo.getWidth();
	int height = averageFbo.getHeight();
	
	averageFbo.bind();
	glReadPixels(0, 0, width, height, GL_RG, GL_FLOAT, averageFloats);
	averageFbo.unbind();
	
	int		totalPixels = width * height;
	ofVec2f cumVelocity;
	ofVec2f velocity;
	area = 0;
	
	for (int i=0; i<totalPixels; i++) {
		velocity.x = averageFloats[i * 2];
		velocity.y = averageFloats[i * 2 + 1];
		
		cumVelocity += velocity;
		
		if (velocity.length() > pAreaTreshold.get())
			area += 1;
	}
	
	amplitude = ofClamp(cumVelocity.length() / (totalPixels) * pAmplitudeMultiplier.get(), 0, 1);
	direction = cumVelocity.normalize();
	area /= totalPixels;
	
	pDirection.set(direction);
	pAmplitude.set(amplitude);
	pArea.set(area);
	
}