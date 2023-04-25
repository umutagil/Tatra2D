#include "HealthDisplaySystem.h"

#include "../Components/HealthComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include "../AssetStore/AssetStore.h"

#include <string>
#include <glm/glm.hpp>
#include <SDL.h>
#include <utility>

const glm::ivec2 textOffset = { 0, 0 };
const glm::ivec2 healthBarOffset = { 0, 10 };
const glm::ivec2 healthBarDimensions = { 24, 4 };

const SDL_Color fullHealthColor = { 0, 255, 0, 255 };
const SDL_Color halfHealthColor = { 255, 195, 0, 255 };
const SDL_Color noHealthColor = { 255, 0, 0, 255 };

SDL_Color LerpColor(const SDL_Color& start, const SDL_Color& end, const float t)
{
	Uint8 r = static_cast<Uint8>((1.0f - t) * start.r + (t * end.r));
	Uint8 g = static_cast<Uint8>((1.0f - t) * start.g + (t * end.g));
	Uint8 b = static_cast<Uint8>((1.0f - t) * start.b + (t * end.b));
	Uint8 a = static_cast<Uint8>((1.0f - t) * start.a + (t * end.a));
	return { r, g, b, a };
}

HealthDisplaySystem::HealthDisplaySystem()
{
	RequireComponent<HealthComponent>();
	RequireComponent<TransformComponent>();
	RequireComponent<SpriteComponent>();
}

void HealthDisplaySystem::Update(SDL_Renderer& renderer, const AssetStore& assetStore, const SDL_Rect& camera)
{
	for (Entity entity : GetSystemEntities()) {
		const TransformComponent& transform = entity.GetComponent<TransformComponent>();
		const HealthComponent& health = entity.GetComponent<HealthComponent>();
		const SpriteComponent& sprite = entity.GetComponent<SpriteComponent>();

		const float healthRatio = static_cast<float>(health.health) / 100.0f;
		const SDL_Color healthColor = healthRatio >= 0.5f
									? LerpColor(halfHealthColor, fullHealthColor, (healthRatio - 0.5f) * 2.0f)
									: LerpColor(noHealthColor, halfHealthColor, healthRatio * 2.0f);

		SDL_SetRenderDrawColor(&renderer, healthColor.r, healthColor.g, healthColor.b, healthColor.a);

		// Update health text

		const std::string healthText(std::to_string(health.health) + "hp");
		SDL_Surface* surface = TTF_RenderText_Blended(assetStore.GetFont("charriot-font-10"), healthText.c_str(), healthColor);
		SDL_Texture* texture = SDL_CreateTextureFromSurface(&renderer, surface);
		SDL_FreeSurface(surface);

		const int textPosX = transform.position.x + textOffset.x - camera.x;
		const int textPosY = transform.position.y - (sprite.height * transform.scale.y / 2.0f) + textOffset.y - camera.y;

		int labelWidth = 0, labelHeight = 0;
		SDL_QueryTexture(texture, nullptr, nullptr, &labelWidth, &labelHeight);
		
		const SDL_Rect textRect = { textPosX, textPosY, labelWidth, labelHeight };
		SDL_RenderCopy(&renderer, texture, nullptr, &textRect);

		// Draw health bar frame

		const int barPosX = transform.position.x + healthBarOffset.x - camera.x;
		const int barPosY = transform.position.y - (sprite.height * transform.scale.y / 2.0f) + healthBarOffset.y - camera.y;
		const SDL_Rect healthBarFrameRect = { barPosX, barPosY, healthBarDimensions.x, healthBarDimensions.y };
		SDL_RenderDrawRect(&renderer, &healthBarFrameRect);

		// Draw filled part of the health bar

		SDL_Rect healthBarRect = healthBarFrameRect;
		healthBarRect.w *= healthRatio;
		SDL_RenderFillRect(&renderer, &healthBarRect);

		// Cleanup texture
		SDL_DestroyTexture(texture);
	}
}