#include "RenderSystem.h"

#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include "../AssetStore/AssetStore.h"

#include <SDL.h>
#include <algorithm>
#include <assert.h>

RenderSystem::RenderSystem()
{
	RequireComponent<TransformComponent>();
	RequireComponent<SpriteComponent>();
}

void RenderSystem::Update(SDL_Renderer& renderer, const AssetStore& assetStore, const SDL_Rect& camera)
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
		const int entityWidth = sprite.width * static_cast<int>(transform.scale.x);
		const int entityHeight = sprite.height * static_cast<int>(transform.scale.y);

		const bool isEntityOutOfCameraView = ((entityPosX + entityWidth) < 0) || (entityPosX > camera.w) || 
											 ((entityPosY + entityHeight) < 0) || (entityPosY > camera.h);

		if (isEntityOutOfCameraView) {
			// Cull the sprites that are out of camera view.
			assert(!sprite.isFixed);
			continue;
		}

		const SDL_Rect destRect = {
			entityPosX,
			entityPosY,
			entityWidth,
			entityHeight
		};

		SDL_RenderCopyEx(
			&renderer,
			assetStore.GetTexture(sprite.assetId),
			&sprite.srcRect,
			&destRect,
			transform.rotation,
			nullptr,
			sprite.flip);
	}
}
