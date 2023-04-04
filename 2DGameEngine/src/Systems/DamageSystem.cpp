#include "DamageSystem.h"

#include "../Components/BoxColliderComponent.h"

#include "../Logger/Logger.h"

#include<string>

DamageSystem::DamageSystem()
{
	RequireComponent<BoxColliderComponent>();
}

void DamageSystem::SubscribeToEvents(EventBus& eventBus)
{
	eventBus.SubscribeToEvent<CollisionEvent>(this, &DamageSystem::OnCollisionHappened);
}

void DamageSystem::OnCollisionHappened(CollisionEvent& event)
{
	Logger::Log("Damage system received collision. Entities (" + std::to_string(event.a.GetId()) + ", " + std::to_string(event.b.GetId()) + ")");
	event.a.Kill();
	event.b.Kill();
}
