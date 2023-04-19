#include "DamageSystem.h"

#include "../Components/BoxColliderComponent.h"
#include "../Components/ProjectileComponent.h"
#include "../Components/HealthComponent.h"

#include "../Logger/Logger.h"

#include<string>

DamageSystem::DamageSystem()
{
	RequireComponent<BoxColliderComponent>();
}

void DamageSystem::SubscribeToEvents(EventBus& eventBus)
{
	eventBus.SubscribeToEvents(this, &DamageSystem::OnCollisionHappened);
}

void DamageSystem::OnCollisionHappened(CollisionEvent& event)
{
	Entity a = event.a;
	Entity b = event.b;

	if (a.BelongsToGroup("projectiles") && b.HasTag("player")) {
		OnProjectileHitsPlayer(a, b);
	}

	if (b.BelongsToGroup("projectiles") && a.HasTag("player")) {
		OnProjectileHitsPlayer(b, a);
	}

	if (a.BelongsToGroup("projectiles") && b.BelongsToGroup("enemies")) {
		OnProjectileHitsEnemy(a, b);
	}

	if (b.BelongsToGroup("projectiles") && a.BelongsToGroup("enemies")) {
		OnProjectileHitsEnemy(b, a);
	}
}

void DamageSystem::OnProjectileHitsPlayer(Entity projectile, Entity player)
{
	Logger::Log("Player received hit. Entities (" + std::to_string(projectile.GetId()) + ", " + std::to_string(player.GetId()) + ")");

	const ProjectileComponent& projectileComp = projectile.GetComponent<ProjectileComponent>();
	if (!projectileComp.isFriendly) {
		HealthComponent& healthComp = player.GetComponent<HealthComponent>();
		healthComp.health -= projectileComp.hitDamage;

		if (healthComp.health <= 0) {
			player.Kill();
		}

		projectile.Kill();
	}
}

void DamageSystem::OnProjectileHitsEnemy(Entity projectile, Entity enemy)
{
	const ProjectileComponent& projectileComp = projectile.GetComponent<ProjectileComponent>();
	if (projectileComp.isFriendly) {
		HealthComponent& healthComp = enemy.GetComponent<HealthComponent>();
		healthComp.health -= projectileComp.hitDamage;

		if (healthComp.health <= 0) {
			enemy.Kill();
		}

		projectile.Kill();
	}
}
