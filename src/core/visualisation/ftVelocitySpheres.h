
#pragma once

#include "ofMain.h"
#include "ftVelocitySpheresShader.h"

namespace flowTools {
	
	class ftVelocitySpheres {
	public:
		
		void	setup(int _width, int _height){
			width = _width;
			height = _height;
			
			fieldMesh.setMode(OF_PRIMITIVE_POINTS);
			float xStep = 1. / width;
			float yStep = 1. / height;
			for (int x=0; x<width; x++) {
				for (int y=0; y<height; y++) {
					fieldMesh.addVertex(glm::vec3((x + 0.5) * xStep, (y + 0.5) * yStep, 0));
					fieldMesh.addTexCoord(glm::vec2((x + 0.5) * xStep, (y + 0.5) * yStep));
					fieldMesh.addColor(ofFloatColor(1,1,1,1));
				}
			}
			fieldVbo.setMesh(fieldMesh, GL_DYNAMIC_DRAW, true, true, false);
			
			parameters.setName("velocity dots");
			parameters.add(isActive.set("active", true));
			parameters.add(distanceScale.set("distance scale", .1, 0, 1));
			parameters.add(sizeScale.set("size scale", 1.5, 0, 2));
			parameters.add(minDotSize.set("min dot size", 2, 0, 20));
			parameters.add(maxDotSize.set("max dot size", 10, 0, 20));
		};
		
		void	draw(ofTexture& _velTex, int _x, int _y, int _width, int _height, ofBlendMode _blendmode = OF_BLENDMODE_ALPHA) {
			if (isActive.get()) {
			ofPushMatrix();
			ofPushStyle();
			
			ofEnableAlphaBlending();
			ofDisableAntiAliasing();
			
			ofTranslate(_x, _y);
			ofScale(_width, _height);
			velocityFieldShader.update(fieldVbo, *velocityTexture, distanceScale.get(), sizeScale.get(), minDotSize.get(), maxDotSize.get());
			
			ofEnableAntiAliasing();
			ofPopStyle();
			ofPopMatrix();
			}
		}
		
		void	setActive(bool _value)		{ isActive.set(_value); }
		void	setVelocity(ofTexture& tex)			{ velocityTexture = &tex; }
		
		bool	getActive()							{ return isActive.get(); }
		int		getWidth()							{ return width; }
		int		getHeight()							{ return height; }
		
		ofParameterGroup	parameters;
		
	protected:
		int		width;
		int		height;
		
		ofParameter<bool> 	isActive;
		ofParameter<float>	distanceScale;
		ofParameter<float>	sizeScale;
		ofParameter<float>	minDotSize;
		ofParameter<float>	maxDotSize;		// scale to normalize velocity
		
		ofTexture*	velocityTexture;
		ofMesh		fieldMesh;
		ofVbo		fieldVbo;
		
		ftVelocitySpheresShader velocityFieldShader;
		
	};
}

