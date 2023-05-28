#include "LevelLoader.h"

#include "../ECS/ECS.h"
#include "../AssetStore/AssetStore.h"
#include "Game.h"
#include "../Logger/Logger.h"
#include "../Game/SceneManager.h"

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
#include "../Components/ScriptComponent.h"

#include <sol/sol.hpp>

#include <SDL.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <assert.h>

LevelLoader::LevelLoader()
{
}

LevelLoader::~LevelLoader()
{
}

void LevelLoader::LoadLevel(sol::state& lua, Registry& registry, AssetStore& assetStore, SDL_Renderer& renderer, const int levelIndex)
{
	const std::string levelFileName("./assets/scripts/level" + std::to_string(levelIndex) + ".lua");

	sol::load_result levelScript = lua.load_file(levelFileName);
	if (!levelScript.valid()) {
		sol::error err = levelScript;
		std::string errorMsg(err.what());
		Logger::Log("Error loading the lua script: " + errorMsg);
		return;
	}

	lua.script_file(levelFileName);
	sol::table level = lua["level"];

	// Read the level assets

	const sol::table& assets = level["assets"];

	for (size_t i = 0; /* noop */; ++i) {

		sol::optional<sol::table> assetOptional = assets[i];
		if (assetOptional == sol::nullopt) {
			break;
		}

		const sol::table& asset = assetOptional.value();
		const std::string assetType = asset["type"];
		const std::string assetId = asset["id"];
		const std::string assetFilePath = asset["file"];

		if (assetType == "texture") {
			assetStore.AddTexture(renderer, assetId, assetFilePath);
			Logger::Log("A new texture assed was added to the asset store with id: " + assetId);
		}
		else if (assetType == "font") {
			assetStore.AddFont(assetId, assetFilePath, asset["font_size"]);
			Logger::Log("A new font assed was added to the asset store with id: " + assetId);
		}
	}

	// Read the level tilemap

	const sol::table& map = level["tilemap"];
	const std::string mapFilePath = map["map_file"];
	const std::string mapTextureAssetId = map["texture_asset_id"];
	int tileSize = map["tile_size"];
	int numRows = map["num_rows"];
	int numCols = map["num_cols"];
	const float mapScale = map["scale"];

	// Load the map tile index info from map file.

	std::ifstream tileMapFile(mapFilePath);
	std::vector<std::string> indexStringLines;
	if (tileMapFile.is_open()) {

		std::string firstLine;
		std::getline(tileMapFile, firstLine);
		std::stringstream stringStream(firstLine);

		std::string numRowsString;
		std::getline(stringStream, numRowsString, ',');
		numRows = std::stoi(numRowsString);

		std::string numColsString;
		std::getline(stringStream, numColsString, ',');
		numCols = std::stoi(numColsString);
		
		std::string tileSizeString;
		std::getline(stringStream, tileSizeString, ',');
		tileSize = std::stoi(tileSizeString);

		while (tileMapFile) {
			std::string line;
			std::getline(tileMapFile, line);
			if (!line.empty()) {
				indexStringLines.push_back(line);
			}
		}
	}

	std::vector<std::vector<int>> tileMapIndices(numCols);
	for (size_t i = 0; i < numCols; ++i) {
		std::string tileIndex;
		std::stringstream stringStream(indexStringLines[i]);
		for (size_t j = 0; j < numRows; ++j) {
			if (std::getline(stringStream, tileIndex, ',')) {
				if (!tileIndex.empty()) {
					tileMapIndices[i].emplace_back(std::stoi(tileIndex));
				}
			}
		}
	}

	SDL_Texture* mapTexture = assetStore.GetTexture(mapTextureAssetId);
	int textureWidth, textureHeight;
	SDL_QueryTexture(mapTexture, nullptr, nullptr, &textureWidth, &textureHeight);
	const int numTilesInTextureRow = textureWidth / tileSize;
	const int zIndex = 0;

	// Create tile entities

	for (size_t i = 0; i < tileMapIndices.size(); ++i) {
		for (size_t j = 0; j < tileMapIndices[i].size(); ++j) {
			Entity tile = registry.CreateEntity();
			tile.Tag("tile");
			tile.Group("tiles");

			const glm::vec2 tilePos(j * (tileSize * mapScale), i * (tileSize * mapScale));
			tile.AddComponent<TransformComponent>(tilePos, glm::vec2(mapScale, mapScale), 0.0f);

			const int tileIdx = tileMapIndices[i][j];
			if (tileIdx < 0) {
				continue;
			}

			const unsigned column = tileIdx % numTilesInTextureRow;
			const unsigned row = tileIdx / numTilesInTextureRow;
			const int srcRectX = tileSize * column;
			const int srcRectY = tileSize * row;
			tile.AddComponent<SpriteComponent>(mapTextureAssetId, tileSize, tileSize, zIndex, false, srcRectX, srcRectY);
		}
	}

	Game::mapWidth = static_cast<int>(tileMapIndices[0].size()) * tileSize * static_cast<int>(mapScale);
	Game::mapHeight = static_cast<int>(tileMapIndices.size()) * tileSize * static_cast<int>(mapScale);

	// Read the character information

	const sol::table& entities = level["entities"];

	for (size_t i = 0; /* noop */; ++i) {
		sol::optional<sol::table> entityOptional = entities[i];
		if (entityOptional == sol::nullopt) {
			break;
		}

		const sol::table& entityInfo = entityOptional.value();
		Entity newEntity = registry.CreateEntity();

		std::optional<std::string> tagOptional = entityInfo["tag"];
		if (tagOptional != sol::nullopt) {
			newEntity.Tag(tagOptional.value());
		}

		std::optional<std::string> groupOptional = entityInfo["group"];
		if (groupOptional != sol::nullopt) {
			newEntity.Group(groupOptional.value());
		}

		sol::optional<sol::table> componentsOptional = entityInfo["components"];
		if (componentsOptional == sol::nullopt) {
			// No component. Is it normal?
			continue;
		}

		const sol::table& components = componentsOptional.value();

		// Transform

		sol::optional<sol::table> transformOptional = components["transform"];
		if (transformOptional != sol::nullopt) {
			const sol::table& transform = transformOptional.value();
			newEntity.AddComponent<TransformComponent>(
				glm::vec2(transform["position"]["x"], transform["position"]["y"]),
				glm::vec2(transform["scale"]["x"].get_or(1.0f), transform["scale"]["y"].get_or(1.0f)),
				transform["rotation"].get_or(0.0f)
			);
		}

		// Rigidbody

		sol::optional<sol::table> rigidbodyOptional = components["rigidbody"];
		if (rigidbodyOptional != sol::nullopt) {
			const sol::table& rigidbody = rigidbodyOptional.value();
			newEntity.AddComponent<RigidBodyComponent>(
				glm::vec2(rigidbody["velocity"]["x"].get_or(0.0f), rigidbody["velocity"]["y"].get_or(0.0f))
			);
		}

		// Sprite

		sol::optional<sol::table> spriteOptional = components["sprite"];
		if (spriteOptional != sol::nullopt) {
			const sol::table& sprite = spriteOptional.value();
			newEntity.AddComponent<SpriteComponent>(
				sprite["texture_asset_id"],
				sprite["width"],
				sprite["height"],
				static_cast<int>(sprite["z_index"].get_or(0)),
				sprite["fixed"].get_or(false),
				static_cast<int>(sprite["src_rect_x"].get_or(0)),
				static_cast<int>(sprite["src_rect_y"].get_or(0))
			);
		}

		// Animation

		sol::optional<sol::table> animationOptional = components["animation"];
		if (animationOptional != sol::nullopt) {
			const sol::table& animation = animationOptional.value();
			newEntity.AddComponent<AnimationComponent>(
				animation["num_frames"].get_or(1),
				animation["speed_rate"].get_or(1)
			);
		}

		// Box Collider

		sol::optional<sol::table> boxColliderOptional = components["box_collider"];
		if (boxColliderOptional != sol::nullopt) {
			const sol::table& boxCollider = boxColliderOptional.value();
			newEntity.AddComponent<BoxColliderComponent>(
				boxCollider["width"],
				boxCollider["height"],
				glm::vec2(boxCollider["offset"]["x"].get_or(0), boxCollider["offset"]["y"].get_or(0))
			);
		}

		// Health

		sol::optional<sol::table> healthOptional = components["health"];
		if (healthOptional != sol::nullopt) {
			const sol::table& health = healthOptional.value();
			newEntity.AddComponent<HealthComponent>(
				health["health"].get_or(100)
			);
		}

		// Projectile Emitter

		sol::optional<sol::table> projectileEmitterOptional = components["projectile_emitter"];
		if (projectileEmitterOptional != sol::nullopt) {
			const sol::table& projectileEmitter = projectileEmitterOptional.value();
			newEntity.AddComponent<ProjectileEmitterComponent>(
				glm::vec2(projectileEmitter["projectile_velocity"]["x"].get_or(0), projectileEmitter["projectile_velocity"]["y"].get_or(0)),
				projectileEmitter["repeat_frequency"].get_or(0.0f),
				projectileEmitter["projectile_duration"].get_or(0.0f),
				static_cast<int>(projectileEmitter["hit_damage"].get_or(0)),
				projectileEmitter["friendly"].get_or(false)
			);
		}

		// Keyboard Controller

		sol::optional<sol::table> keyboardControllerOptional = components["keyboard_controller"];
		if (keyboardControllerOptional != sol::nullopt) {
			const sol::table& keyboardController = keyboardControllerOptional.value();
			newEntity.AddComponent<KeyboardControlledComponent>(
				glm::vec2(keyboardController["up_velocity"]["x"].get_or(0.0f), keyboardController["up_velocity"]["y"].get_or(0.0f)),
				glm::vec2(keyboardController["right_velocity"]["x"].get_or(0.0f), keyboardController["right_velocity"]["y"].get_or(0.0f)),
				glm::vec2(keyboardController["down_velocity"]["x"].get_or(0.0f), keyboardController["down_velocity"]["y"].get_or(0.0f)),
				glm::vec2(keyboardController["left_velocity"]["x"].get_or(0.0f), keyboardController["left_velocity"]["y"].get_or(0.0f))
			);
		}

		// Camera Follow

		sol::optional<sol::table> cameraFollowOptional = components["camera_follow"];
		if (cameraFollowOptional != sol::nullopt) {
			const sol::table& cameraFollow = cameraFollowOptional.value();
			if (cameraFollow["follow"]) {
				newEntity.AddComponent<CameraFollowComponent>();
			}
		}

		// Script

		sol::optional<sol::table> scriptOptional = components["on_update_script"];
		if (scriptOptional != sol::nullopt) {
			const sol::table& script = scriptOptional.value();
			sol::function func = script[0];
			newEntity.AddComponent<ScriptComponent>(func);
		}

	}



	/*Entity label = registry.CreateEntity();
	SDL_Color labelColor = { 10, 220, 200 };
	const bool isLabelFixed = true;
	label.AddComponent<TextLabelComponent>(glm::vec2(Game::windowWidth / 2 - 100, 10), "Iyi Bayramlar!", "charriot-font", labelColor, isLabelFixed);*/
}

void LevelLoader::SaveMap(const std::string& mapFileName, Registry& registry, SceneManager& sceneManager)
{
	std::ofstream file(mapFileName);
	if (!file.is_open()) {
		Logger::Log("Could not open the file: " + mapFileName);
		return;
	}

	const GridProperties grid = sceneManager.GetGridProperties();
	file << grid.cellCountX << "," << grid.cellCountY << "," << grid.cellSize << std::endl;

	const auto tileMapIndices = sceneManager.GetTileMapIndices();

	for (size_t yIdx = 0; yIdx < tileMapIndices.size(); ++yIdx) {
		for (size_t xIdx = 0; xIdx < tileMapIndices[yIdx].size(); ++xIdx) {
			file << tileMapIndices[yIdx][xIdx];
			if (xIdx < tileMapIndices[yIdx].size() - 1) {
				file << ",";
			}
		}

		file << std::endl;
	} 

	file.close();

	Logger::Log("Map saved: " + mapFileName);
}
