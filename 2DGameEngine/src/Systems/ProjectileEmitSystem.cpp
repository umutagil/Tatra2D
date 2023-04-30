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
void ProjectileEmitSystem::Update(std::unique_ptr<Registry>& registry, const float deltaTime)
{
	for (Entity& entity : GetSystemEntities()) {

		const TransformComponent& transform = entity.GetComponent<TransformComponent>();
		ProjectileEmitterComponent& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
		if (projectileEmitter.cooldownTime > 0.0f) {
			projectileEmitter.cooldownTime -= deltaTime;
		}
		const bool canFire = (projectileEmitter.cooldownTime <= 0.0f);
		if (!canFire) {
			continue;
		}

		glm::vec2 projectileVelocity(projectileEmitter.projectileVelocity);

		if (entity.HasTag("player")) {
			if (!pendingPlayerProjectile) {
				continue;
			}

			pendingPlayerProjectile = false;

			const RigidBodyComponent& rigidBody = entity.GetComponent<RigidBodyComponent>();
			const glm::vec2 playerDirection = rigidBody.velocity != glm::vec2(0)
											  ? glm::normalize(rigidBody.velocity)
											  : glm::vec2(0.0f, -1.0f);

			projectileVelocity.x = playerDirection.x * projectileVelocity.x;
			projectileVelocity.y = playerDirection.y * projectileVelocity.y;
		}

		glm::vec2 projectilePos = transform.position;
		if (entity.HasComponent<SpriteComponent>()) {
			const SpriteComponent& sprite = entity.GetComponent<SpriteComponent>();
			projectilePos.x += transform.scale.x * (sprite.width / 2.0f);
			projectilePos.y += transform.scale.y * (sprite.height / 2.0f);
		}

		const ProjectileInfo projectileInfo(projectilePos, glm::vec2(1.0f, 1.0f), projectileVelocity,
										projectileEmitter.hitDamage, projectileEmitter.projectileDurationS, projectileEmitter.isFriendly);

		CreateProjectile(projectileInfo, registry);
		projectileEmitter.cooldownTime = projectileEmitter.repeatFrequencyS;
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
			
			for (Entity& entity : GetSystemEntities()) {

				if (!entity.HasTag("player")) {
					// Skip NPC entities.
					continue;
				}

				ProjectileEmitterComponent& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
				const bool canFire = (projectileEmitter.cooldownTime <= 0);
				pendingPlayerProjectile = canFire;

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
	projectile.Group("projectiles");
	projectile.AddComponent<TransformComponent>(info.position, info.scale);
	projectile.AddComponent<RigidBodyComponent>(info.velocity);
	projectile.AddComponent<ProjectileComponent>(info.hitDamage, info.durationS, info.isFriendly);
	projectile.AddComponent<SpriteComponent>("bullet-texture", 4, 4, 4 /* zIndex */);
	projectile.AddComponent<BoxColliderComponent>(4, 4);
}



