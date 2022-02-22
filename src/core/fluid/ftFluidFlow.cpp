/*  ************************************************************************************
 *
 *  ftFluidFlow
 *
 *  Created by Matthias Oostrik on 03/16.14.
 *  Copyright 2014 http://www.MatthiasOostrik.com All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the author nor the names of its contributors
 *       may be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 *  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 *  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 *  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 *  OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  The Fluid shaders are adapted from various sources. Unfortunately I can't remember all, but the most important ones:
 *    * Mark J Harris: Various online sources
 *    * Patricio Gonzalez Vivo (http://www.patriciogonzalezvivo.com): ofxFluid
 *
 *  ************************************************************************************ */

#include "ftFluidFlow.h"

namespace flowTools {

ftFluidFlow::ftFluidFlow(){
  parameters.setName("fluid");

  velParameters.setName("velocity");
  velParameters.add(velSpeed.set(        "speed"          , 0.3, 0.0, 1.0));
  velParameters.add(velDissipation.set(  "dissipation"    , 0.1, 0.0, 1.0));
  velParameters.add(velVorticity.set(    "vorticity"      , 0.5, 0.0, 1.0));
  velParameters.add(velViscosity.set(    "viscosity"      , 0.5, 0.0, 1.0));
  velParameters.add(velViscosityIter.set( "viscos iter"   ,  20,   0, 100));
  parameters.add(velParameters);

  prsParameters.setName("pressure");
  prsParameters.add(prsSpeed.set(         "speed"         , 0.0, 0.0, 1.0));
  prsParameters.add(prsDissipation.set(   "dissipation"   , 0.1, 0.0, 1.0));
  prsParameters.add(prsIterations.set(    "iterations"    ,  40,   0, 100));
  parameters.add(prsParameters);

  denParameters.setName("density");
  denParameters.add(denSpeed.set(         "speed"         , 0.3, 0.0, 1.0));
  denParameters.add(denDissipation.set(   "dissipation"   , 0.1, 0.0, 1.0));
  parameters.add(denParameters);

  tmpParameters.setName("temperature");
  tmpParameters.add(tmpSpeed.set(         "speed"         , 0.3, 0.0, 1.0));
  tmpParameters.add(tmpDissipation.set(   "dissipation"   , 0.1, 0.0, 1.0));
  tmpParameters.add(tmpBuoyancy.set(      "buoyancy"      , 0.6, 0.0, 1.0));
  tmpParameters.add(tmpWeight.set(        "weight"        , 0.2, 0.0, 1.0));
  tmpParameters.add(tmpAmbient.set(       "abient temp"   , 0.2, 0.0, 1.0));
  parameters.add(tmpParameters);
}

//--------------------------------------------------------------
void ftFluidFlow::setup(int _simulationWidth, int _simulationHeight, int _densityWidth, int _densityHeight) {
  allocate(_simulationWidth, _simulationHeight, GL_RG32F, _densityWidth, _densityHeight, GL_RGBA32F);
  gridScale = 1;
}

//--------------------------------------------------------------
void ftFluidFlow::allocate(int _simulationWidth, int _simulationHeight, GLint _simulationInternalFormat, int _densityWidth, int _densityHeight, GLint _densityInternalFormat) {
  simulationWidth = _simulationWidth;
  simulationHeight = _simulationHeight;
  densityWidth = _densityWidth;
  densityHeight = _densityHeight;

  ftFlow::allocate(simulationWidth, simulationHeight, _simulationInternalFormat, densityWidth, densityHeight, _densityInternalFormat);

  visualizationField.setup(simulationWidth, simulationHeight);

  temperatureFbo.allocate(simulationWidth,simulationHeight,GL_R32F);
  ftUtil::zero(temperatureFbo);
  pressureFbo.allocate(simulationWidth,simulationHeight,GL_R32F);
  ftUtil::zero(pressureFbo);
  divergenceFbo.allocate(simulationWidth, simulationHeight, GL_R32F);
  ftUtil::zero(divergenceFbo);
  vorticityCurlFbo.allocate(simulationWidth, simulationHeight, GL_RG32F);
  ftUtil::zero(vorticityCurlFbo);
  vorticityForceFbo.allocate(simulationWidth, simulationHeight, GL_RG32F);
  ftUtil::zero(vorticityForceFbo);
  buoyancyFbo.allocate(simulationWidth, simulationHeight, GL_RG32F);
  ftUtil::zero(buoyancyFbo);
  obstacleFbo.allocate(simulationWidth, simulationHeight, GL_R8);
  ftUtil::zero(obstacleFbo);
  obstacleOffsetFbo.allocate(simulationWidth, simulationHeight, GL_RGBA8);
  ftUtil::zero(obstacleOffsetFbo);

  initObstacle();
  obstacleOffsetShader.update(obstacleOffsetFbo, obstacleFbo.getTexture());
}

//--------------------------------------------------------------
void ftFluidFlow::update(float _deltaTime){
  ofPushStyle();
  ofEnableBlendMode(OF_BLENDMODE_DISABLED);

  ftPingPongFbo& velocityFbo = inputFbo;
  ftPingPongFbo& densityFbo = outputFbo;


  // DENSITY ADVECT & DISSIPATE
  float advDenStep = _deltaTime * denSpeed.get() * 100 * gridScale;
  float disDen = 1.0 - _deltaTime * denDissipation.get();
  densityFbo.swap();
  advectShader.update(densityFbo.get(), densityFbo.getBackTexture(), velocityFbo.getTexture(), obstacleFbo.getTexture(), gridScale, advDenStep, disDen);


  // VELOCITY ADVECT & DISSIPATE
  float advVelStep = _deltaTime * velSpeed.get() * 100 * gridScale;
  float disVel = 1.0 - _deltaTime * velDissipation.get();
  velocityFbo.swap();
  advectShader.update(velocityFbo.get(), velocityFbo.getBackTexture(), velocityFbo.getBackTexture(), obstacleFbo.getTexture(), gridScale, advVelStep, disVel);

  // VELOCITY DIFFUSE
  if (velViscosity.get() > 0.0) {
    float viscosityStep = .25 * velViscosity.get();
    for (int i = 0; i < velViscosityIter.get(); i++) {
      velocityFbo.swap();
      jacobiDiffusionShader.update(velocityFbo.get(), velocityFbo.getBackTexture(), obstacleFbo.getTexture(), obstacleOffsetFbo.getTexture(), gridScale, viscosityStep);
    }
  }

  // VELOCITY VORTEX CONFINEMENT
  if (velVorticity.get() > 0.0) {
    float vorticityStep = velVorticity.get() * gridScale;
    vorticityCurlShader.update(vorticityCurlFbo, velocityFbo.getTexture(), obstacleFbo.getTexture(), gridScale);
    vorticityForceShader.update(vorticityForceFbo, vorticityCurlFbo.getTexture(), gridScale, vorticityStep);
    addVelocity(vorticityForceFbo.getTexture());
  }


  // TEMPERATURE ADVECT & DISSIPATE
  float advTmpStep = _deltaTime * tmpSpeed.get() * 100 * gridScale;
  float disTmp = 1.0 - _deltaTime * tmpDissipation.get();
  temperatureFbo.swap();
  advectShader.update(temperatureFbo.get(), temperatureFbo.getBackTexture(), velocityFbo.getTexture(), obstacleFbo.getTexture(), gridScale, advTmpStep, disTmp);

  // TEMPERATURE BUOYANCY
  if (tmpBuoyancy.get() > 0.0 && tmpWeight.get() > 0.0 ) {
    float bouyStep = tmpBuoyancy.get();
    buoyancyShader.update(buoyancyFbo, velocityFbo.getTexture(), temperatureFbo.getTexture(), densityFbo.getTexture(), bouyStep, tmpWeight.get(), tmpAmbient );
    addVelocity(buoyancyFbo.getTexture());
  }


  // PRESSURE ADVECT & DISSIPATE
  float advPrsStep = _deltaTime * prsSpeed.get() * 100 * gridScale;
  float disPrs = 1.0 - prsDissipation.get() * prsDissipation.get();
  pressureFbo.swap();
  //    multiplyForceShader.update(pressureFbo.get(), pressureFbo.getBackTexture(), disPrs);
  advectShader.update(pressureFbo.get(), pressureFbo.getBackTexture(), velocityFbo.getTexture(), obstacleFbo.getTexture(), gridScale, advPrsStep, disPrs);

  // PRESSURE
  divergenceShader.update(divergenceFbo, velocityFbo.getTexture(), obstacleFbo.getTexture(), obstacleOffsetFbo.getTexture(), gridScale);
  for (int i = 0; i < prsIterations.get(); i++) {
    pressureFbo.swap();
    jacobiPressureShader.update(pressureFbo.get(), pressureFbo.getBackTexture(), divergenceFbo.getTexture(), obstacleFbo.getTexture(), obstacleOffsetFbo.getTexture(), gridScale);
  }
  velocityFbo.swap();
  gradientShader.update(velocityFbo.get(), velocityFbo.getBackTexture(), pressureFbo.getTexture(), obstacleFbo.getTexture(), obstacleOffsetFbo.getTexture(), gridScale);

  ofPopStyle();
}

//--------------------------------------------------------------
void ftFluidFlow::setFlow(flowTools::ftFlowForceType _type, ofTexture &_tex) {
  switch (_type) {
  case FT_VELOCITY:    setVelocity(_tex);    break;
  case FT_DENSITY:    setDensity(_tex);    break;
  case FT_TEMPERATURE:  setTemperature(_tex);  break;
  case FT_PRESSURE:    setPressure(_tex);    break;
  case FT_OBSTACLE:    setObstacle(_tex);    break;
  default:
    ofLogWarning("ftFluidFlow: addFlow") << "no method to add flow of type " << _type;
    break;
  }
}

//--------------------------------------------------------------
void ftFluidFlow::addFlow(flowTools::ftFlowForceType _type, ofTexture &_tex, float _strength) {
  switch (_type) {
  case FT_VELOCITY:     addVelocity(_tex, _strength);    break;
  case FT_DENSITY:      addDensity(_tex, _strength);    break;
  case FT_TEMPERATURE:  addTemperature(_tex, _strength);  break;
  case FT_PRESSURE:     addPressure(_tex, _strength);    break;
  case FT_OBSTACLE:     addObstacle(_tex);          break;
  default:
    ofLogWarning("ftFluidFlow: addFlow") << "no method to add flow of type " << _type;
    break;
  }
}

//--------------------------------------------------------------
void ftFluidFlow::initObstacle(){
  ofPushStyle();
  ofEnableBlendMode(OF_BLENDMODE_DISABLED);
  ftUtil::one(obstacleFbo);
  obstacleFbo.begin();
  ofSetColor(0,0,0,255);
  int borderSize = 1;
  ofDrawRectangle(borderSize, borderSize, obstacleFbo.getWidth()-borderSize*2, obstacleFbo.getHeight()-borderSize*2);
  obstacleFbo.end();
  ofPopStyle();
}

//--------------------------------------------------------------
void ftFluidFlow::setObstacle(ofTexture & _tex){
  ofPushStyle();
  ofEnableBlendMode(OF_BLENDMODE_DISABLED);
  initObstacle();
  obstacleFbo.swap();
  addBooleanShader.update(obstacleFbo.get(), obstacleFbo.getBackTexture(), _tex);
  obstacleOffsetShader.update(obstacleOffsetFbo, obstacleFbo.getTexture());
  ofPopStyle();
}

//--------------------------------------------------------------
void ftFluidFlow::addObstacle(ofTexture & _tex){
  ofPushStyle();
  ofEnableBlendMode(OF_BLENDMODE_DISABLED);
  obstacleFbo.swap();
  addBooleanShader.update(obstacleFbo.get(), obstacleFbo.getBackTexture(), _tex);
  obstacleOffsetShader.update(obstacleOffsetFbo, obstacleFbo.getTexture());
  ofPopStyle();
}

//--------------------------------------------------------------
void ftFluidFlow::reset() {
  ftFlow::reset();
  ftUtil::zero(pressureFbo);
  ftUtil::zero(temperatureFbo);
  ftUtil::zero(divergenceFbo);
  ftUtil::zero(vorticityCurlFbo);
  ftUtil::zero(buoyancyFbo);
  initObstacle();

  advectShader          = ftAdvectShader();
  buoyancyShader        = ftBuoyancyShader();
  divergenceShader      = ftDivergenceShader();
  gradientShader        = ftGradientShader();
  jacobiDiffusionShader = ftJacobiDiffusionShader();
  obstacleOffsetShader  = ftObstacleOffsetShader();
  vorticityCurlShader   = ftVorticityCurlShader();
  vorticityForceShader  = ftVorticityForceShader();
}
}

