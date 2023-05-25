#pragma once

#include <memory>

#include <SDL_rect.h>
#include <sol/sol.hpp>

#include "../ECS/ECS.h"

using u64 = unsigned __int64;

const unsigned FPS = 120;
const unsigned MILISEC_PER_FRAME = 1000 / FPS;
const unsigned FRAME_LIMITER_MAX_DELTA_TIME = 4 * MILISEC_PER_FRAME; // milisecs

struct SDL_Window;
struct SDL_Renderer;

class BaseController;
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

	inline sol::state& GetLuaState() { return lua; }
	inline AssetStore& GetAssetStore() { return *assetStore; }
	inline EventBus& GetEventBus() { return *eventBus; }
	inline SDL_Rect& GetCamera() { return camera; }
	inline void SetCamera(SDL_Rect newCamera) { camera = newCamera; }

	u64 GetElapsedTime() const;


private:
	void InitializeCamera();

public:
	static int windowWidth;
	static int windowHeight;
	static int mapWidth;
	static int mapHeight;

private:
	
	std::unique_ptr<BaseController> controller;

	bool isRunning;
	u64 elapsedTimeMs = 0;

	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Rect camera;

	sol::state lua;

	std::unique_ptr<Registry> registry;
	std::unique_ptr<AssetStore> assetStore;
	std::unique_ptr<EventBus> eventBus;

};

