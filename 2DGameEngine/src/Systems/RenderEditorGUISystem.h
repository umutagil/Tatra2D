#pragma once

#include "../ECS/ECS.h"

#include <glm/glm.hpp>
#include <string>

struct SDL_Rect;
struct SDL_Renderer;

class AssetStore;
class SceneManager;

class RenderEditorGUISystem : public System
{
public:
	RenderEditorGUISystem() = default;

	void Update(Registry& registry, SceneManager& sceneManager, SDL_Renderer& renderer, AssetStore& assetStore);

private:
	void DisplayLoadedTileset(SceneManager& sceneManager, AssetStore& assetStore);


private:
	std::string activeTilesetId;
	glm::ivec2 activeTilesetDimensions;

};


