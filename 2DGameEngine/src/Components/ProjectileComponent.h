#pragma once

#include <cstdint>

struct ProjectileComponent
{
	ProjectileComponent(const int hitDamage = 0, const float projectileDurationS = 0.0f, const bool isFriendly = false)
		: hitDamage(hitDamage)
		, projectileDurationS(projectileDurationS)
		, isFriendly(isFriendly)
		, lifeTime(projectileDurationS)
	{
	}

	int hitDamage;
	float projectileDurationS;
	bool isFriendly;
	float lifeTime;
};



