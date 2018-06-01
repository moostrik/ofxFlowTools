#pragma once

#include "ofMain.h"
#include "ftUtil.h"
#include "ftDrawForceShader.h"


namespace flowTools {
	
	
	class ftDrawForce {
	public:
		
		ftDrawForce();
		
		~ftDrawForce(){;}
		
		void setup(int _width, int _height, ftFlowType _type, bool _isTemporary) ;
		void applyForce(glm::vec2 _normalizedPosition);
		void update();
		void reset();
		
		void setName(string _name) {parameters.setName(_name);}
		void setType(ftFlowType _type) { type = _type ;}
		void setIsTemporary(bool _isTemporary) { reset(); isTemporary.set(_isTemporary); }
		void setRadius(float _radius) { radius.set(_radius); }
		void setEdge(float _edge) { edge.set(_edge) ;}
		void setStrength(float _strength) { strength = _strength; if (!isTemporary) forceChanged = true;}
		void setForce(float _force) {force = glm::vec4(_force, 0.0, 0.0, 1.0);}
		void setForce(glm::vec2 _force) {force = glm::vec4(_force.x, _force.y, 0.0, 1.0);}
		void setForce(glm::vec3 _force) {force = glm::vec4(_force.x, _force.y, _force.z, 1.0);}
		void setForce(glm::vec4 _force) {force = glm::vec4(_force.x, _force.y, _force.z, _force.w);}
		void setForce(ofFloatColor _force) {force = glm::vec4(_force.r, _force.g, _force.b, _force.a);}
		void setForce(ofColor _force) {force = glm::vec4(_force.r / 255.0, _force.g / 255.0, _force.b / 255.0, _force.a / 255.0);}
		
		int		getWidth() {return width;};
		int		getHeight() {return height;};
		
		ftFlowType getType() { return type ;}
		bool	getIsTemporary() { return isTemporary.get(); }
		float	getRadius() { return radius.get(); }
		float	getEdge() { return edge.get(); }
		float	getStrength() { return strength.get(); }
		glm::vec4	getForce() { return force.get(); }
		ofTexture&	getTexture() {return forceBuffer.getTexture();}
		bool	didChange() {return forceChanged;}
		
		ofParameterGroup	parameters;
		
	protected:
		ftFlowType  type;
		ofParameter<int>	drawType;
		ofParameter<bool>	isTemporary;
		ofParameter<glm::vec4> force;
		ofParameter<float>	strength;
		ofParameter<float>	radius;
		ofParameter<float>	edge;
		ofParameter<bool>	doReset;
		
		ftDrawForceShader	drawForceShader;
		
		ofFbo				forceBuffer;
		
		
		ofParameter<ofFloatColor>	density;
		ofParameter<glm::vec2>		velocity;
		ofParameter<float>			temperature;
		ofParameter<float>			pressure;
		ofParameter<bool>			obstacle;
		
		int width;
		int height;
		
		glm::vec2	absolutePosition;
		float	absoluteRadius;
		
		bool forceChanged;
		bool forceApplied;
		
		void saveValue(ftFlowType _type, glm::vec4 _force);
		
		void resetonTempSwitch(bool &_value) {if(_value) reset(); }
	};
}

