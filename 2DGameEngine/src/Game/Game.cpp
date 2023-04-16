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

#include "../Systems/MovementSystem.h"
#include "../Systems/RenderSystem.h"
#include "../Systems/AnimationSystem.h"
#include "../Systems/CollisionSystem.h"
#include "../Systems/DamageSystem.h"
#include "../Systems/DebugRenderSystem.h"
#include "../Systems/KeyboardControlSystem.h"

#include "../AssetStore/AssetStore.h"

#include "../EventBus/EventBus.h"
#include "../Events/KeyPressedEvent.h"

#include <SDL.h>
#include <SDL_image.h>
#include <glm/glm.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

Game::Game()
	: isRunning(false)
	, milisecsPreviousFrame(0)
	, window(nullptr)
	, renderer(nullptr)
	, windowWidth(0)
	, windowHeight(0)
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
		std::cerr << "Error initializing SDL." << std::endl;
		return;
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

	//SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

	isRunning = true;
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
	
	const float deltaTimeSec = (SDL_GetTicks64() - milisecsPreviousFrame) / 1000.0f;
	milisecsPreviousFrame = SDL_GetTicks64();

	// Reset all the event handlers
	eventBus->Reset();

	// Subscribe to events
	registry->GetSystem<DamageSystem>().SubscribeToEvents(*eventBus);
	registry->GetSystem<KeyboardControlSystem>().SubscribeToEvents(*eventBus);

	// Update the registry to process pending entities
	registry->Update();

	// Update all systems
	registry->GetSystem<MovementSystem>().Update(deltaTimeSec);
	registry->GetSystem<AnimationSystem>().Update(deltaTimeSec);
	registry->GetSystem<CollisionSystem>().Update(*eventBus);
}

void Game::Render()
{
	SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
	SDL_RenderClear(renderer);

	registry->GetSystem<RenderSystem>().Update(renderer, assetStore);
	if (isDebugModeOn) {
		registry->GetSystem<DebugRenderSystem>().Update(renderer);
	}

	// Render to screen
	SDL_RenderPresent(renderer);
}

void Game::Destroy()
{
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
}

void Game::LoadLevel(const unsigned level)
{
	// Add systems
	registry->AddSystem<MovementSystem>();
	registry->AddSystem<RenderSystem>();
	registry->AddSystem<DebugRenderSystem>();
	registry->AddSystem<AnimationSystem>();
	registry->AddSystem<CollisionSystem>();
	registry->AddSystem<DamageSystem>();
	registry->AddSystem<KeyboardControlSystem>();

	// Add assets to asset store
	assetStore->AddTexture(renderer, "tank-image", "./assets/images/tank-panther-right.png");
	assetStore->AddTexture(renderer, "truck-image", "./assets/images/truck-ford-right.png");
	assetStore->AddTexture(renderer, "chopper-image", "./assets/images/chopper-spritesheet.png");
	assetStore->AddTexture(renderer, "radar-image", "./assets/images/radar.png");

	// Load the tilemap
	LoadTileMap();

	// Add entities
	Entity chopper = registry->CreateEntity();
	chopper.AddComponent<TransformComponent>(glm::vec2(10.0f, 100.0f), glm::vec2(1.0f, 1.0f), 0.0f);
	chopper.AddComponent<RigidBodyComponent>(glm::vec2(0.0f, 0.0f));
	chopper.AddComponent<SpriteComponent>("chopper-image", 32, 32, 3);
	chopper.AddComponent<AnimationComponent>(2, 15);

	Entity radar = registry->CreateEntity();
	radar.AddComponent<TransformComponent>(glm::vec2(windowWidth - 74.0f, 10.0f), glm::vec2(1.0f, 1.0f), 0.0f);
	radar.AddComponent<RigidBodyComponent>(glm::vec2(0.0f, 0.0f));
	radar.AddComponent<SpriteComponent>("radar-image", 64, 64, 5);
	radar.AddComponent<AnimationComponent>(8, 5);
	const float chopperSpeed = 50.0f;
	chopper.AddComponent<KeyboardControlledComponent>(glm::vec2(0.0f, -chopperSpeed), glm::vec2(chopperSpeed, 0.0f), glm::vec2(0.0f, chopperSpeed), glm::vec2(-chopperSpeed, 0.0f));

	Entity tank = registry->CreateEntity();
	tank.AddComponent<TransformComponent>(glm::vec2(10.0f, 10.0f), glm::vec2(1.0f, 1.0f), 0.0f);
	tank.AddComponent<RigidBodyComponent>(glm::vec2(50.0f, 0.0f));
	tank.AddComponent<SpriteComponent>("tank-image", 32, 32, 2);
	tank.AddComponent<BoxColliderComponent>(32, 32);

	Entity truck = registry->CreateEntity();
	truck.AddComponent<TransformComponent>(glm::vec2(800.0f, 10.0f), glm::vec2(1.0f, 1.0f), 0.0f);
	truck.AddComponent<RigidBodyComponent>(glm::vec2(-50.0f, 0.0f));
	truck.AddComponent<SpriteComponent>("truck-image", 32, 32, 1);
	truck.AddComponent<BoxColliderComponent>(32, 32);
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
			indexStringLines.push_back(line);
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
	const float tileScale = 1.5f;
	const int numTilesInTextureRow = textureWidth / tileSize;
	const int zIndex = 0;
	
	for (size_t i = 0; i < tileMapIndices.size(); ++i) {
		for (size_t j = 0; j < tileMapIndices[i].size(); ++j) {
			Entity tile = registry->CreateEntity();

			const glm::vec2 tilePos(j * (tileSize * tileScale), i * (tileSize * tileScale));
			tile.AddComponent<TransformComponent>(tilePos, glm::vec2(tileScale, tileScale), 0.0f);

			const unsigned tileIdx = tileMapIndices[i][j];
			const unsigned column = tileIdx % numTilesInTextureRow;
			const unsigned row = tileIdx / numTilesInTextureRow;
			const int srcRectX = tileSize * column;
			const int srcRectY = tileSize * row;
			tile.AddComponent<SpriteComponent>("tilemap-image", tileSize, tileSize, zIndex, srcRectX, srcRectY);
		}
	}
}

#endif // !GAME_H