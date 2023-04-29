#pragma once

#include "../ECS/ECS.h"

#include "../EventBus/EventBus.h"
#include "../Events/CollisionEvent.h"

class DamageSystem : public System
{
public:
	DamageSystem();

	void SubscribeToEvents(EventBus& eventBus);

private:

	void OnCollisionHappened(CollisionEvent& event);
	void OnProjectileHitsPlayer(Entity projectile, Entity player);
	void OnProjectileHitsEnemy(Entity projectile, Entity enemy);
};

