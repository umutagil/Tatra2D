#include "MovementSystem.h"

#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"

#include "../EventBus/EventBus.h"
#include "../Events/CollisionEvent.h"

#include "../Game/Game.h"

#include <assert.h>


MovementSystem::MovementSystem()
{
	RequireComponent<TransformComponent>();
	RequireComponent<RigidBodyComponent>();
	RequireComponent<SpriteComponent>();
}

void MovementSystem::Update(const float deltaTime)
{
	// Update entity position based on its velocity
	// every frame of the game loop.

	for (Entity entity: GetSystemEntities()) {
		TransformComponent& transform = entity.GetComponent<TransformComponent>();
		const RigidBodyComponent& rigidbody = entity.GetComponent<RigidBodyComponent>();

		transform.position += rigidbody.velocity * deltaTime;
	}
}

void MovementSystem::SubscribeToEvents(EventBus& eventBus)
{
	eventBus.SubscribeToEvents(this, &MovementSystem::OnCollisionHappened);
}

void MovementSystem::OnCollisionHappened(CollisionEvent& event)
{
	Entity a = event.a;
	Entity b = event.b;

	if (a.BelongsToGroup("enemies") && b.BelongsToGroup("obstacles")) {
		OnEnemyHitsObstacle(a, b);
	}

	if (a.BelongsToGroup("obstacles") && b.BelongsToGroup("enemies")) {
		OnEnemyHitsObstacle(b, a);
	}
}

void MovementSystem::OnEnemyHitsObstacle(Entity enemy, Entity obstacle)
{
	assert(enemy.HasComponent<TransformComponent>());

	if (enemy.HasComponent<RigidBodyComponent>() && enemy.HasComponent<SpriteComponent>()) {
		RigidBodyComponent& rigidbody = enemy.GetComponent<RigidBodyComponent>();
		rigidbody.velocity *= -1.0f;

		SpriteComponent& sprite = enemy.GetComponent<SpriteComponent>();
		sprite.flip = (sprite.flip == SDL_FLIP_NONE) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

		// A little push to prevent enemy from being stuck in an endless collision loop.
		// To have more robust solution, we can move the enemy out of the box collider area of obstacle.
		TransformComponent& transform = enemy.GetComponent<TransformComponent>();
		transform.position += rigidbody.velocity * (1.0f / 120.0f);
	}

}
