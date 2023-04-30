#ifndef GAME_H
#define GAME_H

#include "Game.h"

#include "../Logger/Logger.h"
#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/AnimationComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/KeyboardControlComponent.h"
#include "../Components/CameraFollowComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/HealthComponent.h"
#include "../Components/TextLabelComponent.h"

#include "../Systems/MovementSystem.h"
#include "../Systems/RenderSystem.h"
#include "../Systems/AnimationSystem.h"
#include "../Systems/CollisionSystem.h"
#include "../Systems/DamageSystem.h"
#include "../Systems/DebugRenderSystem.h"
#include "../Systems/KeyboardControlSystem.h"
#include "../Systems/CameraMovementSystem.h"
#include "../Systems/ProjectileEmitSystem.h"
#include "../Systems/ProjectileLifeCycleSystem.h"
#include "../Systems/RenderTextSystem.h"
#include "../Systems/HealthDisplaySystem.h"
#include "../Systems/RenderGUISystem.h"
#include "../Systems/ScriptSystem.h"

#include "../AssetStore/AssetStore.h"

#include "../EventBus/EventBus.h"
#include "../Events/KeyPressedEvent.h"

#include "LevelLoader.h"

#include <SDL.h>
#include <SDL_image.h>
#include <glm/glm.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_sdlrenderer.h>

#include <string>

int Game::windowWidth;
int Game::windowHeight;
int Game::mapWidth;
int Game::mapHeight;

Game::Game()
	: isRunning(false)
	, milisecsPreviousFrame(0)
	, window(nullptr)
	, renderer(nullptr)
	, camera()
	, registry()
	, assetStore()
	, eventBus()
	, isDebugModeOn(false)
{
	Logger::Log("Game constructor called.");

	registry = std::make_unique<Registry>();
	assetStore = std::make_unique<AssetStore>();
	eventBus = std::make_unique<EventBus>();
}

Game::~Game()
{
	Logger::Log("Game destructor called.");
}

void Game::Initialize()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		Logger::Log("Error initializing SDL.");
		return;
	}

	if (TTF_Init() != 0) {
		Logger::Log("Error initializing SDL TTF.");
	}
	
	SDL_DisplayMode displayMode;
	if (SDL_GetCurrentDisplayMode(0, &displayMode) != 0) {
		Logger::Err("Error getting current display mode.");
	}

	windowWidth = displayMode.w;
	windowHeight = displayMode.h;

	windowWidth = 1200;
	windowHeight = 900;

	window = SDL_CreateWindow(
		nullptr,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		windowWidth,
		windowHeight,
		SDL_WINDOW_BORDERLESS
	);

	if (!window) {
		Logger::Err("Error creating SDL window.");
		return;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!renderer) {
		Logger::Err("Error creating SDL renderer.");
		return;
	}

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

	ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer_Init(renderer);

	InitializeCamera();

	isRunning = true;
}

void Game::Destroy()
{
	ImGui_ImplSDLRenderer_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
}

void Game::Run()
{
	Setup();

	while (isRunning) {
		ProcessInput();
		Update();
		Render();
	}
}

void Game::ProcessInput()
{
	SDL_Event sdlEvent;
	while (SDL_PollEvent(&sdlEvent))
	{
		ImGui_ImplSDL2_ProcessEvent(&sdlEvent);

		switch (sdlEvent.type) {
			case SDL_QUIT:
				isRunning = false;
				break;
			case SDL_KEYDOWN: {
				if (sdlEvent.key.keysym.sym == SDLK_ESCAPE) {
					isRunning = false;
				}
				else if (sdlEvent.key.keysym.sym == SDLK_d) {
					isDebugModeOn = !isDebugModeOn;
				}

				eventBus->EmitEvents<KeyPressedEvent>(sdlEvent.key.keysym.sym);
			}

				break;
			default:
				break;
		}
	}
}

void Game::Setup()
{
	// Add systems
	registry->AddSystem<MovementSystem>();
	registry->AddSystem<RenderSystem>();
	registry->AddSystem<DebugRenderSystem>();
	registry->AddSystem<RenderGUISystem>();
	registry->AddSystem<RenderTextSystem>();
	registry->AddSystem<AnimationSystem>();
	registry->AddSystem<CollisionSystem>();
	registry->AddSystem<DamageSystem>();
	registry->AddSystem<KeyboardControlSystem>();
	registry->AddSystem<CameraMovementSystem>();
	registry->AddSystem<ProjectileEmitSystem>();
	registry->AddSystem<ProjectileLifeCycleSystem>();
	registry->AddSystem<HealthDisplaySystem>();
	registry->AddSystem<ScriptSystem>();

	registry->GetSystem<ScriptSystem>().CreateLuaBindings(lua);

	lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::os);
	LevelLoader levelLoader;
	levelLoader.LoadLevel(lua, *registry, *assetStore, *renderer, 2);
}

void Game::Update()
{
	// If we are running faster than FPS, then waste some time by yielding to match FPS.
	const Uint64 deltaTimeMilisec = SDL_GetTicks64() - milisecsPreviousFrame;
	if (deltaTimeMilisec < MILISEC_PER_FRAME) {
		const Uint32 timeToWait = static_cast<Uint32>(MILISEC_PER_FRAME - deltaTimeMilisec);
		SDL_Delay(timeToWait);
	}
	
	float deltaTimeSec = (SDL_GetTicks64() - milisecsPreviousFrame) / 1000.0f;
	milisecsPreviousFrame = SDL_GetTicks64();

	if (deltaTimeSec > FRAME_LIMITER_MAX_DELTA_TIME) {
		deltaTimeSec = FRAME_LIMITER_MAX_DELTA_TIME;
	}

	// Reset all the event handlers
	eventBus->Reset();

	// Subscribe to events
	registry->GetSystem<DamageSystem>().SubscribeToEvents(*eventBus);
	registry->GetSystem<MovementSystem>().SubscribeToEvents(*eventBus);
	registry->GetSystem<KeyboardControlSystem>().SubscribeToEvents(*eventBus);
	registry->GetSystem<ProjectileEmitSystem>().SubscribeToEvents(*eventBus);

	// Update the registry to process pending entities
	registry->Update();

	// Update all systems
	registry->GetSystem<MovementSystem>().Update(deltaTimeSec);
	registry->GetSystem<AnimationSystem>().Update(deltaTimeSec);
	registry->GetSystem<CollisionSystem>().Update(*eventBus);
	registry->GetSystem<CameraMovementSystem>().Update(camera);
	registry->GetSystem<ProjectileEmitSystem>().Update(registry, deltaTimeSec);
	registry->GetSystem<ProjectileLifeCycleSystem>().Update(deltaTimeSec);
	registry->GetSystem<ScriptSystem>().Update(deltaTimeSec, static_cast<double>(SDL_GetTicks64()));
}

void Game::Render()
{
	SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
	SDL_RenderClear(renderer);

	registry->GetSystem<RenderSystem>().Update(*renderer, *assetStore, camera);
	registry->GetSystem<RenderTextSystem>().Update(*renderer, *assetStore, camera);
	registry->GetSystem<HealthDisplaySystem>().Update(*renderer, *assetStore, camera);
	if (isDebugModeOn) {
		registry->GetSystem<DebugRenderSystem>().Update(*renderer, camera);
		registry->GetSystem<RenderGUISystem>().Update(*registry, camera);
	}

	// Render to screen
	SDL_RenderPresent(renderer);
}

void Game::InitializeCamera()
{
	camera.x = 0;
	camera.y = 0;
	camera.w = windowWidth;
	camera.h = windowHeight;
}

#endif // !GAME_H