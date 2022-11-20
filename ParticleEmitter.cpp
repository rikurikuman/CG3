#include "ParticleEmitter.h"
#include "TimeManager.h"

void ParticleEmitter::Update()
{
	timer += TimeManager::deltaTime;
	while (timer >= spawnInterval) {
		timer -= spawnInterval;
		for (int i = 0; i < spawnCount; i++) {
			Vector3 pos = this->pos;
			pos += {
				Util::GetRand(-spawnAreaSize.x / 2.0f, spawnAreaSize.x / 2.0f),
				Util::GetRand(-spawnAreaSize.y / 2.0f, spawnAreaSize.y / 2.0f),
				Util::GetRand(-spawnAreaSize.z / 2.0f, spawnAreaSize.z / 2.0f)
			};

			Vector3 velocity = {
				Util::GetRand(minVelocity.x, maxVelocity.x),
				Util::GetRand(minVelocity.y, maxVelocity.y),
				Util::GetRand(minVelocity.z, maxVelocity.z)
			};

			Vector3 accel = {
				Util::GetRand(minAccel.x, minAccel.x),
				Util::GetRand(minAccel.y, minAccel.y),
				Util::GetRand(minAccel.z, minAccel.z)
			};

			Vector2 scale = {
				Util::GetRand(minScale.x, maxScale.x),
				Util::GetRand(minScale.y, maxScale.y)
			};

			float aliveTime = Util::GetRand(minAliveTime, maxAliveTime);

			manager->SpawnNormalParticle(pos, velocity, accel, scale, color, aliveTime, endColor, useSubtract);
		}
	}
}
