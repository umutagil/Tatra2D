#pragma once

#include "../ECS/ECS.h"
#include <glm/glm.hpp>

class EventBus;
class KeyPressedEvent;

struct ProjectileInfo
{
	ProjectileInfo()
		: position(glm::vec2(0))
		, scale(glm::vec2(1.0f, 1.0f))
		, velocity(glm::vec2(0))
		, hitDamage(0)
		, durationMs(0)
		, isFriendly(false)
	{
	}

	ProjectileInfo(const glm::vec2& position, const glm::vec2& scale, const glm::vec2& velocity, const unsigned hitDamage, const unsigned durationMs, const bool isFriendly)
		: position(position)
		, scale(scale)
		, velocity(velocity)
		, hitDamage(hitDamage)
		, durationMs(durationMs)
		, isFriendly(isFriendly)
	{
	}

	glm::vec2 position;
	glm::vec2 scale;
	glm::vec2 velocity;
	unsigned hitDamage;
	unsigned durationMs;
	bool isFriendly;
};

class ProjectileEmitSystem : public System
{

public:
	ProjectileEmitSystem();

	void Update(std::unique_ptr<Registry>& registry);

	void SubscribeToEvents(EventBus& eventBus);

private:
	void OnKeyPressed(KeyPressedEvent& event);

	void CreateProjectile(const ProjectileInfo& info, std::unique_ptr<Registry>& registry);

private:
	bool pendingPlayerProjectile;
};



