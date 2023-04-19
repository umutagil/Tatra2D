#include "ProjectileComponent.h"

#include <SDL.h>

ProjectileComponent::ProjectileComponent(const int hitDamage, const unsigned projectileDurationS, const bool isFriendly)
	: hitDamage(hitDamage)
	, projectileDurationS(projectileDurationS)
	, isFriendly(isFriendly)
	, lifeTime(projectileDurationS)
{
}
