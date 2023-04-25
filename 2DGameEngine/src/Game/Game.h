#pragma once

#include <cstdint>
#include <memory>

#include <SDL_rect.h>

#include "../ECS/ECS.h"

const unsigned FPS = 120;
const unsigned MILISEC_PER_FRAME = 1000 / FPS;
const float FRAME_LIMITER_MAX_DELTA_TIME = 4.0f / FPS;

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
	void Destroy();
	void Run();
	void ProcessInput();
	void Setup();
	void Update();
	void Render();

private:
	void LoadLevel(const unsigned level);
	void LoadTileMap();
	void CreateCharacters();
	void InitializeCamera();

public:
	static int windowWidth;
	static int windowHeight;
	static int mapWidth;
	static int mapHeight;

private:
	
	bool isRunning;
	std::uint64_t milisecsPreviousFrame = 0;

	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Rect camera;

	std::unique_ptr<Registry> registry;
	std::unique_ptr<AssetStore> assetStore;
	std::unique_ptr<EventBus> eventBus;

	bool isDebugModeOn;

};

