#pragma once

#include "../ECS/ECS.h"

class EventBus;
class CollisionEvent;

class MovementSystem : public System
{
public:
	MovementSystem();

	void Update(const float deltaTime);
	void SubscribeToEvents(EventBus& eventBus);

private:
	void OnCollisionHappened(CollisionEvent& event);
	void OnEnemyHitsObstacle(Entity enemy, Entity obstacle);
};

