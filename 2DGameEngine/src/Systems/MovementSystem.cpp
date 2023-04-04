#include "MovementSystem.h"

#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"

MovementSystem::MovementSystem()
{
	RequireComponent<TransformComponent>();
	RequireComponent<RigidBodyComponent>();
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
