#include "ProjectileEmitterComponent.h"

#include <SDL.h>

ProjectileEmitterComponent::ProjectileEmitterComponent(const glm::vec2 projectileVelocity, const unsigned repeatFrequencyMs, const unsigned projectileDurationMs,
														const unsigned hitDamage, const bool isFriendly)
	: projectileVelocity(projectileVelocity)
	, repeatFrequencyMs(repeatFrequencyMs)
	, projectileDurationMs(projectileDurationMs)
	, hitDamage(hitDamage)
	, lastEmissiontime(SDL_GetTicks64())
	, isFriendly(isFriendly)
{
}
