
#pragma once

#include "ofMain.h"
#include "ftSplitVelocityFieldShader.h"

namespace flowTools {
	
	class ftSplitVelocityField {
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
			
			parameters.setName("velocity field");
			parameters.add(velocityScale.set("velocity scale", .1, 0, 2));
			parameters.add(maxArrowLength.set("max arrow size", 1, 0, 10));
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
			
			ofTranslate(_x, _y);
			ofScale(_width, _height);
			
			float NormalizedMaxArrowLength =  (2.0 / (width + 1)) * maxArrowLength.get();
			splitVelocityFieldShader.update(fieldVbo, *velocityTexture, velocityScale.get(), NormalizedMaxArrowLength, color.get());
			
			if (lineSmooth.get()) {
				glDisable(GL_LINE_SMOOTH);
			}
			
			ofEnableAntiAliasing();
			ofPopStyle();
			ofPopMatrix();
		}
		
		void	setVelocity(ofTexture& tex)			{ velocityTexture = &tex; }
		void	setScale(float _value)				{ setVelocityScale(_value); }
		void	setVelocityScale(float _value)		{ velocityScale.set(_value); }
		void	setLineSmooth(bool _value)			{ lineSmooth.set(_value); }
		void	setColor(ofFloatColor _value)		{ color.set(_value); }
		void	setMaxArrowLength(float _value)		{ maxArrowLength.set(_value); }
		
		float	getVelocityScale()					{ return velocityScale.get(); }
		bool	getLineSmooth()						{ return lineSmooth.get(); }
		int		getWidth()							{ return width; }
		int		getHeight()							{ return height; }
		ofFloatColor	getColor()					{ return color.get(); }
		void	getMaxArrowLenth()					{ return maxArrowLength.get(); }
		
		ofParameterGroup	parameters;
		
	protected:
		int		width;
		int		height;
		
		ofParameter<float>	velocityScale;		// scale to normalize velocity
		ofParameter<bool>	lineSmooth;
		ofParameter<float>	maxArrowLength;
		ofParameter<ofFloatColor>	color;

		
		ofTexture*	velocityTexture;
		ofMesh		fieldMesh;
		ofVbo		fieldVbo;
		
		ftSplitVelocityFieldShader splitVelocityFieldShader;
		
	};
}

