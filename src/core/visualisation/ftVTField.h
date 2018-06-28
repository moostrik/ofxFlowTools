
#pragma once

#include "ofMain.h"
#include "ftVTFieldShader.h"

namespace flowTools {
	
	class ftVTField {
	public:
		ftVTField() {
			parameters.setName("velocity field");
			parameters.add(isActive.set("active", true));
			parameters.add(velocityScale.set("velocity scale", .1, 0, 2));
			parameters.add(temperatureScale.set("temperature scale", .1, 0, 2));
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
		
		void	draw(ofTexture &velTex, ofTexture &temTex, int _x, int _y, int _width, int _height, bool _antiAlias = true) {
			
			ofPushMatrix();
			ofPushStyle();
			
			ofEnableAlphaBlending();
			if (_antiAlias) { ofEnableAntiAliasing; }
			else { ofDisableAntiAliasing(); }
			
			if (lineSmooth.get()) {
				glEnable(GL_LINE_SMOOTH);
			}
			
			ofTranslate(_x, _y);
			ofScale(_width, _height);
			float maxArrowLength =  2.0 / (width + 1);
			velocityTemperatureFieldShader.update(fieldVbo, velTex, temTex, velocityScale.get(), temperatureScale.get(), maxArrowLength);
			
			if (lineSmooth.get()) {
				glDisable(GL_LINE_SMOOTH);
			}
			
			ofPopStyle();
			ofPopMatrix();
		}
		
		void	setActive(bool _value)				{ isActive.set(_value); }
		void	setVelocityScale(float _value)		{ velocityScale.set(_value); }
		void	setTemperatureScale(float _value)	{ temperatureScale.set(_value); }
		void	setLineSmooth(bool _value)			{ lineSmooth.set(_value); }
		
		bool	getActive()							{ return isActive.get(); }
		int		getWidth()							{ return width; }
		int		getHeight()							{ return height; }
		float	getVelocityScale()					{ return velocityScale.get(); }
		float	getTemperatureScale()				{ return temperatureScale.get(); }
		bool	getLineSmooth()						{ return lineSmooth.get(); }
		
		ofParameterGroup	parameters;
		
	protected:
		int		width;
		int		height;
		
		ofParameter<bool> 	isActive;
		ofParameter<float>	velocityScale;		// scale to normalize velocity
		ofParameter<float>	temperatureScale;	// scale to normalize temperature
		ofParameter<bool>	lineSmooth;
		
		ofMesh		fieldMesh;
		ofVbo		fieldVbo;
		
		ftVTFieldShader velocityTemperatureFieldShader;
		
	};
}

