#pragma once

#include <cstdint>
#include <glm/glm.hpp>

struct ProjectileEmitterComponent
{
	ProjectileEmitterComponent(const glm::vec2 projectileVelocity = glm::vec2(0), const float repeatFrequencyS = 0.0f, const float projectileDurationS = 10,
								const int hitDamage = 10, const bool isFriendly = false);

	glm::vec2 projectileVelocity;
	float repeatFrequencyS;
	float projectileDurationS;
	int hitDamage;
	bool isFriendly;
	float cooldownTime;
};



