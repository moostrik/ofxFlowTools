
#pragma once

#include "ofMain.h"
#include "ftUtil.h"
#include "ftFlow.h"
#include "ftPingPongFbo.h"
#include "ftInitPositionShader.h"
#include "ftDrawParticleShader.h"
#include "ftMoveParticleShader.h"
#include "ftAgeLifespanMassSizeParticleShader.h"
#include "ftAddMultipliedShader.h"

namespace flowTools {

class ftParticleFlow : public ftFlow {
public:
  ftParticleFlow();

  void  setup(int _simulationWidth, int _simulationHeight, int _densityWidth, int _densityHeight) { setup(_simulationWidth, _simulationHeight, _densityWidth, _densityHeight, _densityWidth, _densityHeight); }
  void  setup(int _simulationWidth, int _simulationHeight, int _densityWidth, int _densityHeight, int _numParticlesX, int _numParticlesY);

  void  setFlow(ftFlowForceType _type, ofTexture& _tex);
  void  addFlow(ftFlowForceType _type, ofTexture& _tex, float _strength  = 1.0);

  void  addFluidVelocity (ofTexture& _tex, float _strength = 1.0) { addInput(_tex, _strength); }
  void  addFlowVelocity(ofTexture& _tex, float _strength = 1.0)   { ftFlow::add(flowVelocityFbo, _tex, _strength); }
  void  addDensity(ofTexture& _tex, float _strength  = 1.0)       { ftFlow::add(densityFbo, _tex, _strength); }
  void  addObstacle(ofTexture& _tex)                              { ftFlow::add(obstacleFbo, _tex, 1.0); }

  void  setFluidVelocity (ofTexture& _tex)                        { setInput(_tex); }
  void  setFlowVelocity(ofTexture& _tex)                          { ftFlow::set(flowVelocityFbo, _tex); }
  void  setDensity(ofTexture& _tex)                               { ftFlow::set(densityFbo, _tex); }
  void  setObstacle(ofTexture& _tex)                              { ftFlow::set(obstacleFbo, _tex); }

  void  update(float _deltaTime);
  void  reset() override;

  void  drawOutput(int _x, int _y, int _w, int _h) override;

  float  getSpeed() { return speed.get(); }
  float  getCellSize() { return cellSize.get(); }
  float  getBirthChance() { return birthChance.get(); }
  float  getBirthVelocityChance() { return birthVelocityChance.get(); }
  float  getLifeSpan() { return lifeSpan.get(); }
  float  getLifeSpanSpread() { return lifeSpanSpread.get(); }
  float  getMass() { return mass.get(); }
  float  getMassSpread() { return massSpread.get(); }
  float  getSize() { return size.get(); }
  float  getSizeSpread() { return sizeSpread.get(); }
  glm::vec2 getGravity() {return gravity.get();}

  void  setSpeed(float value) { speed.set(value); }
  void  setCellSize(float value) { cellSize.set(value); }
  void  setBirthChance(float value) { birthChance.set(value); }
  void  setBirthVelocityChance(float value) { birthVelocityChance.set(value); }
  void  setLifeSpan(float value) { lifeSpan.set(value); }
  void  setLifeSpanSpread(float value) { lifeSpanSpread.set(value); }
  void  setMass(float value) { mass.set(value); }
  void  setMassSpread(float value) { massSpread.set(value); }
  void  setSize(float value) { size.set(value); }
  void  setSizeSpread(float value) { sizeSpread.set(value); }
  void  setGravity(glm::vec2 value) { gravity.set(value); }

private:
  ofParameter<float>    speed;
  ofParameter<float>    cellSize;
  ofParameter<float>    birthChance;
  ofParameter<float>    birthVelocityChance;
  ofParameter<float>    lifeSpan;
  ofParameter<float>    lifeSpanSpread;
  ofParameter<float>    mass;
  ofParameter<float>    massSpread;
  ofParameter<float>    size;
  ofParameter<float>    sizeSpread;
  ofParameter<float>    twinkleSpeed;
  ofParameter<glm::vec2>  gravity;

  int            numParticlesX, numParticlesY, numParticles;
  int           simulationWidth;

  ofVboMesh        particleMesh;

  ofFbo          particleHomeFbo;
  ftPingPongFbo      particleAgeLifespanMassSizeFbo;
  ftPingPongFbo      particlePositionFbo;
  ftPingPongFbo      particleColorFbo;


  // inputs
  ftPingPongFbo      flowVelocityFbo;
  //    ftPingPongFbo      fluidVelocityFbo; // use inputFbo instead
  ftPingPongFbo      densityFbo;
  ftPingPongFbo      obstacleFbo;

  ftInitPositionShader  initPositionShader;
  ftDrawParticleShader  drawParticleShader;
  ftMoveParticleShader  moveParticleShader;
  ftAgeLifespanMassSizeParticleShader  ALMSParticleShader;
  ftAddMultipliedShader  addMultipliedShader;
};

}
