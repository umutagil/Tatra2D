#include "ProjectileEmitSystem.h"

#include "../Components/TransformComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/ProjectileComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/CameraFollowComponent.h"

#include "../EventBus/EventBus.h"
#include "../Events/KeyPressedEvent.h"

#include <SDL.h>

ProjectileEmitSystem::ProjectileEmitSystem()
	: pendingPlayerProjectile(false)
{
	RequireComponent<ProjectileEmitterComponent>();
	RequireComponent<TransformComponent>();
	RequireComponent<SpriteComponent>();
	RequireComponent<BoxColliderComponent>();
	RequireComponent<RigidBodyComponent>();
}

// TODO: Maybe find a cleaner way than sending registry as parameter.
void ProjectileEmitSystem::Update(std::unique_ptr<Registry>& registry)
{
 	const uint64_t currentTime = SDL_GetTicks64();

	for (Entity& entity : GetSystemEntities()) {

		const TransformComponent& transform = entity.GetComponent<TransformComponent>();
		ProjectileEmitterComponent& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
		const bool canFire = (currentTime - projectileEmitter.lastEmissiontime) > projectileEmitter.repeatFrequencyMs;

		if (!canFire) {
			continue;
		}

		glm::vec2 projectileVelocity(projectileEmitter.projectileVelocity);

		if (entity.HasComponent<CameraFollowComponent>()) {
			if (!pendingPlayerProjectile) {
				continue;
			}

			pendingPlayerProjectile = false;

			const RigidBodyComponent& rigidBody = entity.GetComponent<RigidBodyComponent>();
			const glm::vec2 playerDirection = glm::normalize(rigidBody.velocity);
			if (playerDirection != glm::vec2(0)) {
				projectileVelocity.x = playerDirection.x * projectileVelocity.x;
				projectileVelocity.y = playerDirection.y * projectileVelocity.y;
			}
		}

		glm::vec2 projectilePos = transform.position;
		if (entity.HasComponent<SpriteComponent>()) {
			const SpriteComponent& sprite = entity.GetComponent<SpriteComponent>();
			projectilePos.x += transform.scale.x * (sprite.width / 2.0f);
			projectilePos.y += transform.scale.y * (sprite.height / 2.0f);
		}

		const ProjectileInfo projectileInfo(projectilePos, glm::vec2(1.0f, 1.0f), projectileVelocity,
										projectileEmitter.hitDamage, projectileEmitter.projectileDurationMs, projectileEmitter.isFriendly);

		CreateProjectile(projectileInfo, registry);
		projectileEmitter.lastEmissiontime = currentTime;
	}
}

void ProjectileEmitSystem::SubscribeToEvents(EventBus& eventBus)
{
	eventBus.SubscribeToEvents(this, &ProjectileEmitSystem::OnKeyPressed);
}

void ProjectileEmitSystem::OnKeyPressed(KeyPressedEvent& event)
{
	switch (event.key) {

		case SDLK_SPACE: {
			const uint64_t currentTime = SDL_GetTicks64();

			for (Entity& entity : GetSystemEntities()) {

				if (!entity.HasComponent<CameraFollowComponent>()) {
					// Skip NPC entities.
					continue;
				}

				ProjectileEmitterComponent& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
				const bool canFire = (currentTime - projectileEmitter.lastEmissiontime) > projectileEmitter.repeatFrequencyMs;
				if (!canFire) {
					break;
				}

				pendingPlayerProjectile = true;
				break;
			}
			break;
		}

		default:
			break;
	}
}

void ProjectileEmitSystem::CreateProjectile(const ProjectileInfo& info, std::unique_ptr<Registry>& registry)
{
	Entity projectile = registry->CreateEntity();
	projectile.Tag("projectile");
	projectile.AddComponent<TransformComponent>(info.position, info.scale);
	projectile.AddComponent<RigidBodyComponent>(info.velocity);
	projectile.AddComponent<ProjectileComponent>(info.hitDamage, info.durationMs, info.isFriendly);
	projectile.AddComponent<SpriteComponent>("bullet-image", 4, 4, 4 /* zIndex */);
	projectile.AddComponent<BoxColliderComponent>(4, 4);
}



