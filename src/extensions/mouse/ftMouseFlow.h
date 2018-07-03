#pragma once

#include "ofMain.h"
#include "ftUtil.h"
#include "ftFlow.h"
#include "ftMouseShader.h"


namespace flowTools {
	
	
	class ftMouseFlow : public ftFlow{
	public:
		
		ftMouseFlow();
		~ftMouseFlow();
		
		void setup(int _width, int _height, ftFlowForceType _type);
		void update(float _deltaTime);
		void reset();
		
		bool				didChange()			{ return bFlowChanged;}
		ofTexture&			getTexture()		{ return outputFbo.getTexture();}
		
		ftFlowForceType 	getType()			{ return type; }
		bool				getPersistent()		{ return pPersistent.get(); }
		float				getRadius()			{ return pRadius.get(); }
		float				getSmooth()			{ return pSmooth.get(); }
		float				getSpeed()			{ return pSpeed.get(); }
		
		void setPersistent(bool _value)			{ pPersistent.set(_value); }
		void setRadius(float _value)			{ pRadius.set(_value); }
		void setSmooth(float _value)			{ pSmooth.set(_value) ;}
		void setSpeed(float _value)				{ pSpeed.set(_value); }
		
		ofParameterGroup&	getParameters()		{ return parameters; }
		void setName(string _name)				{ parameters.setName(_name); }
		
	protected:
		ofParameterGroup		parameters;
		ofParameter<bool>		pPersistent;
		
		ofParameter<float>		pSpeed;
		ofParameter<float>		pRadius;
		ofParameter<float>		pSmooth;
		ofParameter<bool>		pInverse;
		ofParameter<ofFloatColor>	pColor;
		
		void pTypeListener(int &_value);
		void pPersistentListener(bool &_value) { reset(); }
		
		void drawForce(glm::vec2 _startPosition, glm::vec2 _endPosition);
		
		ftFlowForceType  	type;
		ftMouseShader		mouseShader;
		
		glm::vec4	force;
		
		
		void		mouseMoved(ofMouseEventArgs & mouse);
		void		mouseDragged(ofMouseEventArgs & mouse);
		
		glm::vec2	mousePositions[2];
		bool		mps;
		bool		mousePositionsSet;
		
		bool		bMouseDragged;
		bool		bFlowChanged;
		
		void 		saveValue(ftFlowForceType _type, glm::vec4 _force);
		glm::vec4	loadValue(ftFlowForceType _type);
		
	};
}

