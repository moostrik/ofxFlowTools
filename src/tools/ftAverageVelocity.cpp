
#include "ftAverageVelocity.h"

namespace flowTools {
	
	void ftAverageVelocity::setup(int _width, int _height, string _name) {
		width = _width;
		height = _height;
		
		pixelCount = _width * _height;
		
		scaleFbo.allocate(_width, _height, GL_RG32F);
		scaleFbo.black();
		pixels.allocate(_width, _height, 2);
		
		quad.getVertices().resize(4);
		quad.getTexCoords().resize(4);
		quad.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
		
		quad.setVertex(0, ofVec3f(0,0,0));
		quad.setVertex(1, ofVec3f(width,0,0));
		quad.setVertex(2, ofVec3f(width,height,0));
		quad.setVertex(3, ofVec3f(0,height,0));
		
		magnitudes.resize(pixelCount, 0);
		velocities.resize(pixelCount, ofVec2f(0,0));
		
		direction = ofVec2f(0,0);
		totalMagnitude = 0;
		meanMagnitude = 0;
		stdevMagnitude = 0;
		
		parameters.setName("average " + _name);
		parameters.add(pDirection.set("direction", ofVec2f(0,0), ofVec2f(-1,-1), ofVec2f(1,1)));
		parameters.add(pTotalMagnitude.set("total mag", "0"));
		parameters.add(pMeanMagnitude.set("mean mag", "0"));
		parameters.add(pStdevMagnitude.set("stdev mag", "0"));
		
		roiParameters.setName("ROI ");
		roiParameters.add(pRoiX.set("x", 0, 0, 1));
		pRoiX.addListener(this, &ftAverageVelocity::pRoiXListener);
		roiParameters.add(pRoiY.set("y", 0, 0, 1));
		pRoiY.addListener(this, &ftAverageVelocity::pRoiYListener);
		roiParameters.add(pRoiWidth.set("width", 1, 0, 1));
//		pRoiWidth.addListener(this, &ftAverageVelocity::pRoiWidthListener);
		roiParameters.add(pRoiHeight.set("height", 1, 0, 1));
//		pRoiHeight.addListener(this, &ftAverageVelocity::pRoiHeightListener);
		parameters.add(roiParameters);
	}
	
	void ftAverageVelocity::setSize(int _width, int _height) {
		if ( _width != width || _height != height ) {
			width = _width;
			height = _height;
			pixelCount = _width * _height;
			
			scaleFbo.allocate(_width, _height, GL_RG32F);
			pixels.allocate(_width, _height, 2);
			
			magnitudes.resize(pixelCount, 0);
			velocities.resize(pixelCount, ofVec2f(0,0));
			
			quad.setVertex(0, ofVec3f(0,0,0));
			quad.setVertex(1, ofVec3f(width,0,0));
			quad.setVertex(2, ofVec3f(width,height,0));
			quad.setVertex(3, ofVec3f(0,height,0));
		}
	}
	
	void ftAverageVelocity::setTexture(ofTexture _texture) {
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		scaleFbo.black();
		
//		scaleFbo.stretchIntoMe(_texture);

		int x,y,w,h;
		x = pRoiX.get() * _texture.getWidth();
		y = pRoiY.get() * _texture.getHeight();
		w = pRoiWidth.get() * _texture.getWidth();
		h = pRoiHeight.get() * _texture.getHeight();
		
		quad.setTexCoord(0, ofVec2f(x, y));
		quad.setTexCoord(1, ofVec2f(x+w, y));
		quad.setTexCoord(2, ofVec2f(x+w, y+h));
		quad.setTexCoord(3, ofVec2f(x, y+h));
		
		scaleFbo.begin();
		_texture.bind();
		quad.draw();
		_texture.unbind();
		scaleFbo.end();
		
		ofPopStyle();
	}
	
	void ftAverageVelocity::update() {
		// read to pixels
		ofTextureData& texData = scaleFbo.getTexture().getTextureData();
		ofSetPixelStoreiAlignment(GL_PACK_ALIGNMENT,width,4,2);
		glBindTexture(texData.textureTarget, texData.textureID);
		glGetTexImage(texData.textureTarget, 0, GL_RG, GL_FLOAT, pixels.getData());
		glBindTexture(texData.textureTarget, 0);
		float* floatPixelData = pixels.getData();
		
		// calculate magnitudes
		totalVelocity = ofVec2f(0,0);
		totalMagnitude = 0;
		float highMagnitude = 0;
		
		for (int i=0; i<pixelCount; i++) {
			ofVec2f *velocity = &velocities[i];
			velocity->x = floatPixelData[i*2];
			velocity->y = floatPixelData[i*2+1];
			totalVelocity += *velocity;
			
			magnitudes[i] = velocity->length();
			totalMagnitude += magnitudes[i];
			highMagnitude = max(highMagnitude, magnitudes[i]);
		}
		
		direction = totalVelocity.normalize();
		meanMagnitude = totalMagnitude / pixelCount;
		
		std::vector<float> diff(magnitudes.size());
		std::transform(magnitudes.begin(), magnitudes.end(), diff.begin(),
					   std::bind2nd(std::minus<float>(), meanMagnitude));
		float sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
		stdevMagnitude = std::sqrt(sq_sum / magnitudes.size());
		
		pDirection.set(direction);
		pTotalMagnitude.set(ofToString(totalMagnitude));
		pMeanMagnitude.set(ofToString(meanMagnitude));
		pStdevMagnitude.set(ofToString(stdevMagnitude));
	}
}