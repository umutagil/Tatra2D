#include "CollisionSystem.h"

#include "../Components/BoxColliderComponent.h"
#include "../Components/TransformComponent.h"

#include "../EventBus/EventBus.h"
#include "../Events/CollisionEvent.h"

#include "../Logger/Logger.h"

CollisionSystem::CollisionSystem()
{
	RequireComponent<BoxColliderComponent>();
	RequireComponent<TransformComponent>();
}

void CollisionSystem::Update(EventBus& eventBus)
{
	std::vector<Entity>& entities(GetSystemEntities());

	for (auto it = entities.begin(); it != entities.end(); ++it) {
		const Entity& entity(*it);
		const Aabb aabb(GetEntityAabb(entity));

		for (auto itOther = it + 1; itOther != entities.end(); ++itOther) {
			const Entity& entityOther(*itOther);

			const Aabb aabbOther(GetEntityAabb(entityOther));
			if (aabb.Overlaps(aabbOther)) {
				eventBus.EmitEvents<CollisionEvent>(CollisionEvent(entity, entityOther));
			}
		}
	}
}

Aabb GetEntityAabb(const Entity& entity)
{
	const TransformComponent& transform = entity.GetComponent<TransformComponent>();
	const BoxColliderComponent& collider = entity.GetComponent<BoxColliderComponent>();

	const glm::vec2 minPos = transform.position + collider.offset;
	const glm::vec2 maxPos = minPos + glm::vec2(collider.width, collider.height) + collider.offset;
	return Aabb(minPos, maxPos);
}
