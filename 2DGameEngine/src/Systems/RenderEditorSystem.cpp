#include "RenderEditorSystem.h"

#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include "../AssetStore/AssetStore.h"
#include "../Game/SceneManager.h"

#include <SDL.h>
#include <algorithm>
#include <assert.h>

void SetRenderDrawColor(SDL_Renderer& renderer, SDL_Color color)
{
	SDL_SetRenderDrawColor(&renderer, color.r, color.g, color.b, color.a);
}

RenderEditorSystem::RenderEditorSystem()
{
	RequireComponent<TransformComponent>();
	RequireComponent<SpriteComponent>();
}

void RenderEditorSystem::Update(SceneManager& sceneManager, SDL_Renderer& renderer, const AssetStore& assetStore, const SDL_Rect& camera)
{
	DrawGrid(sceneManager, renderer, camera);

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
			sprite.flip
		);
	}

	if (sceneManager.HasActiveTile()) {
		DrawSelectedTile(sceneManager, renderer, assetStore, camera);
	}
}

void RenderEditorSystem::DrawSelectedTile(SceneManager& sceneManager, SDL_Renderer& renderer, const AssetStore& assetStore, const SDL_Rect& camera)
{
	int mouseX, mouseY;
	SDL_GetMouseState(&mouseX, &mouseY);

	const GridProperties& gridProperties = sceneManager.GetGridProperties();
	const TileProperties& tileProperties = sceneManager.GetActiveTile();
	const glm::ivec2 tilePosScreen = sceneManager.WorldToScreen(tileProperties.posWorld);

	const SDL_Rect srcRect = {
			tileProperties.uvX,
			tileProperties.uvY,
			tileProperties.width,
			tileProperties.height
	};

	const SDL_Rect destRect = {
		tilePosScreen.x,
		tilePosScreen.y,
		tileProperties.width,
		tileProperties.height
	};

	SDL_RenderCopyEx(
		&renderer,
		assetStore.GetTexture(tileProperties.assetId),
		&srcRect,
		&destRect,
		0,
		nullptr,
		SDL_FLIP_NONE
	);

}

void RenderEditorSystem::DrawGrid(const SceneManager& sceneManager, SDL_Renderer& renderer, const SDL_Rect& camera)
{
	const GridProperties& gridProperties = sceneManager.GetGridProperties();

	for (size_t i = 0; i < gridProperties.cellCountY; ++i) {
		for (size_t j = 0; j < gridProperties.cellCountX; ++j) {

			const int xPosWorld = gridProperties.startPos.x + (j * gridProperties.cellSize);
			const int yPosWorld = gridProperties.startPos.y + (i * gridProperties.cellSize);
			const glm::ivec2 posScreen = sceneManager.WorldToScreen({ xPosWorld, yPosWorld });
			const SDL_Rect cell = { posScreen.x, posScreen.y, gridProperties.cellSize, gridProperties.cellSize};

			const SDL_Color& currentCellColor = ((i + j) % 2) ? gridProperties.cellColor1 : gridProperties.cellColor2;
			SetRenderDrawColor(renderer, currentCellColor);
			SDL_RenderDrawRect(&renderer, &cell);
			SDL_RenderFillRect(&renderer, &cell);
		}
	}

}
