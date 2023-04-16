#include "ProjectileLifeCycleSystem.h"

#include <SDL.h>

#include "../Components/ProjectileComponent.h"

ProjectileLifeCycleSystem::ProjectileLifeCycleSystem()
{
	RequireComponent<ProjectileComponent>();
}

void ProjectileLifeCycleSystem::Update()
{
	const uint64_t currentTime = SDL_GetTicks64();

	for (Entity entity : GetSystemEntities()) {
		ProjectileComponent& projectile = entity.GetComponent<ProjectileComponent>();

		if (currentTime - projectile.startTime > projectile.projectileDurationMs) {
			entity.Kill();
		}
	}
}
