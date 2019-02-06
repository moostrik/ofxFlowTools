#include "ftAverageFlow.h"

namespace flowTools {
	
	int ftAverageFlow::averageFlowCount = 0;
	
	//--------------------------------------------------------------
	void ftAverageFlow::setup(ftFlowForceType _type) {
		averageFlowCount++;
		type = _type;
		GLint internalFormat = ftUtil::getInternalFormatFromType(type);
		numChannels = ftUtil::getNumChannelsFromInternalFormat(internalFormat);
		
		roi = ofRectangle(0,0,1,1);
		velocity.resize(numChannels, 0);
		
		// move draw graph out of average;
		magnitudeColor = ofFloatColor(1, 1, 1, 1.);
		velocityColors.push_back(ofFloatColor(1.0, 0.0, 1.0, 1.));
		velocityColors.push_back(ofFloatColor(0.0, 1.0, 1.0, 1.));
		velocityColors.push_back(ofFloatColor(0.5, 1.0, 0.0, 1.));
		velocityColors.push_back(ofFloatColor(1.0, 0.5, 0.0, 1.));
		
		bUpdateVisualizer = false;
		
		setupParameters();
		setupDraw();
	}
	
	//--------------------------------------------------------------
	void ftAverageFlow::setupParameters() {
		
		string name = "average " + ftFlowForceNames[type];
		if (averageFlowCount > 1) name += " " + ofToString(averageFlowCount - 1);
		parameters.setName(name);
		parameters.add(pMagnitudeNormalization.set("mag normalization", .5, .01, 1));
		if (type == FT_VELOCITY_SPLIT) {
			parameters.add(pHighComponentBoost.set("boost directionality", 0, 0, 5));
		}	else { pHighComponentBoost.set(0); }
		parameters.add(pPauze.set("pauze", false));
		
		roiParameters.setName("region of interest");
		pRoi.resize(4);
		roiParameters.add(pRoi[0].set("x", 0, 0, 1));
		roiParameters.add(pRoi[1].set("y", 0, 0, 1));
		roiParameters.add(pRoi[2].set("width", 1, 0, 1));
		roiParameters.add(pRoi[3].set("height", 1, 0, 1));
		for (int i=0; i<4; i++) {
			pRoi[i].addListener(this, &ftAverageFlow::pRoiListener);
		}
		parameters.add(roiParameters);
		
		parameters.add(pMagnitude.set("magnitude", 0, 0, 1));
		
		pVelocity.resize(numChannels);
		if (numChannels > 1) {
			velocityParameters.setName("velocity");
			for (int i=0; i<numChannels; i++) {
				velocityParameters.add(pVelocity[i].set(ftUtil::getComponentName(type, i), 0, -1, 1));
			}
			parameters.add(velocityParameters);
		} else {
			parameters.add(pVelocity[0].set(ftUtil::getComponentName(type, 0), 0, -1, 1));
		}
	}
	
	//--------------------------------------------------------------

	void ftAverageFlow::update(ofFloatPixels& _pixels) {
		if (!pPauze.get()) {
			int pixelWidth = _pixels.getWidth();
			int pixelHeight = _pixels.getHeight();
			ofRectangle roiReal = ofRectangle(roi.x * pixelWidth, roi.y * pixelHeight, int(roi.width * pixelWidth), int(roi.height * pixelHeight));
			
			
			int numRoiPixels = roiReal.width * roiReal.height;
			float* pixelData = _pixels.getData();
			
			vector<float> totalVelocity;
			totalVelocity.resize(numChannels, 0);
			
			pixelMagnitudes.resize(numRoiPixels);
			
			for (int x=0; x<roiReal.width; x++) {
				for (int y=0; y<roiReal.height; y++) {
					int pixelIndex = (x + roiReal.x + (y + roiReal.y) * pixelWidth) * numChannels;
					int roiIndex = x + y * roiReal.width;
					
					float mag = 0;
					for (int c=0; c<numChannels; c++) {
						float vel = pixelData[pixelIndex + c];
						totalVelocity[c] += vel;
						mag += vel * vel;
					}
					pixelMagnitudes[roiIndex] = sqrt(mag);
				}
			}
			
			magnitude = accumulate(pixelMagnitudes.begin(), pixelMagnitudes.end(), 0.0) / (float)pixelMagnitudes.size();
			magnitude *= 4.0; // magnitude / 255 * 1000
			magnitude /= pMagnitudeNormalization.get();
			magnitude = ofClamp(magnitude, 0, 1);
			
			
			float totalMagnitude = 0;
			for (auto tv : totalVelocity) { totalMagnitude += tv * tv; }
			totalMagnitude = sqrt(totalMagnitude);
			
			vector<float>	direction;
			direction.resize(numChannels, 0);
			
			for (int i=0; i<numChannels; i++) {
				if (totalMagnitude > 0) { direction[i] = totalVelocity[i] / totalMagnitude; }
				else { direction[i] = 0; }
				velocity[i] = direction[i] * magnitude;
			}
			
			// normalize to highest component and apply boost
			if (pHighComponentBoost.get() > 0 && type == FT_VELOCITY_SPLIT) {
				float highVelocity = 0;
				float P = 1;
				for (int i=0; i<numChannels; i++) {
					if (fabs(velocity[i]) > highVelocity) {
						highVelocity = fabs(velocity[i]);
						if (velocity[i] < 0) P = -1;
					}
				}
				for (int i=0; i<numChannels; i++) {
					velocity[i] /= highVelocity;
					velocity[i] = powf(fabs(velocity[i]), pHighComponentBoost.get()) * P;
					velocity[i] *= highVelocity;
				}
			}
			
			// use only 2 decimals
			
			pMagnitude.set(int(magnitude * 100) / 100.0);
			
			for (int i=0; i<numChannels; i++) {
				pVelocity[i] = int(velocity[i] * 100) / 100.0;
			}
			
			bUpdateVisualizer = true;
		}
	}
	
	//--------------------------------------------------------------
	void ftAverageFlow::getMeanStDev(vector<float> &_v, float &_mean, float &_stDev) {
		float mean = accumulate(_v.begin(), _v.end(), 0.0) / (float)_v.size();
		std::vector<float> diff(_v.size());
		std::transform(_v.begin(), _v.end(), diff.begin(), std::bind2nd(std::minus<float>(), mean));
		float sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
		float stDev = std::sqrt(sq_sum / _v.size());
		
		_mean = mean;
		_stDev = stDev;
	}
	
	//--------------------------------------------------------------
	void ftAverageFlow::setRoi(ofRectangle _rect) {
		float x = ofClamp(_rect.x, 0, 1);
		float y = ofClamp(_rect.y, 0, 1);
		float maxW = 1.0 - x;
		float maxH = 1.0 - y;
		float w = min(_rect.width, maxW);
		float h = min(_rect.height, maxH);
		
		roi = ofRectangle(x, y, w, h);
		
		if (pRoi[0] != x) { pRoi[0].set(x); }
		if (pRoi[1] != y) { pRoi[1].set(y); }
		if (pRoi[2].getMax() != maxW) { pRoi[2].setMax(maxW); pRoi[2].set(w); }
		if (pRoi[3].getMax() != maxH) { pRoi[3].setMax(maxH); pRoi[3].set(h); }
		if (pRoi[2] != w) { pRoi[2].set(w); }
		if (pRoi[3] != h) { pRoi[3].set(h); }
	}
	
	//--------------------------------------------------------------
	void ftAverageFlow::reset() {
		magnitude = 0;
		pMagnitude.set(0);
		for (int i=0; i<numChannels; i++) {
			velocity[i] = 0;
			pVelocity[i] = 0;
		}
		bUpdateVisualizer = true;
	}
	
	//--------------------------------------------------------------
	void ftAverageFlow::setupDraw() {
		graphSize = 120; //
		vector<ofIndexType> indices;
		vector<glm::vec3> vertices;
		vector<ofFloatColor> colors;
		indices.resize(graphSize);
		vertices.resize(graphSize);
		colors.resize(graphSize, magnitudeColor);
		for (int i=0; i<graphSize; i++) {
			indices[i] = i;
			vertices[i] = glm::vec3((1.0 / graphSize) * float(i), 0.5, 0);
		}
		magnitudeMesh.setMode(OF_PRIMITIVE_LINE_STRIP);
		magnitudeMesh.addIndices(indices);
		magnitudeMesh.addVertices(vertices);
		magnitudeMesh.addColors(colors);
		
		velocityMeshes.resize(numChannels);
		for (int i=0; i<numChannels; i++) {
			velocityMeshes[i].setMode(OF_PRIMITIVE_LINE_STRIP);
			velocityMeshes[i].addIndices(indices);
			velocityMeshes[i].addVertices(vertices);
			colors.clear();
			colors.resize(graphSize, velocityColors[i]);
			velocityMeshes[i].addColors(colors);
		}
	}
	
	//--------------------------------------------------------------
	void ftAverageFlow::draw(int _x, int _y, int _w, int _h) {
		int x = _x + roi.x * _w;
		int y = _y + roi.y * _h;
		int w = roi.width * _w;
		int h = roi.height * _h;
		
		drawVisualizer(x, y, w, h);
	}
	
	//--------------------------------------------------------------
	void ftAverageFlow::drawROI(int _x, int _y, int _w, int _h) {
		int x = _x + roi.x * _w;
		int y = _y + roi.y * _h;
		int w = roi.width * _w;
		int h = roi.height * _h;
		
		drawBackground(x, y, w, h);
	}
	
	//--------------------------------------------------------------
	void ftAverageFlow::drawVisualizer(int _x, int _y, int _w, int _h) {
		drawBackground(_x, _y, _w, _h);
		drawGraph(_x, _y, _w, _h);
	}
	
	//--------------------------------------------------------------
	void ftAverageFlow::drawBackground(int _x, int _y, int _w, int _h) {
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_ALPHA);
		ofSetColor(0, 0, 0, 63);
		ofDrawRectangle(_x, _y, _w, _h);
		ofNoFill();
		ofSetColor(0, 0, 0, 255);
		ofDrawRectangle(_x-1, _y-1, _w+2, _h+2);
		ofPopStyle();
	}
	
	//--------------------------------------------------------------
	void ftAverageFlow::drawGraph(int _x, int _y, int _w, int _h) {
		if (bUpdateVisualizer) {
			bUpdateVisualizer = false;
			
			for (int i=0; i<graphSize-1; i++) {
				magnitudeMesh.setVertex(i, glm::vec3(magnitudeMesh.getVertex(i).x, magnitudeMesh.getVertex(i+1).y, 0));
				for (int j=0; j<numChannels; j++) {
					velocityMeshes[j].setVertex(i, glm::vec3(velocityMeshes[j].getVertex(i).x, velocityMeshes[j].getVertex(i+1).y, 0));
				}
			}
			float m = (type == FT_VELOCITY_SPLIT)? 1.0 - magnitude : 0.5 + magnitude * -.5;
			magnitudeMesh.setVertex(graphSize-1, glm::vec3(magnitudeMesh.getVertex(graphSize-1).x, m, 0));
			for (int i=0; i<numChannels; i++) {
				float c = (type == FT_VELOCITY_SPLIT)? 1.0 - velocity[i] : 0.5 + velocity[i] * -.5;
				velocityMeshes[i].setVertex(graphSize-1, glm::vec3(velocityMeshes[i].getVertex(graphSize-1).x, c, 0));
			}
		}
		
		ofPushStyle();
		ofEnableBlendMode(OF_BLENDMODE_ALPHA);
		ofPushView();
		ofTranslate(_x, _y);
		ofScale(_w, _h);
		magnitudeMesh.draw();
		for (int c=0; c<numChannels; c++) {
			velocityMeshes[c].draw();
		}
		ofPopView();
		
		ofEnableBlendMode(OF_BLENDMODE_ALPHA);
		drawGraphOverlay(_x, _y, _w, _h);
		ofPopStyle();
	}
	
	//--------------------------------------------------------------
	void ftAverageFlow::drawGraphOverlay(int _x, int _y, int _w, int _h) {
		ofPushStyle();
		ofPushView();
		ofTranslate(_x, _y);
		
		int yStep = 16;
		int yOffset = yStep;
		ofDrawBitmapStringHighlight(parameters.getName(), 5, yOffset);
		yOffset += yStep * 1.5;
		ofSetColor(magnitudeColor);
		ofDrawBitmapString("magnitude",5, yOffset);
		yOffset += yStep;
		
		for (int i=0; i<numChannels; i++) {
			ofSetColor(velocityColors[i]);
			ofDrawBitmapString(ftUtil::getComponentName(type, i), 5, yOffset);
			yOffset += yStep;
		}
		
		ofSetColor(255,255,255,255);
		if (type != FT_VELOCITY_SPLIT) {
			ofDrawBitmapString("1",  _w - 10, yStep);
			ofDrawBitmapString("0",  _w - 10, (_h * 0.5) + yStep);
			ofDrawBitmapString("-1", _w - 18, _h - yStep * .5);
			ofSetColor(255,255,255,195);
			ofDrawLine(0, 0.5 * _h, _w, 0.5 * _h);
		} else {
			ofDrawBitmapString("1", _w - 10, yStep);
			ofDrawBitmapString("0", _w - 10, _h - yStep * .5);
		}
		ofPopView();
		ofPopStyle();
	}
	
}

