#pragma once

#include "ofMain.h"
#include "ftUtil.h"
#include "ftFlow.h"
#include "ftMouseShader.h"


namespace flowTools {
	
	
	class ftBaseMouseFlow : public ftFlow{
	public:
		
		ftBaseMouseFlow();
		~ftBaseMouseFlow(){;}
		
		void setup(int _width, int _height);
		void update();
		void reset();
		
		bool				didChange()			{ return bFlowChanged;}
		ofTexture&			getTexture()		{ return outputFbo.getTexture();}
		
		ftFlowForceType 	getType()			{ return type; }
		bool				getIsTemporary()	{ return pIsTemporary.get(); }
		float				getRadius()			{ return radius.get(); }
		float				getEdge()			{ return edge.get(); }
		float				getStrength()		{ return strength.get(); }
		
		void setIsTemporary(bool _value)		{ reset(); pIsTemporary.set(_value); }
		void setRadius(float _value)			{ radius.set(_value); }
		void setEdge(float _value)				{ edge.set(_value) ;}
		void setStrength(float _value)			{ strength = _value; if (!pIsTemporary) bStrengthUpdated = true; }
		
		ofParameterGroup&	getParameters()		{ return parameters; }
		void setName(string _name)				{ parameters.setName(_name); }
		
	protected:
		ofParameterGroup		parameters;
		ofParameter<bool>		pIsTemporary;
		
		ofParameter<float>		strength;
		ofParameter<float>		radius;
		ofParameter<float>		edge;
		ofParameter<bool>		doReset;
		
		void pTypeListener(int &_value);
		void pIsTemporaryListener(bool &_value) { reset(); }
		void pForceListener(glm::vec4 &_value);
		void pColorListener(ofFloatColor &_value) { _value = ofFloatColor(1,0,0,1); }
		
		void drawForce(glm::vec2 _startPosition, glm::vec2 _endPosition);
		
		ftFlowForceType  	type;
		ftMouseShader		mouseShader;
		
		glm::vec4	force;
		
		void		mouseMoved(ofMouseEventArgs & mouse);
		void		mouseDragged(ofMouseEventArgs & mouse);
		
		glm::vec2	mousePositions[2];
		bool		mps;
		bool		mousePositionsSet;
		
		bool		bDraw;
		bool		bStrengthUpdated;
		bool		bFlowChanged;
		
		void 		saveValue(ftFlowForceType _type, glm::vec4 _force);
		glm::vec4	loadValue(ftFlowForceType _type);
		
	};
}

