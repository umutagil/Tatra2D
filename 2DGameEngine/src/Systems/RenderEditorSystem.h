#pragma once

#include "../ECS/ECS.h"


struct SDL_Renderer;
struct SDL_Rect;
class AssetStore;
class SceneManager;

class RenderEditorSystem : public System
{
public:
	RenderEditorSystem();

	void Update(SceneManager& sceneManager, SDL_Renderer& renderer, const AssetStore& assetStore, const SDL_Rect& camera);

	void DrawSelectedTile(SceneManager& sceneManager, SDL_Renderer& renderer, const AssetStore& assetStore, const SDL_Rect& camera);
	void DrawGrid(const SceneManager& sceneManager, SDL_Renderer& renderer, const SDL_Rect& camera);
};
