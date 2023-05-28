#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <SDL.h>

class Registry;

struct TileProperties
{
	glm::ivec2 posWorld;
	int uvX;
	int uvY;
	int width;
	int height;
	int srcIndexLinear;
	std::string assetId;
};

struct GridProperties
{
	glm::ivec2 startPos = glm::ivec2(0, 0);
	unsigned cellCountX = 20;
	unsigned cellCountY = 20;
	unsigned cellSize = 32;

	const SDL_Color cellColor1 = { 200, 200, 200, 70 };
	const SDL_Color cellColor2 = { 20, 200, 230, 70 };

	bool isInside(const int x, const int y) const;
};

/**
 * @brief A class for controlling the addition and removal of entities to the scene.
 */
class SceneManager
{
public:
	using TileMapArray = std::vector<std::vector<int>>;

	SceneManager(Registry& registry, const SDL_Rect& camera);

	void SetActiveTile(TileProperties properties);
	inline const TileProperties& GetActiveTile();
	inline const bool HasActiveTile();
	void SetActiveTilePos(const glm::ivec2& posWorld);

	inline const SDL_Rect& GetCamera() const;
	inline glm::ivec2 ScreenToWorld(const glm::ivec2& screenPos) const;
	inline glm::ivec2 WorldToScreen(const glm::ivec2& worldPos) const;

	inline const GridProperties& GetGridProperties() const;
	inline const TileMapArray& GetTileMapIndices() const;

	void AddActiveTile();

private:
	Registry& registry;
	const SDL_Rect& camera;

	TileProperties activeTile;
	GridProperties gridProperties;
	
	TileMapArray tileMapIndices;

public:
	bool snapToGrid = false;
};


const TileProperties& SceneManager::GetActiveTile()
{
	return activeTile;
}

const bool SceneManager::HasActiveTile()
{
	return !activeTile.assetId.empty();
}

inline const SDL_Rect& SceneManager::GetCamera() const
{
	return camera;
}

inline glm::ivec2 SceneManager::ScreenToWorld(const glm::ivec2& screenPos) const
{
	return glm::ivec2(screenPos.x + camera.x, screenPos.y + camera.y);
}

inline glm::ivec2 SceneManager::WorldToScreen(const glm::ivec2& worldPos) const
{
	return glm::ivec2(worldPos.x - camera.x, worldPos.y - camera.y);
}

inline const GridProperties& SceneManager::GetGridProperties() const
{
	return gridProperties;
}

inline const SceneManager::TileMapArray& SceneManager::GetTileMapIndices() const
{
	return tileMapIndices;
}




