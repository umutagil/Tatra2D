#pragma once

#include "../ECS/ECS.h"
#include "../Utilities/Geometry.h"

class EventBus;

Aabb GetEntityAabb(const Entity& entity);

class CollisionSystem : public System
{
public:
	CollisionSystem();

	void Update(EventBus& eventBus);
};

