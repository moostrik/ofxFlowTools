//
//  ftAverageFlowSubSection.cpp
//  example_extended
//
//  Created by Ties East on 19/09/2018.
//

#include "ftAverageFlowSubSection.h"

namespace flowTools {
	
	void ftAverageFlowSubSection::setup(int _width, int _height, flowTools::ftFlowForceType _type){
		ftAverageFlow::setup(_width, _height, _type);
		ofAddListener(ofEvents().update, this, &ftAverageFlowSubSection::resetPixelsUpdated);
		bPixelsUpdated = false;
	}
	
	void ftAverageFlowSubSection::update(ofRectangle _subSection) {
		if (!bPixelsUpdated) {
			ftUtil::toPixels(inputFbo, inputPixels);
			bPixelsUpdated = true;
		}
		inRange(_subSection);
		getSubSection(inputPixels, subPixels, _subSection);
		compute(subPixels);
	}
	
	void ftAverageFlowSubSection::update() {
		if (!bPixelsUpdated) {
			ftUtil::toPixels(inputFbo, inputPixels);
			bPixelsUpdated = true;
		}
		compute(inputPixels);
	}
	
	void ftAverageFlowSubSection::getSubSection(ofFloatPixels& _srcPixels, ofFloatPixels& _dstPixels, ofRectangle _section) {
		int x = _section.x * _srcPixels.getWidth();
		int y = _section.y * _srcPixels.getWidth();
		int w = _section.width * _srcPixels.getWidth();
		int h = _section.height * _srcPixels.getHeight();
		int c = _srcPixels.getNumChannels();
		_dstPixels.allocate(w, h, c);
		float* srcPixelData = _srcPixels.getData();
		float* dstPixelData = _dstPixels.getData();
		
		for (int iY=0; iY<h; iY++) {
			for (int iX=0; iX<w; iX++) {
				for (int iC=0; iC<c; iC++) {
					int subI = ((iY * w) + iX) * c + iC;
					int srcI = (((y+iY) * _srcPixels.getWidth()) + (x+iX)) * c + iC;
					dstPixelData[subI] = srcPixelData[srcI];
				}
			}
		}
	}
	
	void ftAverageFlowSubSection::inRange(ofRectangle &_srcRect, ofRectangle _rangeRect) {
		_srcRect.x = ofClamp(_srcRect.x, _rangeRect.x, _rangeRect.width);
		_srcRect.y = ofClamp(_srcRect.y, _rangeRect.y, _rangeRect.height);
		_srcRect.width = ofClamp(_srcRect.width, 0, _rangeRect.width - _rangeRect.x);
		_srcRect.height = ofClamp(_srcRect.height, 0, _rangeRect.height - _rangeRect.y);
	}
}

