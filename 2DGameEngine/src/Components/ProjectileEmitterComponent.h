#pragma once

#include <cstdint>
#include <glm/glm.hpp>

struct ProjectileEmitterComponent
{
	ProjectileEmitterComponent(const glm::vec2 projectileVelocity = glm::vec2(0), const unsigned repeatFrequencyMs = 0, const unsigned projectileDurationMs = 10000,
								const unsigned hitDamage = 10, const bool isFriendly = false);

	glm::vec2 projectileVelocity;
	unsigned repeatFrequencyMs;
	unsigned projectileDurationMs;
	unsigned hitDamage;
	bool isFriendly;
	uint64_t lastEmissiontime;
};



