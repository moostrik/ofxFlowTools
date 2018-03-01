
#pragma once

#include "ftBaseArea.h"

namespace flowTools {
	
	void ftBaseArea::setTexture(ofTexture& _texture) {
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_DISABLED);
		ftUtil::black(scaleFbo);
		ftUtil::roi(scaleFbo, _texture, ofRectangle(pRoiX.get(), pRoiY.get(), pRoiWidth.get(), pRoiHeight.get()));
		ofPopStyle();
	}
	
	void ftBaseArea::setMask(ofTexture& _texture) {
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_MULTIPLY);
		ftUtil::stretch(scaleFbo, _texture);
		ofPopStyle();
	}
}
