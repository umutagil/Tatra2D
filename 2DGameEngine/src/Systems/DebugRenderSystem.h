#pragma once

#include "../ECS/ECS.h"

struct SDL_Renderer;
class AssetStore;
union SDL_Event;

class DebugRenderSystem : public System
{
public:
	DebugRenderSystem();

	void Update(SDL_Renderer* renderer);
	void HandleInput(const SDL_Event& event);

private:
	void DrawColliders(SDL_Renderer* renderer);

private:
	bool colliderDrawingEnabled;
};

