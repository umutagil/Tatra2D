#pragma once

#include "BaseController.h"
#include "SceneManager.h"

class EditorController : public BaseController
{
public:

	EditorController(Game& game, Registry& registry);

	virtual void Initialize(SDL_Renderer& renderer, sol::state& lua) override;
	virtual void HandleEvent(SDL_Event& sdlEvent) override;
	virtual void Update(const unsigned deltaTimeMs) override;
	virtual void Render(SDL_Renderer& renderer) override;

	void CreateTileEntity();

private:
	SceneManager sceneManager;

};



