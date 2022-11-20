#pragma once
#include "ParticleManager.h"

class ParticleEmitter
{
public:
	ParticleManager* manager = nullptr;

	Vector3 pos;
	float spawnInterval = 0.5f;
	int spawnCount = 1;
	float timer = 0;

	Vector3 spawnAreaSize = { 1, 1, 1 };
	Vector3 minVelocity = { 0, 0, 0 };
	Vector3 maxVelocity = { 0, 0, 0 };
	Vector3 minAccel = { 0, 0, 0 };
	Vector3 maxAccel = { 0, 0, 0 };
	Vector2 minScale = { 1, 1 };
	Vector2 maxScale = { 1, 1 };
	Color color = { 1, 1, 1, 1 };
	Color endColor = { 1, 1, 1, 1 };
	float minAliveTime = 0.5f;
	float maxAliveTime = 0.5f;
	bool useSubtract = false;

	ParticleEmitter(ParticleManager* manager)
		: manager(manager) {}

	void Update();
};

