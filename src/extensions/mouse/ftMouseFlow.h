#pragma once

#include "ofMain.h"
#include "ftUtil.h"
#include "ftFlow.h"
#include "ftMouseShader.h"


namespace flowTools {
	
	
	class ftMouseFlow : public ftFlow{
	public:
		
		ftMouseFlow();
		~ftMouseFlow(){;}
		
		void setup(int _width, int _height, ftFlowForceType _type, bool _isTemporary = true) ;
		void update();
		void reset();
		
		bool				didChange() 		{ return bHasChanged;}
		ofTexture&			getTexture() 		{ return outputFbo.getTexture();}
		
		ftFlowForceType 	getType() 			{ return type; }
		bool				getIsTemporary() 	{ return pIsTemporary.get(); }
		glm::vec4			getForce() 			{ return pForce.get(); }
		float				getRadius() 		{ return radius.get(); }
		float				getEdge() 			{ return edge.get(); }
		float				getStrength()		{ return strength.get(); }
		
		void setType(ftFlowForceType _value) 	{ pType.set(_value) ;}
		void setIsTemporary(bool _value) 		{ reset(); pIsTemporary.set(_value); }
		void setForce(glm::vec4 _value) 		{ pForce.set(_value); }
		void setRadius(float _value) 			{ radius.set(_value); }
		void setEdge(float _value) 				{ edge.set(_value) ;}
		void setStrength(float _value)			{ strength = _value; if (!pIsTemporary) bHasChanged = true; }
		
		ofParameterGroup&	getParameters() 	{ return parameters; }
		void setName(string _name) 				{ parameters.setName(_name); }
		
	protected:
		ofParameterGroup		parameters;
		ofParameter<int>		pType;
		ofParameter<string>		pTypeName;
		ofParameter<bool>		pIsTemporary;
		ofParameter<glm::vec4> 	pForce;
		ofParameter<float>		strength;
		ofParameter<float>		radius;
		ofParameter<float>		edge;
		ofParameter<bool>		doReset;
		
		void pTypeListener(int &_value);
		void pIsTemporaryListener(bool &_value) { reset(); }
		void pForceListener(glm::vec4 &_value);
		
		void applyForce(glm::vec2 _normalizedPosition);
		
		ftFlowForceType  	type;
		ftMouseShader		mouseShader;
		
		glm::vec4	density;
		glm::vec4	input;
		glm::vec4	velocity;
		glm::vec4	temperature;
		glm::vec4	pressure;
		glm::vec4	obstacle;
		
		void		mouseMoved(ofMouseEventArgs & mouse);
		void		mouseDragged(ofMouseEventArgs & mouse);
		
		glm::vec2	lastNormalizedMouse;
		bool		lastNormalizedMouseSet;
		
		
		bool		bHasChanged;
		bool		bForceSet;
		bool		bForceApplied;
		
		void 		saveValue(ftFlowForceType _type, glm::vec4 _force);
		glm::vec4	loadValue(ftFlowForceType _type);
		
	};
}

