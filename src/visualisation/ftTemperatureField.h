
#pragma once

#include "ofMain.h"
#include "ftTemperatureFieldShader.h"

namespace flowTools {
	
	class ftTemperatureField {
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
			
			
			parameters.setName("temperature field");
			parameters.add(temperatureScale.set("temperature scale", .25, 0, 1));
			parameters.add(lineSmooth.set("line smooth", false));
		};
		
		void	draw(int _x, int _y, int _width, int _height) {
			
			ofPushMatrix();
			ofPushStyle();
			
			ofEnableAlphaBlending();
			ofDisableAntiAliasing();
			
			if (lineSmooth.get()) {
				glEnable(GL_LINE_SMOOTH);
			}
						
			ofScale(_width, _height);
			float barHeight =  2.0 * 0.9 / height;
			float barWidth = 5.0;
			temperatureFieldShader.update(fieldVbo, *temperatureTexture, temperatureScale.get(), barHeight, barWidth);
			
			if (lineSmooth.get()) {
				glDisable(GL_LINE_SMOOTH);
			}
			
			ofEnableAntiAliasing();
			ofPopStyle();
			ofPopMatrix();
		}
		
		void	setTemperature(ofTexture& tex)		{ temperatureTexture = &tex; }
		void	setTemperatureScale(float _value)	{ temperatureScale.set(_value); }
		void	setLineSmooth(bool _value)			{ lineSmooth.set(_value); }
		
		float	getTemperatureScale()				{ return temperatureScale.get(); }
		bool	getLineSmooth()						{ return lineSmooth.get(); }
		int		getWidth()							{ return width; }
		int		getHeight()							{ return height; }
		
		
		ofParameterGroup	parameters;
		
	protected:
		int		width;
		int		height;
		
		ofParameter<float>	temperatureScale;
		ofParameter<bool>	lineSmooth;
		
		ofTexture*	temperatureTexture;
		ofMesh		fieldMesh;
		ofVbo		fieldVbo;
		
		ftTemperatureFieldShader temperatureFieldShader;
		
	};
}

