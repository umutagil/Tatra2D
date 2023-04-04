#pragma once

#include "../ECS/ECS.h"

#include <glm/glm.hpp>

class EventBus;

struct Aabb
{
	Aabb(const glm::vec2& min, const glm::vec2& max);
	bool Overlaps(const Aabb& aabb) const;
	float GetWidth() const;
	float GetHeight() const;

	glm::vec2 min;
	glm::vec2 max;
};

Aabb GetEntityAabb(const Entity& entity);

class CollisionSystem : public System
{
public:
	CollisionSystem();

	void Update(EventBus& eventBus);
};

