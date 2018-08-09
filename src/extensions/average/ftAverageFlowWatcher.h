
#pragma once

#include "ofMain.h"
#include "ftAverageFlow.h"

namespace flowTools {
	
	class ftAverageFlowWatcher: public ftAverageFlow {
	public:
		
		void setup(int _width, int _height, ftFlowForceType _type) override;
		
		void update() override;
		
		bool	getMagnitudeEvent()					{ return magnitudeEvent; }
		
		vector<int>	getComponentEvents()			{ return componentEvents; }
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
		
		ofFloatColor		baseColor, thresholdColor;
		
		void	drawGraph(int _x, int _y, int _w, int _h) override;
		void	createGraphOverlay(int _w, int _h);
		
	};
}

