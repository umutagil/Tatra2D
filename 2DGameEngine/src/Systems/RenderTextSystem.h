#pragma once

#include "../ECS/ECS.h"

struct SDL_Rect;
struct SDL_Renderer;
class AssetStore;

class RenderTextSystem : public System
{
public:
	RenderTextSystem();

	void Update(SDL_Renderer& renderer, const AssetStore& assetStore, const SDL_Rect& camera);

};



