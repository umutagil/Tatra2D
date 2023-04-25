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

#include "../AssetStore/AssetStore.h"

#include "../EventBus/EventBus.h"
#include "../Events/KeyPressedEvent.h"

#include <SDL.h>
#include <SDL_image.h>
#include <glm/glm.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_sdlrenderer.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

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
	ImGuiIO& io = ImGui::GetIO(); (void)io;
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
	LoadLevel(1);
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

		ImGui_ImplSDLRenderer_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();
		ImGui::ShowDemoWindow();
		ImGui::Render();

		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		//SDL_SetRenderDrawColor(renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
		//SDL_RenderClear(renderer);
		ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
		SDL_RenderPresent(renderer);
	}

	// Render to screen
	SDL_RenderPresent(renderer);
}

void Game::LoadLevel(const unsigned level)
{
	// Add systems
	registry->AddSystem<MovementSystem>();
	registry->AddSystem<RenderSystem>();
	registry->AddSystem<DebugRenderSystem>();
	registry->AddSystem<RenderTextSystem>();
	registry->AddSystem<AnimationSystem>();
	registry->AddSystem<CollisionSystem>();
	registry->AddSystem<DamageSystem>();
	registry->AddSystem<KeyboardControlSystem>();
	registry->AddSystem<CameraMovementSystem>();
	registry->AddSystem<ProjectileEmitSystem>();
	registry->AddSystem<ProjectileLifeCycleSystem>();
	registry->AddSystem<HealthDisplaySystem>();

	// Add assets to asset store
	assetStore->AddTexture(renderer, "tank-image", "./assets/images/tank-panther-right.png");
	assetStore->AddTexture(renderer, "truck-image", "./assets/images/truck-ford-right.png");
	assetStore->AddTexture(renderer, "chopper-image", "./assets/images/chopper-spritesheet.png");
	assetStore->AddTexture(renderer, "bullet-image", "./assets/images/bullet.png");
	assetStore->AddTexture(renderer, "radar-image", "./assets/images/radar.png");
	assetStore->AddFont("charriot-font-20", "./assets/fonts/charriot.ttf", 20);
	assetStore->AddFont("charriot-font-10", "./assets/fonts/charriot.ttf", 10);

	// Load the tilemap
	LoadTileMap();

	// Add character entities
	CreateCharacters();

	// Add UI entities
	Entity radar = registry->CreateEntity();
	radar.Tag("ui");
	radar.AddComponent<TransformComponent>(glm::vec2(windowWidth - 74.0f, 10.0f), glm::vec2(1.0f, 1.0f), 0.0f);
	radar.AddComponent<RigidBodyComponent>(glm::vec2(0.0f, 0.0f));
	radar.AddComponent<SpriteComponent>("radar-image", 64, 64, 5, true /* isFixed */);
	radar.AddComponent<AnimationComponent>(8, 5);

	Entity label = registry->CreateEntity();
	SDL_Color labelColor = { 10, 220, 200 };
	const bool isLabelFixed = true;
	label.AddComponent<TextLabelComponent>(glm::vec2(windowWidth / 2 - 100, 10), "Iyi Bayramlar!", "charriot-font", labelColor, isLabelFixed);

}

void Game::LoadTileMap()
{
	// Add texture to asset store
	assetStore->AddTexture(renderer, "tilemap-image", "./assets/tilemaps/jungle.png");

	// Load the map tile index info from file.

	std::ifstream tileMapFile("./assets/tilemaps/jungle.map");
	std::vector<std::string> indexStringLines;
	if (tileMapFile.is_open()) {
		while (tileMapFile) {
			std::string line;
			std::getline(tileMapFile, line);
			if (!line.empty()) {
				indexStringLines.push_back(line);
			}
		}
	}

	std::vector<std::vector<unsigned>> tileMapIndices(indexStringLines.size());
	for (size_t i = 0; i < indexStringLines.size(); ++i) {
		std::string tileIndex;
		std::stringstream stringStream(indexStringLines[i]);
		while (std::getline(stringStream, tileIndex, ',')) {
			if (!tileIndex.empty()) {
				tileMapIndices[i].emplace_back(std::stoul(tileIndex));
			}
		}
	}

	// Create tile entities

	SDL_Texture *mapTexture = assetStore->GetTexture("tilemap-image");
	int textureWidth, textureHeight;
	SDL_QueryTexture(mapTexture, nullptr, nullptr, &textureWidth, &textureHeight);
	const int tileSize = 32;
	const float tileScale = 2.0f;
	const int numTilesInTextureRow = textureWidth / tileSize;
	const int zIndex = 0;
	
	for (size_t i = 0; i < tileMapIndices.size(); ++i) {
		for (size_t j = 0; j < tileMapIndices[i].size(); ++j) {
			Entity tile = registry->CreateEntity();
			tile.Tag("tile");

			const glm::vec2 tilePos(j * (tileSize * tileScale), i * (tileSize * tileScale));
			tile.AddComponent<TransformComponent>(tilePos, glm::vec2(tileScale, tileScale), 0.0f);

			const unsigned tileIdx = tileMapIndices[i][j];
			const unsigned column = tileIdx % numTilesInTextureRow;
			const unsigned row = tileIdx / numTilesInTextureRow;
			const int srcRectX = tileSize * column;
			const int srcRectY = tileSize * row;
			tile.AddComponent<SpriteComponent>("tilemap-image", tileSize, tileSize, zIndex, false, srcRectX, srcRectY);
		}
	}

	mapWidth = static_cast<int>(tileMapIndices[0].size()) * tileSize * static_cast<int>(tileScale);
	mapHeight = static_cast<int>(tileMapIndices.size()) * tileSize * static_cast<int>(tileScale);
}

void Game::CreateCharacters()
{
	Entity chopper = registry->CreateEntity();
	const float chopperSpeed = 100.0f;
	chopper.Tag("player");
	chopper.AddComponent<TransformComponent>(glm::vec2(10.0f, 300.0f), glm::vec2(1.0f, 1.0f), 0.0f);
	chopper.AddComponent<RigidBodyComponent>(glm::vec2(0.0f, -chopperSpeed));
	chopper.AddComponent<SpriteComponent>("chopper-image", 32, 32, 3);
	chopper.AddComponent<AnimationComponent>(2, 15);
	chopper.AddComponent<BoxColliderComponent>(32, 32);
	chopper.AddComponent<KeyboardControlledComponent>(glm::vec2(0.0f, -chopperSpeed), glm::vec2(chopperSpeed, 0.0f), glm::vec2(0.0f, chopperSpeed), glm::vec2(-chopperSpeed, 0.0f));
	chopper.AddComponent<CameraFollowComponent>();
	chopper.AddComponent<ProjectileEmitterComponent>(glm::vec2(200.0f, 200.0f), 0.1f, 10.0f, 10, true);
	chopper.AddComponent<HealthComponent>(100);

	Entity tank = registry->CreateEntity();
	tank.Group("enemies");
	tank.AddComponent<TransformComponent>(glm::vec2(200.0f, 220.0f), glm::vec2(1.0f, 1.0f), 0.0f);
	tank.AddComponent<RigidBodyComponent>(glm::vec2(0.0f, 0.0f));
	tank.AddComponent<SpriteComponent>("tank-image", 32, 32, 2);
	tank.AddComponent<BoxColliderComponent>(32, 32);
	tank.AddComponent<ProjectileEmitterComponent>(glm::vec2(100.0f, 0.0f), 1.0f, 5.0f, 10, false);
	tank.AddComponent<HealthComponent>(80);

	Entity truck = registry->CreateEntity();
	truck.Group("enemies");
	truck.AddComponent<TransformComponent>(glm::vec2(800.0f, 500.0f), glm::vec2(1.0f, 1.0f), 0.0f);
	truck.AddComponent<RigidBodyComponent>(glm::vec2(0.0f, 0.0f));
	truck.AddComponent<SpriteComponent>("truck-image", 32, 32, 1);
	truck.AddComponent<BoxColliderComponent>(32, 32);
	truck.AddComponent<ProjectileEmitterComponent>(glm::vec2(0.0f, -100.0f), 2.0f, 5.0f, 10, false);
	truck.AddComponent<HealthComponent>(40);
}

void Game::InitializeCamera()
{
	camera.x = 0;
	camera.y = 0;
	camera.w = windowWidth;
	camera.h = windowHeight;
}

#endif // !GAME_H