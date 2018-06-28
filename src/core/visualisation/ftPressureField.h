
#pragma once

#include "ofMain.h"
#include "ftPressureFieldShader.h"

namespace flowTools {
	
	class ftPressureField {
	public:
		ftPressureField() {
			parameters.setName("pressure field");
			parameters.add(isActive.set("active", true));
			parameters.add(pressureScale.set("pressure scale", .45, 0, 1));
		}
		
		void	setup(int _width, int _height){
			width = _width;
			height = _height;
			
			fieldMesh.clear();
			fieldMesh.setMode(OF_PRIMITIVE_POINTS);
			float xStep = 1. / width;
			float yStep = 1. / height;
			for (int x=0; x<width; x++) {
				for (int y=0; y<height; y++) {
					fieldMesh.addVertex(glm::vec3((x + 0.5) * xStep, (y + 0.5) * yStep, 0));
				}
			}
			fieldVbo.setMesh(fieldMesh, GL_DYNAMIC_DRAW, false, false, false);
		};
		
		void draw(ofTexture& _prsTex, int _x, int _y, int _width, int _height, ofBlendMode _blendmode = OF_BLENDMODE_ALPHA) {
			if (isActive.get()) {
				ofPushMatrix();
				ofPushStyle();
				
				ofEnableBlendMode(_blendmode);
				
				ofTranslate(_x, _y);
				ofScale(_width, _height);
				float radius = 2.0 / (height) * 0.275;
				pressureFieldShader.update(fieldVbo, _prsTex, pressureScale.get(), radius);
				
				ofPopStyle();
				ofPopMatrix();
			}
		}
		
		void	setActive(bool _value)			{ isActive.set(_value); }
		void	setScale(float _value)			{ pressureScale.set(_value); }
		void	setPressureScale(float _value)	{ setScale(_value); }
		
		int		getWidth()						{ return width; }
		int		getHeight()						{ return height; }
		bool	getActive()						{ return isActive.get(); }
		float	getPressureScale()				{ return pressureScale.get(); }
		
		ofParameterGroup	parameters;
		
	protected:
		int		width;
		int		height;
		
		ofParameter<bool> 		isActive;
		ofParameter<float>		pressureScale;
		ofMesh					fieldMesh;
		ofVbo					fieldVbo;
		ftPressureFieldShader	pressureFieldShader;
	};
}

