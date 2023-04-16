#pragma once

#include <cstdint>

struct ProjectileComponent
{
	ProjectileComponent(const unsigned hitDamage = 0, const unsigned projectileDurationMs = 0, const bool isFriendly = false);

	unsigned hitDamage;
	unsigned projectileDurationMs;
	bool isFriendly;
	uint64_t startTime;
};



