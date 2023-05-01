#pragma once

#include <cstdint>

class Game;
class Registry;
class AssetStore;

struct SDL_Renderer;

union SDL_Event;

namespace sol
{
	class state;
};

class BaseController
{
public:
	BaseController(Game& game, Registry& registry);

	virtual void Initialize(SDL_Renderer& renderer, sol::state& lua) = 0;
	virtual void HandleEvent(SDL_Event& sdlEvent) = 0;
	virtual void Update(const unsigned deltaTimeMs) = 0;
	virtual void Render(SDL_Renderer& renderer) = 0;

protected:
	Game& game;
	Registry& registry;

};


