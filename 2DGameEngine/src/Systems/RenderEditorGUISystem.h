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

	void Update(SceneManager& sceneManager, SDL_Renderer& renderer, AssetStore& assetStore, const SDL_Rect& camera);

private:
	void DisplayLoadedTileset(SceneManager& sceneManager, AssetStore& assetStore);


private:
	std::string activeTilesetId;
	glm::ivec2 activeTilesetDimensions;

};


