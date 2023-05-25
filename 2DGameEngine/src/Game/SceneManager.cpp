#include "SceneManager.h"

#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include "../ECS/ECS.h"

#include <assert.h>

SceneManager::SceneManager(Registry& registry, const SDL_Rect& camera)
	: registry(registry)
	, camera(camera)
	, activeTile()
	, gridProperties()
	, snapToGrid(false)
{
}

// TODO: move construct, RVO check
void SceneManager::SetActiveTile(TileProperties properties)
{
	activeTile = properties;
}

void SceneManager::SetActiveTilePos(const glm::ivec2& posWorld)
{
	activeTile.posWorld = posWorld;
}

// TODO: Change to generic add entity function
// tag can be retrieved from dropdown list, values can be stored in const array
// Add layer as parameter to tile properties and also this can be imgui parameter
// Basically we can add whatever needed to imgui as it will be entity creation panel.
void SceneManager::AddActiveTile()
{
	assert(HasActiveTile());

	Entity tile = registry.CreateEntity();
	tile.Tag("tile");
	tile.AddComponent<TransformComponent>(activeTile.posWorld, glm::vec2(1, 1), 0.0f);
	tile.AddComponent<SpriteComponent>(activeTile.assetId, activeTile.width, activeTile.height, 1, false, activeTile.uvX, activeTile.uvY);
}

bool GridProperties::isInside(const int x, const int y) const
{
	const unsigned endPosX = startPos.x + (cellCountX * cellSize);
	const unsigned endPosY = startPos.y + (cellCountY * cellSize);

	return x >= startPos.x && x <= endPosX && y >= startPos.y && y <= endPosY;
}
