#pragma once

#include <string>
#include <glm/glm.hpp>
#include <SDL.h>

struct TileProperties
{
	int uvX;
	int uvY;
	int width;
	int height;
	std::string assetId;
};

/**
 * @brief A class for controlling the addition and removal of entities to the scene.
 */
class SceneManager
{
public:
	SceneManager() = default;

	void SetActiveTile(TileProperties properties);
	const TileProperties& GetActiveTile();
	const bool HasActiveTile();

	inline const SDL_Rect& GetCamera() const;
	void AddTile(const TileProperties& tileProperties);
	inline glm::ivec2 ScreenToWorld(const glm::ivec2& screenPos) const;
	inline glm::ivec2 WorldToScreen(const glm::ivec2& worldPos) const;

private:
	const SDL_Rect& camera;
	TileProperties activeTile;

};


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



