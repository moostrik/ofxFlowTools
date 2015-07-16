
#pragma once

#include "ofMain.h"
#include "ftPressureFieldShader.h"

namespace flowTools {
	
	class ftPressureField {
	public:
		
		void	setup(int _width, int _height){
			width = _width;
			height = _height;
			
			fieldMesh.setMode(OF_PRIMITIVE_POINTS);
			float xStep = 1. / width;
			float yStep = 1. / height;
			for (int x=0; x<width; x++) {
				for (int y=0; y<height; y++) {
					fieldMesh.addVertex(ofVec3f((x + 0.5) * xStep, (y + 0.5) * yStep, 0));
				}
			}
			fieldVbo.setMesh(fieldMesh, GL_DYNAMIC_DRAW, false, false, false);
			
			parameters.setName("pressure field");
			parameters.add(pressureScale.set("pressure scale", .45, 0, 1));
		};
		
		void	draw(int _x, int _y, int _width, int _height) {
			
			ofPushMatrix();
			ofPushStyle();
			
			ofEnableAlphaBlending();
			ofDisableAntiAliasing();
			
			ofScale(_width, _height);
			float radius = 2.0 / (height) * 0.275;
			pressureFieldShader.update(fieldVbo, *pressureTexture, pressureScale.get(), radius);
			
			ofEnableAntiAliasing();
			ofPopStyle();
			ofPopMatrix();
		}
		
		void	setPressure(ofTexture& tex)		{ pressureTexture = &tex; }
		void	setPressureScale(float _value)	{ pressureScale.set(_value); }
		
		float	getPressureScale()				{ return pressureScale.get(); }
		int		getWidth()						{ return width; }
		int		getHeight()						{ return height; }
		
		ofParameterGroup	parameters;
		
	protected:
		int		width;
		int		height;
		
		ofParameter<float>	pressureScale;
		
		ofTexture*	pressureTexture;
		ofMesh		fieldMesh;
		ofVbo		fieldVbo;
		
		ftPressureFieldShader pressureFieldShader;
	};
}

