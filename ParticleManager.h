#pragma once
#include "Particle.h"
#include <vector>
#include "PolygonParticle.h"
#include "ModelObj.h"

class ParticleManager
{
private:
	std::vector<Particle> normalParticlePool;
	std::vector<PolygonParticle> polygonParticlePool;

	RootSignature normalParticleRoot;
	GraphicsPipeline normalParticlePipeline;
	GraphicsPipeline subParticlePipeline;
	RootSignature polygonParticleRoot;
	GraphicsPipeline polygonParticlePipeline;

public:
	ParticleManager();

	void Update();
	void Draw();

	void SpawnNormalParticle(Vector3 pos, Vector3 velocity, Vector3 accel, Vector2 scale, Color color, float aliveTime, Color endcolor = { 0, 0, 0, 1 }, bool subtract = false);

	void SpawnPolygonExplostion(ModelObj& obj, Vector3 minVelocity, Vector3 maxVelocity, Vector3 minAccel, Vector3 maxAccel, float aliveTime);
};

