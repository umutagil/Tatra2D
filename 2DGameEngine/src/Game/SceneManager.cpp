#include "SceneManager.h"

// TODO: move construct, RVO check
void SceneManager::SetActiveTile(TileProperties properties)
{
	activeTile = properties;
}

const TileProperties& SceneManager::GetActiveTile()
{
	return activeTile;
}

const bool SceneManager::HasActiveTile()
{
	return activeTile.width != 0;
}

void SceneManager::AddTile(const TileProperties& tileProperties)
{

}
