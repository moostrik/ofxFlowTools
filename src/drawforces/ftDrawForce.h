#pragma once

#include "ofMain.h"
#include "ftFbo.h"
#include "ftDrawForceShader.h"


namespace flowTools {
	
	enum ftDrawForceType{
		FT_NONE			= 0,
		FT_DENSITY		= 1,
		FT_VELOCITY		= 2,
		FT_TEMPERATURE	= 3,
		FT_PRESSURE		= 4,
		FT_OBSTACLE		= 5,
	};
	
	class ftDrawForce {
	public:
		
		ftDrawForce();
		
		~ftDrawForce(){;}
		
		void setup(int _width, int _height, ftDrawForceType _type, bool _isTemporary) ;
		void applyForce(ofVec2f _normalizedPosition);
		void update();
		void reset();
		
		void setName(string _name) {parameters.setName(_name);}
		void setType(ftDrawForceType _type) { type = _type ;}
		void setIsTemporary(bool _isTemporary) { reset(); isTemporary.set(_isTemporary); }
		void setRadius(float _radius) { radius.set(_radius); }
		void setEdge(float _edge) { edge.set(_edge) ;}
		void setStrength(float _strength) { strength = _strength; if (!isTemporary) forceChanged = true;}
		void setForce(float _force) {force = ofVec4f(_force, 0.0, 0.0, 1.0);}
		void setForce(ofVec2f _force) {force = ofVec4f(_force.x, _force.y, 0.0, 1.0);}
		void setForce(ofVec3f _force) {force = ofVec4f(_force.x, _force.y, _force.z, 1.0);}
		void setForce(ofVec4f _force) {force = ofVec4f(_force.x, _force.y, _force.z, _force.w);}
		void setForce(ofFloatColor _force) {force = ofVec4f(_force.r, _force.g, _force.b, _force.a);}
		void setForce(ofColor _force) {force = ofVec4f(_force.r / 255.0, _force.g / 255.0, _force.b / 255.0, _force.a / 255.0);}
		
		int		getWidth() {return width;};
		int		getHeight() {return height;};
		
		ftDrawForceType getType() { return type ;}
		bool	getIsTemporary() { return isTemporary.get(); }
		float	getRadius() { return radius.get(); }
		float	getEdge() { return edge.get(); }
		float	getStrength() { return strength.get(); }
		ofVec4f	getForce() { return force.get(); }
		ofTexture&	getTexture() {return forceBuffer.getTexture();}
		bool	didChange() {return forceChanged;}
		
		ofParameterGroup	parameters;
		
	protected:
		ftDrawForceType  type;
		ofParameter<int>	drawType;
		ofParameter<bool>	isTemporary;
		ofParameter<ofVec4f> force;
		ofParameter<float>	strength;
		ofParameter<float>	radius;
		ofParameter<float>	edge;
		ofParameter<bool>	doReset;
		
		ftDrawForceShader	drawForceShader;
		
		ftFbo				forceBuffer;
		
		
		ofParameter<ofFloatColor>	density;
		ofParameter<ofVec2f>		velocity;
		ofParameter<float>			temperature;
		ofParameter<float>			pressure;
		ofParameter<bool>			obstacle;
		
		int width;
		int height;
		
		ofVec2f	absolutePosition;
		float	absoluteRadius;
		
		bool forceChanged;
		bool forceApplied;
		
		void saveValue(ftDrawForceType _type, ofVec4f _force);
		
		void resetonTempSwitch(bool &_value) {if(_value) reset(); }
	};
}

