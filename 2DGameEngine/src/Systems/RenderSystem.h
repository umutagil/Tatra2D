#pragma once

#include "../ECS/ECS.h"


struct SDL_Renderer;
struct SDL_Rect;
class AssetStore;

class RenderSystem : public System
{
public:
	RenderSystem();

	void Update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore, SDL_Rect& camera);
};

