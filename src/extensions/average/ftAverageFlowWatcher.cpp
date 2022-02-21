#include "ftAverageFlowWatcher.h"

namespace flowTools {

void ftAverageFlowWatcher::setup(flowTools::ftFlowForceType _type)   {

  ftAverageFlow::setup(_type);

  magnitudeEvent = 0;
  magnitudeHigh = 0;
  magnitudeLow = 0;

  velocityEvents.resize(numChannels, 0);
  velocityHighs.resize(numChannels, 0);
  velocityLows.resize(numChannels, 0);

  baseColor =      ofFloatColor(0.5, 0.5, 0.5, 0.8);
  thresholdColor =  ofFloatColor(1.0, 0.0, 0.0, 1.0);

  eventParameters.setName("events");
  eventParameters.add(pThreshold.set("threshold", .25, .0, .3));
  eventParameters.add(pBase.set("base", .6, .5, .75));
  eventParameters.add(pMagnitudeEvent.set("magnitude", 0, -1, 1));
  pVelocityEvents.resize(numChannels);
  for (int i=0; i<numChannels; i++) {
    eventParameters.add(pVelocityEvents[i].set("velocity " + ofToString(i), false));
  }
  parameters.add(eventParameters);

  baseMesh.setMode(OF_PRIMITIVE_LINE_STRIP);
  baseMesh.addIndices(magnitudeMesh.getIndices());
  baseMesh.addVertices(magnitudeMesh.getVertices());
  vector<ofFloatColor> colors;
  colors.resize(graphSize, baseColor);
  baseMesh.addColors(colors);
}

//--------------------------------------------------------------
void ftAverageFlowWatcher::update(ofFloatPixels& _pixels)  {
  ftAverageFlow::update(_pixels);

  float eV = magnitude;
  if (!magnitudeEvent) {
    if (eV < magnitudeLow) { magnitudeLow = eV; }

    if (eV > magnitudeLow + pThreshold.get()) {
      magnitudeEvent = true;
      magnitudeHigh = eV;
    }
  }
  if (magnitudeEvent) {
    if (eV > magnitudeHigh) {
      magnitudeHigh = eV;
    }
    if (eV < magnitudeHigh * pThreshold.get()) {
      magnitudeEvent = false;
      magnitudeLow = eV;
    }
  }
  pMagnitudeEvent = magnitudeEvent;

  for (int i=0; i<numChannels; i++) {
    eV = fabs(velocity[i]);
    if (velocityEvents[i] == 0) {
      if (eV < velocityLows[i]) { velocityLows[i] = eV; }

      if (eV > velocityLows [i] + pThreshold.get() && eV > magnitude * pBase.get()) {

        velocityEvents[i] = (velocity[i] > 0)? 1 : -1;
        velocityHighs[i] = eV;
      }
    }
    if (velocityEvents[i] != 0) {
      if (eV > velocityHighs[i]) {
        velocityHighs[i] = eV;
      }
      if (eV < velocityHighs[i] * pThreshold.get()) {
        velocityEvents[i] = 0;
        velocityLows[i] = eV;
      }
    }
    pVelocityEvents[i] = velocityEvents[i] != 0;
  }
}

//--------------------------------------------------------------
void ftAverageFlowWatcher::setupDraw() {
  ftAverageFlow::setupDraw();

  int eventStep = 16;
  int eventStart = eventStep + eventStep * 1.25;

  vector<ofIndexType> indices;
  vector<glm::vec3> vertices;
  vector<ofFloatColor> colors;
  indices.resize(graphSize);
  vertices.resize(graphSize);
  colors.resize(graphSize, magnitudeColor);
  for (int i=0; i<graphSize; i++) {
    indices[i] = i;
    vertices[i] = glm::vec3((1.0 / graphSize) * float(i), eventStart, 0);
  }
  magnitudeEventMesh.setMode(OF_PRIMITIVE_LINE_STRIP);
  magnitudeEventMesh.addIndices(indices);
  magnitudeEventMesh.addVertices(vertices);
  magnitudeEventMesh.addColors(colors);

  velocityEventMeshes.resize(numChannels);
  for (int i=0; i<numChannels; i++) {
    velocityEventMeshes[i].setMode(OF_PRIMITIVE_LINE_STRIP);
    velocityEventMeshes[i].addIndices(indices);

    for (int j=0; j<graphSize; j++) {
      vertices[j].y = eventStart + eventStep * (i + 1);
    }
    velocityEventMeshes[i].addVertices(vertices);
    colors.clear();
    colors.resize(graphSize, velocityColors[i]);
    velocityEventMeshes[i].addColors(colors);
  }
}

//--------------------------------------------------------------
void ftAverageFlowWatcher::drawGraph(int _x, int _y, int _w, int _h) {
  if (bUpdateVisualizer) {
    for (int i=0; i<graphSize-1; i++) {
      baseMesh.setVertex(i, glm::vec3(baseMesh.getVertex(i).x, baseMesh.getVertex(i+1).y, 0));
      magnitudeEventMesh.setColor(i, magnitudeEventMesh.getColor(i+1));
      for (int c=0; c<numChannels; c++) {
        velocityEventMeshes[c].setColor(i, velocityEventMeshes[c].getColor(i+1));
      }
    }
    magnitudeEventMesh.setColor(graphSize-1, magnitudeEvent? magnitudeColor : ofFloatColor(0,0,0,0));
    for (int c=0; c<numChannels; c++) {
      velocityEventMeshes[c].setColor(graphSize-1, velocityEvents[c]? velocityColors[c] : ofFloatColor(0,0,0,0));
    }

    float b = (type == FT_VELOCITY_SPLIT)? 1.0 - magnitude * pBase.get() : 0.5 + magnitude * pBase.get() * -.5;
    baseMesh.setVertex(graphSize-1, glm::vec3(baseMesh.getVertex(graphSize-1).x, b, 0));
  }

  ofPushStyle();
  ofEnableBlendMode(OF_BLENDMODE_ALPHA);
  ofPushView();
  ofTranslate(_x, _y);
  ofScale(_w, _h);
  ofSetColor(thresholdColor);
  if (type != FT_VELOCITY_SPLIT) {
    float t = 0.5 + pThreshold.get() * .5;
    ofDrawLine(0, t, 1, t);
    t = 0.5 + pThreshold.get() * -.5;
    ofDrawLine(0, t, 1, t);
  }
  else {
    float t = 1.0 - pThreshold.get();
    ofDrawLine(0, t, 1, t);
  }
  baseMesh.draw();
  ofPopView();
  ofPopStyle();

  ofPushStyle();
  ofEnableBlendMode(OF_BLENDMODE_ALPHA);
  ofPushView();
  ofTranslate(_x, _y);
  ofScale(_w, 1);
  magnitudeEventMesh.draw();
  for (int c=0; c<numChannels; c++) { velocityEventMeshes[c].draw(); }
  ofPopView();
  ofPopStyle();

  ftAverageFlow::drawGraph(_x, _y, _w, _h);
}

//--------------------------------------------------------------
void ftAverageFlowWatcher::drawGraphOverlay(int _x, int _y, int _w, int _h) {
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

  ofSetColor(thresholdColor);
  ofDrawBitmapString("event threshold", 5, yOffset);
  yOffset += yStep;
  ofSetColor(baseColor);
  ofDrawBitmapString("event base", 5, yOffset);

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

};
