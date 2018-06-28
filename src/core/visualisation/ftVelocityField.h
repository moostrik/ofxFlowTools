
#pragma once

#include "ofMain.h"
#include "ftVelocityFieldShader.h"

namespace flowTools {
	
	class ftVelocityField {
	public:
		ftVelocityField() {
			parameters.setName("velocity field");
			parameters.add(isActive.set("active", true));
			parameters.add(velocityScale.set("velocity scale", 1, 0, 10));
			//			parameters.add(arrowSize.set("arrow size", 4, 1, 8));
			arrowSize.set("arrow size", 4, 1, 8);
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
		
		void	draw(ofTexture& _velTex, int _x, int _y, int _width, int _height, ofBlendMode _blendmode = OF_BLENDMODE_ALPHA) {
			if (isActive.get()) {
				ofPushMatrix();
				ofPushStyle();
				
				//			ofDisableAntiAliasing();
				//			if (lineSmooth.get()) {
				//				glEnable(GL_LINE_SMOOTH);
				//			}
				
				ofTranslate(_x, _y);
				ofScale(_width, _height);
				
				velocityFieldShader.update(fieldVbo, _velTex, (1.0 / width) * arrowSize.get() * velocityScale.get(), color.get());
				
				//			if (lineSmooth.get()) {
				//				glDisable(GL_LINE_SMOOTH);
				//			}
				
				ofPopStyle();
				ofPopMatrix();
			}
		}
		
		void	setActive(bool _value)				{ isActive.set(_value); }
		void	setScale(float _value)				{ setVelocityScale(_value); }
		void	setVelocityScale(float _value)		{ velocityScale.set(_value); }
		void	setLineSmooth(bool _value)			{ lineSmooth.set(_value); }
		void	setColor(ofFloatColor _value)		{ color.set(_value); }
		void	setArrowSize(float _value)			{ arrowSize.set(_value); }
		
		int		getWidth()							{ return width; }
		int		getHeight()							{ return height; }
		bool	getActive()							{ return isActive.get(); }
		float	getVelocityScale()					{ return velocityScale.get(); }
		bool	getLineSmooth()						{ return lineSmooth.get(); }
		ofFloatColor	getColor()					{ return color.get(); }
//		float	getMaxArrowLength()					{ return maxArrowLength.get(); }
		
		ofParameterGroup	parameters;
		
	protected:
		int		width;
		int		height;
		
		ofParameter<bool> 	isActive;
		ofParameter<float>	velocityScale;		// scale to normalize velocity
		ofParameter<bool>	lineSmooth;
		ofParameter<float>	arrowSize;
		ofParameter<ofFloatColor>	color;

		ofMesh		fieldMesh;
		ofVbo		fieldVbo;
		
		ftVelocityFieldShader velocityFieldShader;
		
	};
}

