#pragma once

#include "BaseController.h"

class GameController : public BaseController
{
public:
	GameController(Game& game, Registry& registry);

	virtual void Initialize(SDL_Renderer& renderer, sol::state& lua) override;
	virtual void HandleEvent(SDL_Event& sdlEvent) override;
	virtual void Update(const unsigned deltaTimeMs) override;
	virtual void Render(SDL_Renderer& renderer) override;

private:
	bool isDebugModeOn;
};

