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

void RenderSystem::Update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore)
{
	// Sort the entities according to zIndex
	std::vector<Entity> entitiesSorted(GetSystemEntities());
	std::sort(entitiesSorted.begin(), entitiesSorted.end(), [](Entity a, Entity b) {
		return a.GetComponent<SpriteComponent>().zIndex < b.GetComponent<SpriteComponent>().zIndex;
	});

	for (Entity entity : entitiesSorted) {
		const TransformComponent& transform = entity.GetComponent<TransformComponent>();
		const SpriteComponent& sprite = entity.GetComponent<SpriteComponent>();

		const SDL_Rect destRect = {
			static_cast<int>(transform.position.x),
			static_cast<int>(transform.position.y),
			static_cast<int>(sprite.width * transform.scale.x),
			static_cast<int>(sprite.height * transform.scale.y)
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
