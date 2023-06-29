
#pragma once

#include "ofMain.h"

#include "ftFlow.h"
#include "ftAdvectShader.h"
#include "ftBuoyancyShader.h"
#include "ftDivergenceShader.h"
#include "ftGradientShader.h"
#include "ftJacobiDiffusionShader.h"
#include "ftJacobiPressureShader.h"
#include "ftObstacleOffsetShader.h"
#include "ftVorticityCurlShader.h"
#include "ftVorticityForceShader.h"

#include "ftAddBooleanShader.h"
#include "ftMultiplyForceShader.h"

namespace flowTools {
class ftFluidFlow : public ftFlow{
public:
  ftFluidFlow();

  void  setup(int _simulationWidth, int _simulationHeight)  { setup(_simulationWidth, _simulationHeight, _simulationWidth, _simulationHeight); }
  void  setup(int _simulationWidth, int _simulationHeight, int _densityWidth, int _densityHeight);
  void  update(float _deltaTime);
  void  reset() override;

  void  setFlow(ftFlowForceType _type, ofTexture& _tex);
  void  setVelocity(ofTexture& _tex)    { setInput(_tex); }
  void  setDensity(ofTexture& _tex)     { set(outputFbo, _tex); }
  void  setTemperature(ofTexture& _tex) { set(temperatureFbo, _tex); }
  void  setPressure(ofTexture& _tex)    { set(pressureFbo, _tex); }
  void  setObstacle(ofTexture& _tex);

  void  addFlow(ftFlowForceType _type, ofTexture& _tex, float _strength  = 1.0);
  void  addVelocity(ofTexture& _tex, float _strength  = 1.0)    { addInput(_tex, _strength); }
  void  addDensity(ofTexture& _tex, float _strength  = 1.0)     { add(outputFbo, _tex, _strength); }
  void  addTemperature(ofTexture& _tex, float _strength  = 1.0) { add(temperatureFbo, _tex, _strength); }
  void  addPressure(ofTexture& _tex, float _strength  = 1.0)    { add(pressureFbo, _tex, _strength); }
  void  addObstacle(ofTexture& _tex);

  void resetObstacle() { initObstacle(); }

  ofTexture&  getDensity()        { return getOutput(); }
  ofTexture&  getVelocity()       { return getInput(); }
  ofTexture&  getPressure()       { return pressureFbo.getTexture(); }
  ofTexture&  getTemperature()    { return temperatureFbo.getTexture(); }
  ofTexture&  getDivergence()     { return divergenceFbo.getTexture(); }
  ofTexture&  getObstacle()       { return obstacleOffsetFbo.getTexture(); }
  ofTexture&  getObstacleEdges()  { return obstacleFbo.getTexture(); }
  ofTexture&  getVorticityCurl()  { return vorticityCurlFbo.getTexture(); }
  ofTexture&  getBuoyancy()       { return buoyancyFbo.getTexture(); }

  void  drawVelocity(int _x, int _y, int _w, int _h)        { drawInput (_x, _y, _w, _h); }
  void  drawOutput(int _x, int _y, int _w, int _h) override { drawDensity (_x, _y, _w, _h); }
  void  drawDensity(int _x, int _y, int _w, int _h)         { outputFbo.getTexture().draw(_x, _y, _w, _h); }
  void  drawPressure(int _x, int _y, int _w, int _h)        { visualizationField.draw(pressureFbo.getTexture(), _x, _y, _w, _h); }
  void  drawTemperature(int _x, int _y, int _w, int _h)     { visualizationField.draw(temperatureFbo.getTexture(), _x, _y, _w, _h); }
  void  drawDivergence(int _x, int _y, int _w, int _h)      { visualizationField.draw(divergenceFbo.getTexture(), _x, _y, _w, _h); }
  void  drawObstacle(int _x, int _y, int _w, int _h)        { obstacleFbo.draw(_x, _y, _w, _h); }
  void  drawVorticity(int _x, int _y, int _w, int _h)       { visualizationField.draw(vorticityForceFbo.getTexture(), _x, _y, _w, _h); }
  void  drawBuoyancy(int _x, int _y, int _w, int _h)        { visualizationField.draw(buoyancyFbo.getTexture(), _x, _y, _w, _h); }

  int    getSimulationWidth()         { return simulationWidth; }
  int    getSimulationHeight()        { return simulationHeight; }
  int    getDensityWidth()            { return densityWidth; }
  int    getDensityHeight()           { return densityHeight; }

  float  getVelocitySpeed()           { return velSpeed.get(); }
  float  getVelocityDissipation()     { return velDissipation.get(); }
  float  getVorticity()               { return velVorticity.get(); }
  float  getViscosity()               { return velViscosity.get(); }
  int    getViscosityIterations()     { return velViscosityIter.get(); }

  float  getDensitySpeed()            { return denSpeed.get(); }
  float  getDensityDissipation()      { return denDissipation.get(); }

  float  getTemperatureSpeed()        { return tmpSpeed.get(); }
  float  getTemperatureDissipation()  { return tmpDissipation.get(); }
  float  getTemperatureBuoyancy()     { return tmpBuoyancy.get(); }
  float  getTemperatureWeight()       { return tmpWeight.get(); }
  float  getTemperatureAmbient()      { return tmpAmbient.get(); }

  float  getPressureSpeed()           { return prsSpeed.get(); }
  float  getPressureDissipation()     { return prsDissipation.get(); }
  int    getPressureIterations()      { return prsIterations.get(); }


  void  setSpeed(float _value)        { setVelocitySpeed(_value); setDensitySpeed(_value); setTemperatureSpeed(_value); }
  void  setDissipation(float _value)  { setVelocityDissipation(_value); setDensityDissipation(_value); setTemperatureDissipation(_value); }

  void  setVelocitySpeed(float _value)          { velSpeed.set(_value); }
  void  setVelocityDissipation(float _value)    { velDissipation.set(_value); }
  void  setVorticity(float _value)              { velVorticity.set(_value); }
  void  setViscosity(float _value)              { velViscosity.set(_value); }
  void  setViscosityIterations(int _value)      { velViscosityIter.set(_value); }

  void  setDensitySpeed(float _value)           { denSpeed.set(_value); }
  void  setDensityDissipation(float _value)     { denDissipation.set(_value); }

  void  setTemperatureSpeed(float _value)       { tmpSpeed.set(_value); }
  void  setTemperatureDissipation(float _value) { tmpDissipation.set(_value); }
  void  setTemperatureBuoyancy(float _value)    { tmpBuoyancy.set(_value); }
  void  setTemperatureWeight(float _value)      { tmpWeight.set(_value); }
  void  setTemperatureAmbient(float _value)     { tmpAmbient.set(_value); }

  void  setPressureSpeed(float _value)          { prsSpeed.set(_value); }
  void  setPressureDissipation(float _value)    { prsDissipation.set(_value); }
  void  setPressureIterations(int _value)       { prsIterations.set(_value); }

protected:
  ofParameterGroup    velParameters;
  ofParameter<float>  velSpeed;
  ofParameter<float>  velDissipation;
  ofParameter<float>  velVorticity;
  ofParameter<float>  velViscosity;
  ofParameter<int>    velViscosityIter;

  ofParameterGroup    prsParameters;
  ofParameter<float>  prsSpeed;
  ofParameter<float>  prsDissipation;
  ofParameter<int>    prsIterations;

  ofParameterGroup    denParameters;
  ofParameter<float>  denSpeed;
  ofParameter<float>  denDissipation;

  ofParameterGroup    tmpParameters;
  ofParameter<float>  tmpSpeed;
  ofParameter<float>  tmpDissipation;
  ofParameter<float>  tmpBuoyancy;
  ofParameter<float>  tmpWeight;
  ofParameter<float>  tmpAmbient;

  ftAdvectShader          advectShader;
  ftBuoyancyShader        buoyancyShader;
  ftDivergenceShader      divergenceShader;
  ftGradientShader        gradientShader;
  ftJacobiDiffusionShader jacobiDiffusionShader;
  ftJacobiPressureShader  jacobiPressureShader;
  ftObstacleOffsetShader  obstacleOffsetShader;
  ftVorticityCurlShader   vorticityCurlShader;
  ftVorticityForceShader  vorticityForceShader;

  ftAddBooleanShader      addBooleanShader;
  ftMultiplyForceShader   multiplyForceShader;

  ftPingPongFbo pressureFbo;
  ftPingPongFbo temperatureFbo;
  ftPingPongFbo obstacleFbo;
  ofFbo         obstacleOffsetFbo;
  ofFbo         divergenceFbo;
  ofFbo         vorticityCurlFbo;
  ofFbo         vorticityForceFbo;
  ofFbo         buoyancyFbo;

  int simulationWidth, simulationHeight, densityWidth, densityHeight;
  int gridScale;

  void  allocate(int _inputWidth, int _inputHeight, GLint _inputInternalFormat, int _outputWidth, int _outputHeight, GLint _outputInternalFormat) override ;

  void  initObstacle();

};
}

