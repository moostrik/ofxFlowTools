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
		
		void setName(string _name) 				{parameters.setName(_name);}
		
		void setType(ftFlowForceType _type) 	{ pType.set(_type) ;}
		void setIsTemporary(bool _isTemporary) 	{ reset(); pIsTemporary.set(_isTemporary); }
		void setRadius(float _radius) 			{ radius.set(_radius); }
		void setEdge(float _edge) 				{ edge.set(_edge) ;}
		void setStrength(float _strength)		{ strength = _strength; if (!pIsTemporary) bHasChanged = true; }
		
		
		ftFlowForceType 	getType() 			{ return type; }
		bool				getIsTemporary() 	{ return pIsTemporary.get(); }
		float				getRadius() 		{ return radius.get(); }
		float				getEdge() 			{ return edge.get(); }
		float				getStrength()		{ return strength.get(); }
		
		bool				didChange() 		{ return bHasChanged;}
//		glm::vec4			getForce() 			{ return force.get(); }
		ofTexture&			getTexture() 		{ return outputFbo.getTexture();}
		
		ofParameterGroup&	getParameters() 	{ return parameters; }
		
		
		void applyForce(glm::vec2 _normalizedPosition);
		
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

