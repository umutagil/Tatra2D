#pragma once

#include "../ECS/ECS.h"

struct SDL_Renderer;
struct SDL_Rect;
class AssetStore;

class HealthDisplaySystem : public System
{
public:
	HealthDisplaySystem();

	void Update(SDL_Renderer& renderer, const AssetStore& assetStore, const SDL_Rect& camera);

private:
	void DrawHealthBar();
};

