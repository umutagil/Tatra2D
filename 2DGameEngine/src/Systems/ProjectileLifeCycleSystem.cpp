#include "ProjectileLifeCycleSystem.h"

#include "../Components/ProjectileComponent.h"

ProjectileLifeCycleSystem::ProjectileLifeCycleSystem()
{
	RequireComponent<ProjectileComponent>();
}

void ProjectileLifeCycleSystem::Update(const float deltaTime)
{
	for (Entity entity : GetSystemEntities()) {
		ProjectileComponent& projectile = entity.GetComponent<ProjectileComponent>();
		projectile.lifeTime -= deltaTime;
		if (projectile.lifeTime <= 0) {
			entity.Kill();
		}
	}
}
