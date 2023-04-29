#include "MovementSystem.h"

#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"

#include "../EventBus/EventBus.h"
#include "../Events/CollisionEvent.h"

#include "../Game/Game.h"

#include <assert.h>

const float OutOfMapEntityKillMargin = 80.0f;

static bool isInRange(const float value, const float min, const float max)
{
	assert(min <= max);
	return (value >= min) && (value <= max);
}

static bool isFullyInsideMap(const glm::vec2& pos, const glm::vec2 size = glm::vec2(0.0f))
{
	const float rightBorder = Game::mapWidth - size.x;
	const float bottomBorder = Game::mapHeight - size.y;
	return isInRange(pos.x, 0, rightBorder) && isInRange(pos.y, 0, bottomBorder);
}

static bool isFullyOutsideMap(const glm::vec2& pos, const glm::vec2 size = glm::vec2(0.0f), const float margin = 0.0f)
{
	const float rightBorder = Game::mapWidth + size.x + margin;
	const float bottomBorder = Game::mapHeight + size.y + margin;
	return !isInRange(pos.x, -size.x, rightBorder) && !isInRange(pos.y, -size.y, bottomBorder);
}

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

	const glm::vec2 mapMin(0.0f);
	const glm::vec2 mapMax(Game::mapWidth - 32, Game::mapHeight - 32);

	for (Entity entity: GetSystemEntities()) {
		TransformComponent& transform = entity.GetComponent<TransformComponent>();
		const RigidBodyComponent& rigidbody = entity.GetComponent<RigidBodyComponent>();
		const SpriteComponent& sprite = entity.GetComponent<SpriteComponent>();
		const glm::vec2 entitySize((sprite.width * transform.scale.x), (sprite.height * transform.scale.y));

		const glm::vec2 deltaPosition = rigidbody.velocity * deltaTime;
		transform.position += deltaPosition;

		if (entity.HasTag("player")) {
			if (!isFullyInsideMap(transform.position, entitySize)) {
				// Prevent player to pass map borders
				transform.position -= deltaPosition;
			}
		}
		else if (entity.BelongsToGroup("projectiles")) {
			if (isFullyOutsideMap(transform.position, entitySize)) {
				entity.Kill();
			}
		}
		else if (entity.BelongsToGroup("enemies")) {
			if (isFullyOutsideMap(transform.position, entitySize, OutOfMapEntityKillMargin)) {
				entity.Kill();
			}
		}
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
