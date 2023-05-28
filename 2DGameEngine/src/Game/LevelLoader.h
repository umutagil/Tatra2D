#pragma once

#include <string>

class Registry;
class AssetStore;
class SceneManager;

namespace sol
{
	class state;
};

struct SDL_Renderer;

class LevelLoader
{
public:
	LevelLoader();
	~LevelLoader();

	static void LoadLevel(sol::state& lua, Registry& registry, AssetStore& assetStore, SDL_Renderer& renderer, const int levelIndex);

	static void SaveMap(const std::string& mapFileName, Registry& registry, SceneManager& sceneManager);
};







