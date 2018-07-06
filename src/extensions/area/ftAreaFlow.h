
#pragma once

#include "ofMain.h"
#include "ftUtil.h"
#include "ftFlow.h"

namespace flowTools {
	
	class ftAreaFlow : public ftFlow {
	public:
		virtual void setup(int _width, int _height, ftFlowForceType _type);
		virtual void update();
		
		void draw(int _x, int _y, int _w, int _h) override;	
		
		void	setInput(ofTexture &_tex) override { ftUtil::roi(inputFbo, _tex, roi); bInputSet = true; }
		void	addInput(ofTexture &_tex, float _strength = 1.0) override;
		
		void	setRoi(float _x, float _y, float _width, float _height) { setRoi(ofRectangle(_x, _y, _width, _height)); }
		void	setRoi(ofRectangle _rect);

		float	getComponent(int _index) 	{ getVelocity(_index); }
		float	getVelocity(int _index) 	{ if (_index < numChannels) { return velocity[_index];  } else { return 0; } }
		float	getDirection(int _index)	{ if (_index < numChannels) { return direction[_index];  } else { return 0; } }
		float	getMagnitude()				{ return getMeanMagnitude(); }
		float	getMeanMagnitude()			{ return meanMagnitude; }
		float	getStDevMagnitude()			{ return stdevMagnitude; }
//
		int		getNumChannels()			{ return numChannels; }
		int		getWidth()					{ return inputWidth; }
		int		getHeight()					{ return inputHeight; }

		ofRectangle		getRoi()			{ return roi; }
		ofPoint			getRoiCentre()		{ return ofPoint(roi.x + roi.width / 2, roi.y + roi.height / 2); }
		
	protected:
		ofParameterGroup 					roiParameters;
		ofParameterGroup 					componentParameters;
		ofParameterGroup 					directionParameters;
		ofParameter<float>					pMeanMagnitude;
		ofParameter<float>					pStdevMagnitude;
		vector< ofParameter<float> > 		pRoi;
		vector< ofParameter<float> > 		pComponents;
		vector< ofParameter<float> > 		pDirection;
		void pRoiListener(float& _value) 	{ setRoi(pRoi[0], pRoi[1], pRoi[2], pRoi[3]); }
		
		ftFlowForceType type;
		int 			numChannels;
		static int		areaCount;
		
		ofFloatPixels 	inputPixels;
		ofRectangle 	roi;
		
		float			meanMagnitude;
		float			stdevMagnitude;
		vector<float>	magnitudes;
		vector<float> 	direction;
		vector<float> 	velocity;
		
		ofFbo			roiFbo;
		
		void allocate(int _width, int _height, int _numChannels);
		void getMeanStDev(vector<float> &_v, float &_mean, float &_stDev);
		
		
	};
}
