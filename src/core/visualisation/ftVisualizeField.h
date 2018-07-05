
#pragma once

#include "ofMain.h"
#include "ftVisualize.h"
#include "ftVelocityFieldShader.h"
#include "ftTemperatureFieldShader.h"

namespace flowTools {
	
	class ftVisualizeField : public ftVisualize {
	public:
		ftVisualizeField() {
			parameters.add(pToggleScalar.set("toggle scalar", false));
		}
		
		void setup(int _width, int _height) override {
			setup(_width / 2, _height / 2, _width, _height);
		}

		void setup(int _fieldWidth, int _fieldHeight, int _scalarWidth, int _scalarHeight) {
			setupField(_fieldWidth, _fieldHeight);
			setupScalar(_scalarWidth, _scalarHeight);
		}

		void setupField(int _width, int _height) {
			fieldWidth = _width;
			fieldHeight = _height;
			
			fieldMesh.clear();
			fieldMesh.setMode(OF_PRIMITIVE_POINTS);
			float xStep = 1. / fieldWidth;
			float yStep = 1. / fieldHeight;
			for (int x=0; x<fieldWidth; x++) {
				for (int y=0; y<fieldHeight; y++) {
					fieldMesh.addVertex(glm::vec3((x + 0.5) * xStep, (y + 0.5) * yStep, 0));
				}
			}
			fieldVbo.clear();
			fieldVbo.setMesh(fieldMesh, GL_DYNAMIC_DRAW, false, false, false);
			
			fieldColor = ofFloatColor(1,1,1,1);
		}
		
		void setupScalar(int _width, int _height) {
			scalarWidth = _width;
			scalarHeight = _height;
			displayScalarFbo.allocate(scalarWidth, scalarHeight, GL_RGBA);
		}
		
		void draw(ofTexture _tex, int _x, int _y, int _width, int _height) override {
			ofPushStyle();
			if (pToggleScalar) {
				ftUtil::zero(displayScalarFbo);
				displayScalarShader.update(displayScalarFbo, _tex, pScale.get());
				displayScalarFbo.draw(_x, _y, _width, _height);
			} else {
				ofPushMatrix();
				ofTranslate(_x, _y);
				ofScale(_width, _height);
				int numChannels = ftUtil::getNumChannelsFromInternalFormat(_tex.getTextureData().glInternalFormat);
				if (numChannels == 1) {
					float barHeight = _height / (float)fieldHeight;
					float barWidth = _width / (float)fieldWidth * 0.25;
					temperatureFieldShader.update(fieldVbo, _tex, pScale.get(), barHeight, barWidth);
				} else {
					float arrowLength = _width / (float)fieldWidth;
					velocityFieldShader.update(fieldVbo, _tex, (1.0 / _width) * arrowLength * pScale.get(), fieldColor);
				}
				ofPopMatrix();
			}
			ofPopStyle();
		}
		
		
		int		getFieldWidth()				{ return fieldWidth; }
		int		getFieldHeight()			{ return fieldHeight; }
		
		void 	setToggleScalar(int _value) { pToggleScalar.set(_value); }
		bool 	getToggleScalar() 			{ return pToggleScalar.get(); }
		
		ofParameterGroup&	getParameters() { return parameters; }
		
	protected:
		ofParameter<bool>	pToggleScalar;
		
		int					fieldWidth;
		int					fieldHeight;
		ofMesh				fieldMesh;
		ofVbo				fieldVbo;
		ofFloatColor		fieldColor;
		
		ftTemperatureFieldShader 	temperatureFieldShader;
		ftVelocityFieldShader		velocityFieldShader;
		
	};
}

