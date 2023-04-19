#include "ProjectileEmitterComponent.h"

#include <SDL.h>

ProjectileEmitterComponent::ProjectileEmitterComponent(const glm::vec2 projectileVelocity, const float repeatFrequencyS, const float projectileDurationS,
														const int hitDamage, const bool isFriendly)
	: projectileVelocity(projectileVelocity)
	, repeatFrequencyS(repeatFrequencyS)
	, projectileDurationS(projectileDurationS)
	, hitDamage(hitDamage)
	, cooldownTime(0)
	, isFriendly(isFriendly)
{
}
