#pragma once

#include <cstdint>

struct ProjectileComponent
{
	ProjectileComponent(const int hitDamage = 0, const float projectileDurationS = 0.0f, const bool isFriendly = false);

	int hitDamage;
	unsigned projectileDurationS;
	bool isFriendly;
	float lifeTime;
};



