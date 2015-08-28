#pragma once

#include "ofMain.h"
#include "ftFbo.h"
#include "ftDrawForce.h"


namespace flowTools {
	
	class ftDrawMouseForces {
		
	public:
		ftDrawMouseForces();
        ~ftDrawMouseForces();
	
		void				setup(int _simulationWidth, int _simulationHeight, int _densityWidth = 0, int _densityHeight = 0) ;
		void				update(float _deltaTime);
		void				reset()  { for (int i=0; i<numDrawForces; i++) drawForces[i].reset(); }
		
		int					getNumForces() { return numDrawForces; }
		
		bool				didChange(int _index);
		ftDrawForceType		getType(int _index);
		ofTexture&			getTextureReference(int _index);
		float				getStrength(int _index);
		
//		ofParameterGroup	parameters;
		ofParameterGroup	leftButtonParameters;
		ofParameterGroup	rightButtonParameters;
		
	private:
		ofParameter<bool>	doResetDrawForces;
		void				resetDrawForcesListner(bool& _value) { if (_value) { reset(); }; doResetDrawForces.set(false); }
		
		int					numDrawForces;
		ftDrawForce*		drawForces;
		
		float				deltaTime;
		
		int					simulationWidth;
		int					simulationHeight;
		int					densityWidth;
		int					densityHeight;
		
		void				mouseMoved(ofMouseEventArgs & mouse);
		void				mouseDragged(ofMouseEventArgs & mouse);
		
		ofVec2f				lastNormalizedMouse;
		
	};
}