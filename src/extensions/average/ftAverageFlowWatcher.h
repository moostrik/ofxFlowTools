
#pragma once

#include "ofMain.h"
#include "ftAverageFlow.h"

namespace flowTools {
	
	class ftAverageFlowWatcher: public ftAverageFlow {
	public:
		
		void setup(int _width, int _height, ftFlowForceType _type) override;
		
		void update(ofFloatPixels& _pixels) override;
		
		int		getMagnitudeEvent()					{ return magnitudeEvent; } // 0 or 1 : off or on
		
		vector<int>	getComponentEvents()			{ return componentEvents; } // 0 or 1 or -1 : off, positive or negative
		int		getComponentEvent(int _index)		{ if (_index < numChannels) { return componentEvents[_index]; } else { return 0; } }
		
		float	getThreshold()						{ return pThreshold.get(); }
		float	getBase()							{ return pBase.get(); }
		
		void	setThreshold(float _value)			{ pThreshold.set(_value); }
		void	setBase(float _value)				{ pBase.set(_value); }
	protected:
		ofParameterGroup	eventParameters;
		bool				magnitudeEvent;
		float				magnitudeActiveHigh;
		float		 		magnitudeInActiveLow;
		
		vector<int>			componentEvents;
		vector<float>		componentActiveHighs;
		vector<float>		componentInActiveLows;
		
		ofParameter<float>	pThreshold;
		ofParameter<float>	pBase;
		ofParameter<bool>	pMagnitudeEvent;
		vector< ofParameter<bool> >	pComponentEvents;
		
		ofMesh				baseMesh;
		ofFloatColor		baseColor, thresholdColor;
		
		void	drawGraph(int _x, int _y, int _w, int _h) override;
		void	drawGraphOverlay(int _x, int _y, int _w, int _h) override;
		
	};
}

