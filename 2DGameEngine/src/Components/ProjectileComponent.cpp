#include "ProjectileComponent.h"

#include <SDL.h>

ProjectileComponent::ProjectileComponent(const unsigned hitDamage, const unsigned projectileDurationMs, const bool isFriendly)
	: hitDamage(hitDamage)
	, projectileDurationMs(projectileDurationMs)
	, isFriendly(isFriendly)
	, startTime(SDL_GetTicks64())
{
}
