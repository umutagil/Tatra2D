#include "RenderSystem.h"

#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include "../AssetStore/AssetStore.h"

#include <SDL.h>
#include <algorithm>

RenderSystem::RenderSystem()
{
	RequireComponent<TransformComponent>();
	RequireComponent<SpriteComponent>();
}

void RenderSystem::Update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore, SDL_Rect& camera)
{
	// Sort the entities according to zIndex
	std::vector<Entity> entitiesSorted(GetSystemEntities());
	std::sort(entitiesSorted.begin(), entitiesSorted.end(), [](Entity a, Entity b) {
		return a.GetComponent<SpriteComponent>().zIndex < b.GetComponent<SpriteComponent>().zIndex;
	});

	for (Entity entity : entitiesSorted) {
		const TransformComponent& transform = entity.GetComponent<TransformComponent>();
		const SpriteComponent& sprite = entity.GetComponent<SpriteComponent>();

		const int entityPosX = static_cast<int>(transform.position.x) - (sprite.isFixed ? 0 : camera.x);
		const int entityPosY = static_cast<int>(transform.position.y) - (sprite.isFixed ? 0 : camera.y);

		const SDL_Rect destRect = {
			entityPosX,
			entityPosY,
			sprite.width * static_cast<int>(transform.scale.x),
			sprite.height * static_cast<int>(transform.scale.y)
		};

		SDL_RenderCopyEx(
			renderer,
			assetStore->GetTexture(sprite.assetId),
			&sprite.srcRect,
			&destRect,
			transform.rotation,
			nullptr,
			SDL_FLIP_NONE);
	}
}
