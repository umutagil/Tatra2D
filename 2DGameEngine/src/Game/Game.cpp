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

#include "GameController.h"

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
	, elapsedTimeMs(0)
	, window(nullptr)
	, renderer(nullptr)
	, camera()
	, registry()
	, assetStore()
	, eventBus()
	, controller()
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

	controller = std::make_unique<GameController>(*this, *registry);

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

		controller->HandleEvent(sdlEvent);

		switch (sdlEvent.type) {
			case SDL_QUIT:
				isRunning = false;
				break;
			case SDL_KEYDOWN: {
				if (sdlEvent.key.keysym.sym == SDLK_ESCAPE) {
					isRunning = false;
				}
				break;
			}
			default:
				break;
		}
	}
}

void Game::Setup()
{
	controller->Initialize(*renderer, lua);
}

void Game::Update()
{
	// If we are running faster than FPS, then waste some time by yielding to match FPS.
	unsigned deltaTimeMs = static_cast<unsigned>(SDL_GetTicks64() - elapsedTimeMs);
	if (deltaTimeMs < MILISEC_PER_FRAME) {
		const Uint32 timeToWait = static_cast<Uint32>(MILISEC_PER_FRAME - deltaTimeMs);
		SDL_Delay(timeToWait);
	}
	
	deltaTimeMs = static_cast<unsigned>(SDL_GetTicks64() - elapsedTimeMs);
	elapsedTimeMs = SDL_GetTicks64();

	if (deltaTimeMs > FRAME_LIMITER_MAX_DELTA_TIME) {
		deltaTimeMs = FRAME_LIMITER_MAX_DELTA_TIME;
	}

	controller->Update(deltaTimeMs);
}

void Game::Render()
{
	SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
	SDL_RenderClear(renderer);

	controller->Render(*renderer);

	// Render to screen
	SDL_RenderPresent(renderer);
}

u64 Game::GetElapsedTime() const
{
	return elapsedTimeMs;
}

void Game::InitializeCamera()
{
	camera.x = 0;
	camera.y = 0;
	camera.w = windowWidth;
	camera.h = windowHeight;
}

#endif // !GAME_H