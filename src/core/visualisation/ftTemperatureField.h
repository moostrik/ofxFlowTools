
#pragma once

#include "ofMain.h"
#include "ftTemperatureFieldShader.h"

namespace flowTools {
	
	class ftTemperatureField {
	public:
		ftTemperatureField() {
			parameters.setName("temperature field");
			parameters.add(isActive.set("active", true));
			parameters.add(temperatureScale.set("temperature scale", .25, 0, 1));
			parameters.add(lineSmooth.set("line smooth", false));
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
		
		void	draw(ofTexture& _temTex, int _x, int _y, int _width, int _height, ofBlendMode _blendmode = OF_BLENDMODE_ALPHA) {
			if (isActive.get()) {
				ofPushMatrix();
				ofPushStyle();
				
				ofEnableAlphaBlending();
				ofDisableAntiAliasing();
				
				if (lineSmooth.get()) {
					glEnable(GL_LINE_SMOOTH);
				}
				
				ofTranslate(_x, _y);
				ofScale(_width, _height);
				float barHeight =  2.0 * 0.9 / height;
				float barWidth = 5.0;
				temperatureFieldShader.update(fieldVbo, _temTex, temperatureScale.get(), barHeight, barWidth);
				
				if (lineSmooth.get()) {
					glDisable(GL_LINE_SMOOTH);
				}
				
				ofEnableAntiAliasing();
				ofPopStyle();
				ofPopMatrix();
			}
		}
		
		void	setActive(bool _value)				{ isActive.set(_value); }
		void	setScale(float _value)				{ temperatureScale.set(_value); }
		void	setTemperatureScale(float _value)	{ setScale(_value); }
		void	setLineSmooth(bool _value)			{ lineSmooth.set(_value); }
		
		int		getWidth()							{ return width; }
		int		getHeight()							{ return height; }
		bool	getActive()							{ return isActive.get(); }
		float	getTemperatureScale()				{ return temperatureScale.get(); }
		bool	getLineSmooth()						{ return lineSmooth.get(); }
		
		ofParameterGroup	parameters;
		
	protected:
		int		width;
		int		height;
		
		ofParameter<bool> 	isActive;
		ofParameter<float>	temperatureScale;
		ofParameter<bool>	lineSmooth;
		
		ofMesh		fieldMesh;
		ofVbo		fieldVbo;
		
		ftTemperatureFieldShader temperatureFieldShader;
		
	};
}

