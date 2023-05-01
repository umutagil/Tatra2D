#include "GameController.h"

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
#include <SDL_rect.h>
#include <glm/glm.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_sdlrenderer.h>

GameController::GameController(Game& game, Registry& registry)
	: BaseController(game, registry)
	, isDebugModeOn(false)
{
}

void GameController::Initialize(SDL_Renderer& renderer, sol::state& lua)
{
	AssetStore& assetStore = game.GetAssetStore();

	// Add systems
	registry.AddSystem<MovementSystem>();
	registry.AddSystem<RenderSystem>();
	registry.AddSystem<DebugRenderSystem>();
	registry.AddSystem<RenderGUISystem>();
	registry.AddSystem<RenderTextSystem>();
	registry.AddSystem<AnimationSystem>();
	registry.AddSystem<CollisionSystem>();
	registry.AddSystem<DamageSystem>();
	registry.AddSystem<KeyboardControlSystem>();
	registry.AddSystem<CameraMovementSystem>();
	registry.AddSystem<ProjectileEmitSystem>();
	registry.AddSystem<ProjectileLifeCycleSystem>();
	registry.AddSystem<HealthDisplaySystem>();
	registry.AddSystem<ScriptSystem>();

	registry.GetSystem<ScriptSystem>().CreateLuaBindings(lua);

	lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::os);
	LevelLoader levelLoader;
	levelLoader.LoadLevel(lua, registry, assetStore, renderer, 2);
}

void GameController::HandleEvent(SDL_Event& sdlEvent)
{
	if (sdlEvent.type == SDL_KEYDOWN) {
		EventBus& eventBus = game.GetEventBus();
		eventBus.EmitEvents<KeyPressedEvent>(sdlEvent.key.keysym.sym);

		if (sdlEvent.key.keysym.sym == SDLK_d) {
			isDebugModeOn = !isDebugModeOn;
		}
	}
}

void GameController::Update(const unsigned deltaTimeMs)
{
	const float deltaTimeSec = deltaTimeMs / 1000.0f;

	// Reset all the event handlers
	EventBus& eventBus = game.GetEventBus();
	eventBus.Reset();

	// Subscribe to events
	registry.GetSystem<DamageSystem>().SubscribeToEvents(eventBus);
	registry.GetSystem<MovementSystem>().SubscribeToEvents(eventBus);
	registry.GetSystem<KeyboardControlSystem>().SubscribeToEvents(eventBus);
	registry.GetSystem<ProjectileEmitSystem>().SubscribeToEvents(eventBus);

	// Update the registry to process pending entities
	registry.Update();

	// Update all systems
	registry.GetSystem<MovementSystem>().Update(deltaTimeSec);
	registry.GetSystem<AnimationSystem>().Update(deltaTimeSec);
	registry.GetSystem<CollisionSystem>().Update(eventBus);
	registry.GetSystem<CameraMovementSystem>().Update(game.GetCamera());
	registry.GetSystem<ProjectileEmitSystem>().Update(registry, deltaTimeSec);
	registry.GetSystem<ProjectileLifeCycleSystem>().Update(deltaTimeSec);
	registry.GetSystem<ScriptSystem>().Update(deltaTimeSec, static_cast<double>(game.GetElapsedTime()));
}

void GameController::Render(SDL_Renderer& renderer)
{
	AssetStore& assetStore = game.GetAssetStore();
	SDL_Rect& camera = game.GetCamera();

	registry.GetSystem<RenderSystem>().Update(renderer, assetStore, camera);
	registry.GetSystem<RenderTextSystem>().Update(renderer, assetStore, camera);
	registry.GetSystem<HealthDisplaySystem>().Update(renderer, assetStore, camera);

	if (isDebugModeOn) {
		registry.GetSystem<DebugRenderSystem>().Update(renderer, camera);
		registry.GetSystem<RenderGUISystem>().Update(registry, camera);
	}
}
