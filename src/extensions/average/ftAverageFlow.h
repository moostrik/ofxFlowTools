
#pragma once

#include "ofMain.h"
#include "ftUtil.h"
#include "ftFlow.h"

namespace flowTools {
	
	class ftAverageFlow : public ftFlow {
	public:
		virtual void	setup(int _width, int _height, ftFlowForceType _type);
		virtual void	update(ofFloatPixels& _pixels);
		
		void		setInput(ofTexture &_tex) override { ; }
		void		addInput(ofTexture &_tex, float _strength = 1.0) override { ; }
		
		void		setRoi(float _x, float _y, float _width, float _height) { setRoi(ofRectangle(_x, _y, _width, _height)); }
		void		setRoi(ofRectangle _rect);
		
		float		setNormalizationMax(float _value)	{ pNormalizationMax.set(_value); }
		void		setHighComponentBoost(float _value)	{ pHighComponentBoost.set(_value); }

		float		getComponent(int _index)	{ if (_index < numChannels) { return components[_index];  } else { return 0; } }
		float		getDirection(int _index)	{ if (_index < numChannels) { return direction[_index];  } else { return 0; } }
		float		getMagnitude()				{ return getNormalizedMagnitude(); }
		float		getNormalizedMagnitude()	{ return normalizedMagnitude; }
		float		getMeanMagnitude()			{ return meanMagnitude; }		// not normalized
		float		getStDevMagnitude()			{ return stdevMagnitude; }		// not normalized
		float		getArea(int _index)			{ if (_index < numChannels) { return areas[_index]; } else { return 0; } }
		vector<float>	getComponents()			{ return components; }
		vector<float>	getAreas()				{ return areas; }

		int			getNumChannels()			{ return numChannels; }
		int			getWidth()					{ return inputWidth; }
		int			getHeight()					{ return inputHeight; }
		
		float		getNormalizationMax()		{ return pNormalizationMax.get(); }
		float		getHighComponentBoost()		{ return pHighComponentBoost.get(); }
		
		ofRectangle	getRoi()			{ return roi; }
		ofPoint		getRoiCentre()		{ return ofPoint(roi.x + roi.width / 2, roi.y + roi.height / 2); }
		
		void	drawOutput(int _x, int _y, int _w, int _h) override;
		void	drawROI(int _x, int _y, int _w, int _h);
		void	drawVisualizer(int _x, int _y, int _w, int _h);
		
	protected:
		static int		averageFlowCount;
		ftFlowForceType	type;
		int 			numChannels;
		
		ofFloatPixels	roiPixels;
		ofRectangle		roi;
		void			getRoiData(ofFloatPixels& _srcPixels, ofFloatPixels& _roiPixels, ofRectangle _rect);
		
		vector<float>	magnitudes;
		float			meanMagnitude;
		float			normalizedMagnitude;
		float			stdevMagnitude;
		vector<float>	direction;
		vector<float>	components;
		vector<float>	areas;
		void			getMeanStDev(vector<float> &_v, float &_mean, float &_stDev);
		
		void								setupParameters();
		ofParameterGroup					roiParameters;
		ofParameterGroup					componentParameters;
		ofParameterGroup					areaParameters;
		ofParameter<float>					pNormalizedMagnitude;
		ofParameter<float>					pStdevMagnitude;
		ofParameter<float>					pNormalizationMax;
		ofParameter<float>					pHighComponentBoost;
		ofParameter<bool>					pPauze;
		vector< ofParameter<float> >		pAreas;
		vector< ofParameter<float> >		pRoi;
		vector< ofParameter<float> >		pComponents;
		vector< ofParameter<float> >		pDirection;
		void pRoiListener(float& _value)	{ setRoi(pRoi[0], pRoi[1], pRoi[2], pRoi[3]); }
		
		
		
		
		
		
		float			prevNormalizedMagnitude;
		vector<float>	prevComponents;
		
		ofFloatColor			magnitudeColor;
		vector<ofFloatColor>	componentColors;
		ofFbo					overlayFbo;
		bool 					bUpdateVisualizer;
		
		virtual void	drawBackground(int _x, int _y, int _w, int _h);
		virtual void	drawGraph(int _x, int _y, int _w, int _h);
		void			createGraphOverlay(int _w, int _h);
		
	};
}
