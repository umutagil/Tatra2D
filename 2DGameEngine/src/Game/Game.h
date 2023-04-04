#pragma once

#include <cstdint>
#include <memory>

#include "../ECS/ECS.h"

const unsigned FPS = 120;
const unsigned MILISEC_PER_FRAME = 1000 / FPS;

struct SDL_Window;
struct SDL_Renderer;

class AssetStore;
class EventBus;

class Game
{

public:
	Game();
	~Game();

	void Initialize();
	void Run();
	void ProcessInput();
	void Setup();
	void Update();
	void Render();
	void Destroy();

private:
	void LoadLevel(const unsigned level);
	void LoadTileMap();

private:
	
	bool isRunning;
	std::uint64_t milisecsPreviousFrame = 0;

	SDL_Window* window;
	SDL_Renderer* renderer;

	int windowWidth;
	int windowHeight;

	std::unique_ptr<Registry> registry;
	std::unique_ptr<AssetStore> assetStore;
	std::unique_ptr<EventBus> eventBus;

	bool isDebugModeOn;

};

