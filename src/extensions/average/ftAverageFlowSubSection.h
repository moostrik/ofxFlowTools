
#pragma once

#include "ofMain.h"
#include "ftAverageFlow.h"

namespace flowTools {
	
	class ftAverageFlowSubSection: public ftAverageFlow {
	public:
		
		void setup(int _width, int _height, ftFlowForceType _type) override;
		void update(ofRectangle _subSection);
		void update() override ;
		
	protected:
		ofFloatPixels	subPixels;
		bool bPixelsUpdated;
		void resetPixelsUpdated(ofEventArgs& eventArgs) { bPixelsUpdated = false; }
		
		void getSubSection(ofFloatPixels& _srcPixels, ofFloatPixels& _dstPixels, ofRectangle _section);
		
		void inRange(ofRectangle& _srcRect, ofRectangle _rangeRect = ofRectangle(0,0,1,1));
	};
}

