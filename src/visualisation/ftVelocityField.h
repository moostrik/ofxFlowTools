
#pragma once

#include "ofMain.h"
#include "ftVelocityFieldShader.h"

namespace flowTools {
	
	class ftVelocityField {
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
			float maxArrowLength =  2.0 / (width + 1);
			velocityFieldShader.update(fieldVbo, *velocityTexture, velocityScale.get(), maxArrowLength);
			
			if (lineSmooth.get()) {
				glDisable(GL_LINE_SMOOTH);
			}
			
			ofEnableAntiAliasing();
			ofPopStyle();
			ofPopMatrix();
		}
		
		void	setVelocity(ofTexture& tex)			{ velocityTexture = &tex; }
		void	setVelocityScale(float _value)		{ velocityScale.set(_value); }
		void	setLineSmooth(bool _value)			{ lineSmooth.set(_value); }
		
		float	getVelocityScale()					{ return velocityScale.get(); }
		bool	getLineSmooth()						{ return lineSmooth.get(); }
		int		getWidth()							{ return width; }
		int		getHeight()							{ return height; }
		
		ofParameterGroup	parameters;
		
	protected:
		int		width;
		int		height;
		
		ofParameter<float>	velocityScale;		// scale to normalize velocity
		ofParameter<bool>	lineSmooth;

		
		ofTexture*	velocityTexture;
		ofMesh		fieldMesh;
		ofVbo		fieldVbo;
		
		ftVelocityFieldShader velocityFieldShader;
		
	};
}

