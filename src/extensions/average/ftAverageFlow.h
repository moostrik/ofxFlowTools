
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
		
		void		reset() override;
		
		void		setRoi(float _x, float _y, float _width, float _height) { setRoi(ofRectangle(_x, _y, _width, _height)); }
		void		setRoi(ofRectangle _rect);
		
		void		setNormalization(float _value)	{ pMagnitudeNormalization.set(_value); }
		void		setHighComponentBoost(float _value)	{ pHighComponentBoost.set(_value); }

		float		getMagnitude()				{ return magnitude; }
//		float		getMagnitudeArea()			{ return magnitudeArea; }
		vector<float>	getVelocity()			{ return velocity; }
//		vector<float>	getVelocityArea()		{ return velocityArea; }

		int			getNumChannels()			{ return numChannels; }
		int			getWidth()					{ return inputWidth; }
		int			getHeight()					{ return inputHeight; }
		
		float		getNormalization()			{ return pMagnitudeNormalization.get(); }
		float		getHighComponentBoost()		{ return pHighComponentBoost.get(); }
		
		ofRectangle	getRoi()			{ return roi; }
		ofPoint		getRoiCentre()		{ return ofPoint(roi.x + roi.width / 2, roi.y + roi.height / 2); }
		
		void		drawOutput(int _x, int _y, int _w, int _h) override;
		void		drawROI(int _x, int _y, int _w, int _h);
		void		drawVisualizer(int _x, int _y, int _w, int _h);
		
	protected:
		static int		averageFlowCount;
		ftFlowForceType	type;
		int 			numChannels;
		
		ofFloatPixels	roiPixels;
		ofRectangle		roi;
		void			getRoiData(ofFloatPixels& _srcPixels, ofFloatPixels& _roiPixels, ofRectangle _rect);
		
		vector<float>	pixelMagnitudes;
		
		float			magnitude;
//		float			magnitudeArea;
		
		vector<float>	velocity;
//		vector<float>	velocityArea;
		
		void			getMeanStDev(vector<float> &_v, float &_mean, float &_stDev);
		
		void								setupParameters();
		ofParameter<float>					pMagnitudeNormalization;
		ofParameter<float>					pAreaNormalization;
		ofParameter<float>					pHighComponentBoost;
		ofParameter<bool>					pPauze;
		ofParameterGroup					roiParameters;
		vector< ofParameter<float> >		pRoi;
		ofParameter<float>					pMagnitude;
//		ofParameter<float>					pMagnitudeArea;
		ofParameterGroup					velocityParameters;
		vector< ofParameter<float> >		pVelocity;
//		ofParameterGroup					velocityAreaParameters;
//		vector< ofParameter<float> >		pVelocityArea;
		
		void pRoiListener(float& _value)	{ setRoi(pRoi[0], pRoi[1], pRoi[2], pRoi[3]); }
		
		// DRAW FUNCTIONS
		int 					graphSize;
		ofMesh					magnitudeMesh;
		vector< ofMesh >		velocityMeshes;
		
		ofFloatColor			magnitudeColor;
		vector<ofFloatColor>	velocityColors;
		bool 					bUpdateVisualizer;
		
		virtual void			setupDraw();
		virtual void			drawBackground(int _x, int _y, int _w, int _h);
		virtual void			drawGraph(int _x, int _y, int _w, int _h);
		virtual void			drawGraphOverlay(int _x, int _y, int _w, int _h);
		
	};
}
