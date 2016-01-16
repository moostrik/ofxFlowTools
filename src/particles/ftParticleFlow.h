
#pragma once

#include "ofMain.h"
#include "ftFbo.h"
#include "ftSwapBuffer.h"
#include "ftInitPositionShader.h"
#include "ftDrawParticleShader.h"
#include "ftMoveParticleShader.h"
#include "ftAgeLifespanMassSizeParticleShader.h"
#include "ftAddForceShader.h"

namespace flowTools {
	
	class ftParticleFlow {
	public:
		ftParticleFlow();
		
		void setup(int _simulationWidth, int _simulationHeight, int _numParticlesX, int _numParticlesY);
		
		void addFlowVelocity(ofTexture& _tex, float _strength = 1.0) ;
		void addFluidVelocity (ofTexture& _tex, float _strength = 1.0) ;
		void setObstacle (ofTexture& _tex) ;
		
		void update(float _deltaTime = 0);
		
		void draw(int _x, int _y) {draw(_x, _y, numParticlesX, numParticlesY);}
		void draw(int _x, int _y, int _width, int _height);
		
		bool	isActive() {return bIsActive; }
		void	activate(bool _state) {bIsActive.set(_state);}
		
		float	getSpeed() { return speed.get(); }
		float	getCellSize() { return cellSize.get(); }
		float	getBirthChance() { return birthChance.get(); }
		float	getBirthVelocityChance() { return birthVelocityChance.get(); }
		float	getLifeSpan() { return lifeSpan.get(); }
		float	getLifeSpanSpread() { return lifeSpanSpread.get(); }
		float	getMass() { return mass.get(); }
		float	getMassSpread() { return massSpread.get(); }
		float	getSize() { return size.get(); }
		float	getSizeSpread() { return sizeSpread.get(); }
		
		void	setSpeed(float value) { speed.set(value); }
		void	setCellSize(float value) { cellSize.set(value); }
		void	setBirthChance(float value) { birthChance.set(value); }
		void	setBirthVelocityChance(float value) { birthVelocityChance.set(value); }
		void	setLifeSpan(float value) { lifeSpan.set(value); }
		void	setLifeSpanSpread(float value) { lifeSpanSpread.set(value); }
		void	setMass(float value) { mass.set(value); }
		void	setMassSpread(float value) { massSpread.set(value); }
		void	setSize(float value) { size.set(value); }
		void	setSizeSpread(float value) { sizeSpread.set(value); }
		
		ofParameterGroup parameters;
	private:
		ofParameter<bool>	bIsActive;
		ofParameter<float>	speed;
		ofParameter<float>	cellSize;
		ofParameter<float>	birthChance;
		ofParameter<float>	birthVelocityChance;
		ofParameter<float>	lifeSpan;
		ofParameter<float>	lifeSpanSpread;
		ofParameter<float>	mass;
		ofParameter<float>	massSpread;
		ofParameter<float>	size;
		ofParameter<float>	sizeSpread;
		ofParameter<float>	twinkleSpeed;
		
		float	simulationWidth;
		float	simulationHeight;
		float	numParticlesX;
		float	numParticlesY;
		int		numParticles;
		float	deltaTime;
		float	lastTime;
		float	timeStep;
		
		ofTexture *flowVelocityTexture;
		ofTexture *fluidVelocityTexture;
		ofTexture *colorTexture;
		
		ofVboMesh	particleMesh;
		
		ftSwapBuffer			particleAgeLifespanMassSizeSwapBuffer;
		ftSwapBuffer			particlePositionSwapBuffer;
		ftSwapBuffer			particleColorSwapBuffer;
		
		ftFbo					particleHomeBuffer;
		ftSwapBuffer			flowVelocitySwapBuffer;
		ftSwapBuffer			fluidVelocitySwapBuffer;
		ftSwapBuffer			densitySwapBuffer;
		ftFbo					obstacleBuffer;
		
		ftInitPositionShader	initPositionShader;
		ftDrawParticleShader	drawParticleShader;
		ftMoveParticleShader	moveParticleShader;
		ftAgeLifespanMassSizeParticleShader	ALMSParticleShader;
		ftAddForceShader		addShader;
	};
		
}