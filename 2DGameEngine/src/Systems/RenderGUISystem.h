#pragma once

#include "../ECS/ECS.h"

#include <glm/glm.hpp>

struct SDL_Rect;

class RenderGUISystem : public System
{

public:
	RenderGUISystem() = default;

	void Update(Registry& registry, const SDL_Rect& camera);
	
private:
	struct EnemyProperties;
	void CreateEnemy(Registry& registry, const EnemyProperties& properties);

private:
	struct EnemyProperties
	{
		// Transform component parameters
		glm::ivec2 position = glm::vec2(0);
		glm::vec2 scale = glm::vec2(1);
		float rotation = 0;

		// Rigidbody component parameters
		glm::vec2 velocity = glm::vec2(0);

		// Sprite component parameters
		std::string assetId;

		// Projectile Emitter component parameters
		glm::vec2 projectileVelocity = glm::vec2(0);
		float repeatFrequencyS = 0;
		float projectileDurationS = 0;
		int hitDamage = 0;
		bool isFriendly = false;

		// Health component parameters
		int health = 0;
	};

};


