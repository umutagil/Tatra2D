#pragma once

#include "../ECS/ECS.h"

struct SDL_Renderer;
struct SDL_Rect;
class AssetStore;
union SDL_Event;

class DebugRenderSystem : public System
{
public:
	DebugRenderSystem();

	void Update(SDL_Renderer* renderer, SDL_Rect& camera);
	void HandleInput(const SDL_Event& event);

private:
	void DrawColliders(SDL_Renderer* renderer, SDL_Rect& camera);

private:
	bool colliderDrawingEnabled;
};

