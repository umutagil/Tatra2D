#include "MapEditSystem.h"

#include "../Components/SpriteComponent.h"
#include "../Components/TransformComponent.h"
#include "../Events/MouseEvents.h"
#include "../EventBus/EventBus.h"

#include "../Game/SceneManager.h"

#include "../Logger/Logger.h"

#include "../Utilities/Geometry.h"

MapEditSystem::MapEditSystem(SceneManager& sceneManager)
	: sceneManager(sceneManager)
{
	RequireComponent<SpriteComponent>();
}

void MapEditSystem::SubscribeToEvents(EventBus& eventBus)
{
	eventBus.SubscribeToEvents(this, &MapEditSystem::OnMouseButtonClicked);
	eventBus.SubscribeToEvents(this, &MapEditSystem::OnMouseMoved);
}

void MapEditSystem::OnMouseMoved(MouseMotionEvent& event)
{
	if (!sceneManager.HasActiveTile()) {
		return;
	}

	const glm::ivec2 mousePos(event.mousemotion.x, event.mousemotion.y);
	glm::ivec2 mousePosWorld = sceneManager.ScreenToWorld(mousePos);

	const TileProperties& tileProperties = sceneManager.GetActiveTile();
	const glm::ivec2 tileScrenPos(mousePos.x - (tileProperties.width / 2), mousePos.y - (tileProperties.height / 2));
	glm::ivec2 tilePosWorld = sceneManager.ScreenToWorld(tileScrenPos);

	if (sceneManager.GetGridProperties().isInside(mousePosWorld.x, mousePosWorld.y)) {
		if (sceneManager.snapToGrid) {
			const GridProperties& gridProperties = sceneManager.GetGridProperties();
			const glm::ivec2 mousePosWorld = sceneManager.ScreenToWorld(mousePos);
			glm::ivec2 tilePosRelative = mousePosWorld - gridProperties.startPos;
			tilePosRelative = (tilePosRelative / static_cast<int>(gridProperties.cellSize)) * static_cast<int>(gridProperties.cellSize);
			tilePosWorld = gridProperties.startPos + tilePosRelative;
		}
	}

	// Update position of tile
	sceneManager.SetActiveTilePos(tilePosWorld);
}

void MapEditSystem::OnMouseButtonClicked(MouseButtonEvent& event)
{
	if (event.mouseButton.type != SDL_MOUSEBUTTONDOWN) {
		return;
	}

	if (event.mouseButton.button != SDL_BUTTON_LEFT) {
		return;
	}

	if (!sceneManager.HasActiveTile()) {
		return;
	}

	const glm::ivec2 mousePosWorld = sceneManager.ScreenToWorld({ event.mouseButton.x, event.mouseButton.y });
	if (!sceneManager.GetGridProperties().isInside(mousePosWorld.x, mousePosWorld.y)) {
		return;
	}

	const TileProperties& activeTile = sceneManager.GetActiveTile();
	const glm::ivec2 tileCenterPos = activeTile.posWorld + glm::ivec2(activeTile.width / 2, activeTile.height / 2);

	// Check if there's a tile on that position
	for (Entity entity : GetSystemEntities()) {
		if (!entity.HasTag("tile")) {
			continue;
		}

		const TransformComponent& transform = entity.GetComponent<TransformComponent>();
		const SpriteComponent& sprite = entity.GetComponent<SpriteComponent>();
		const glm::vec2 maxPos(transform.position + glm::vec2(sprite.width, sprite.height));
		const Aabb entitySpriteAabb(transform.position, maxPos);

		if (entitySpriteAabb.Contains(tileCenterPos)) {
			entity.Kill();
		}
	}

	sceneManager.AddActiveTile();
}
