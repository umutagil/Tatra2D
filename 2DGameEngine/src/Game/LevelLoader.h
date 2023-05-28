#pragma once

class Registry;
class AssetStore;

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
};







