#pragma once

#include <string>

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

	void AddTile(const TileProperties& tileProperties);

private:
	TileProperties activeTile;

};





