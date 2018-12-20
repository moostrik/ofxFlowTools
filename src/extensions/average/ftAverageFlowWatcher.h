
#pragma once

#include "ofMain.h"
#include "ftAverageFlow.h"

namespace flowTools {
	
	class ftAverageFlowWatcher: public ftAverageFlow {
	public:
		
		void setup(int _width, int _height, ftFlowForceType _type) override;
		
		void update(ofFloatPixels& _pixels) override;
		
		int		getMagnitudeEvent()					{ return magnitudeEvent; } // 0 or 1 : off or on
		
		vector<int>	getVelocityEvents()			{ return velocityEvents; } // 0 or 1 or -1 : off, positive or negative
		int		getVelocityEvent(int _index)		{ if (_index < numChannels) { return velocityEvents[_index]; } else { return 0; } }
		
		float	getThreshold()						{ return pThreshold.get(); }
		float	getBase()							{ return pBase.get(); }
		
		void	setThreshold(float _value)			{ pThreshold.set(_value); }
		void	setBase(float _value)				{ pBase.set(_value); }
	protected:
		ofParameterGroup	eventParameters;
		bool				magnitudeEvent;
		float				magnitudeHigh;
		float		 		magnitudeLow;
		
		vector<int>			velocityEvents;
		vector<float>		velocityHighs;
		vector<float>		velocityLows;
		
		ofParameter<float>	pThreshold;
		ofParameter<float>	pBase;
		ofParameter<bool>	pMagnitudeEvent;
		vector< ofParameter<bool> >	pVelocityEvents;
		
		ofMesh				baseMesh;
		ofFloatColor		baseColor, thresholdColor;
		ofMesh				magnitudeEventMesh;
		vector< ofMesh >	velocityEventMeshes;
		
		void 	setupDraw() override;
		void	drawGraph(int _x, int _y, int _w, int _h) override;
		void	drawGraphOverlay(int _x, int _y, int _w, int _h) override;
		
	};
}

