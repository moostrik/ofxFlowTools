
#pragma once

#include "ofMain.h"
#include "ftUtil.h"
#include "ftPixelFlow.h"

namespace flowTools {
	
	class ftAverageFlow {
	public:
		virtual void	setup(ftFlowForceType _type);
		virtual void	update(ofFloatPixels& _pixels);
		
		void		reset();
		
		void		setRoi(float _x, float _y, float _width, float _height) { setRoi(ofRectangle(_x, _y, _width, _height)); }
		void		setRoi(ofRectangle _rect);
		
		void		setNormalization(float _value)		{ pMagnitudeNormalization.set(_value); }
		void		setHighComponentBoost(float _value)	{ pHighComponentBoost.set(_value); }
		float		getNormalization()					{ return pMagnitudeNormalization.get(); }
		float		getHighComponentBoost()				{ return pHighComponentBoost.get(); }

		float		getMagnitude()				{ return magnitude; }
		vector<float>	getVelocity()			{ return velocity; }
		
		ofRectangle	getRoi()			{ return roi; }
		ofPoint		getRoiCentre()		{ return ofPoint(roi.x + roi.width / 2, roi.y + roi.height / 2); }
		
		void		draw(int _x, int _y, int _w, int _h);
		void		drawROI(int _x, int _y, int _w, int _h);
		void		drawVisualizer(int _x, int _y, int _w, int _h);
		
		ofParameterGroup&	getParameters() 	{ return parameters; }
		
	protected:
		
		ofParameterGroup	parameters;
		
		ftFlowForceType		type;
		int 				numChannels;
		
		int 				inputWidth;
		int 				inputHeight;
		
		
		static int			averageFlowCount;
		
		ofRectangle			roi;
		
		vector<float>		pixelMagnitudes;
		
		float				magnitude;
		
		vector<float>		velocity;
		
		void				getMeanStDev(vector<float> &_v, float &_mean, float &_stDev);
		
		void								setupParameters();
		ofParameter<float>					pMagnitudeNormalization;
		ofParameter<float>					pHighComponentBoost;
		ofParameter<bool>					pPauze;
		ofParameterGroup					roiParameters;
		vector< ofParameter<float> >		pRoi;
		ofParameter<float>					pMagnitude;
		ofParameterGroup					velocityParameters;
		vector< ofParameter<float> >		pVelocity;
		
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
		
		void allocate(int _inputWidth, int _inputHeight, GLint _inputInternalFormat, int _outputWidth, int _outputHeight, GLint _outputInternalFormat);
		
	};
}
