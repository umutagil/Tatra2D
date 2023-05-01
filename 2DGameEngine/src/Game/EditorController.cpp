#include "EditorController.h"

#include "Game.h"

#include "../Logger/Logger.h"
#include "../ECS/ECS.h"

#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/KeyboardControlComponent.h"
#include "../Components/CameraFollowComponent.h"
#include "../Components/TextLabelComponent.h"

#include "../Systems/RenderEditorSystem.h"
#include "../Systems/KeyboardControlSystem.h"
#include "../Systems/CameraMovementSystem.h"
#include "../Systems/RenderTextSystem.h"
#include "../Systems/RenderEditorGUISystem.h"
#include "../Systems/ScriptSystem.h"

#include "LevelLoader.h"

#include "../EventBus/EventBus.h"
#include "../Events/KeyPressedEvent.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_rect.h>
#include <glm/glm.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_sdlrenderer.h>

EditorController::EditorController(Game& game, Registry& registry)
	: BaseController(game, registry)
	, sceneManager()
{
}

void EditorController::Initialize(SDL_Renderer& renderer, sol::state& lua)
{
	AssetStore& assetStore = game.GetAssetStore();

	// Add systems
	registry.AddSystem<RenderEditorSystem>();
	registry.AddSystem<RenderEditorGUISystem>();
	registry.AddSystem<RenderTextSystem>();
	registry.AddSystem<KeyboardControlSystem>();
	registry.AddSystem<CameraMovementSystem>();
	registry.AddSystem<ScriptSystem>();

	registry.GetSystem<ScriptSystem>().CreateLuaBindings(lua);

	lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::os);
}

void EditorController::HandleEvent(SDL_Event& sdlEvent)
{
}

void EditorController::Update(const unsigned deltaTimeMs)
{
	const float deltaTimeSec = deltaTimeMs / 1000.0f;

	// Reset all the event handlers
	EventBus& eventBus = game.GetEventBus();
	eventBus.Reset();

	// Subscribe to events
	registry.GetSystem<KeyboardControlSystem>().SubscribeToEvents(eventBus);

	// Update the registry to process pending entities
	registry.Update();

	// Update all systems
	registry.GetSystem<CameraMovementSystem>().Update(game.GetCamera());
	registry.GetSystem<ScriptSystem>().Update(deltaTimeSec, static_cast<double>(game.GetElapsedTime()));
}

void EditorController::Render(SDL_Renderer& renderer)
{
	AssetStore& assetStore = game.GetAssetStore();
	SDL_Rect& camera = game.GetCamera();

	registry.GetSystem<RenderEditorSystem>().Update(sceneManager, renderer, assetStore, camera);
	registry.GetSystem<RenderTextSystem>().Update(renderer, assetStore, camera);
	registry.GetSystem<RenderEditorGUISystem>().Update(sceneManager, renderer, assetStore, camera);
}
